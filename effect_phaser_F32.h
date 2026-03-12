#ifndef _effect_phaser_F32_h_
#define _effect_phaser_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectPhaser_F32 :
  public AudioStream_F32
{
//GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
//GUI: shortName:delay  
  public:
    AudioEffectPhaser_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
      init(AUDIO_SAMPLE_RATE_EXACT);
    }

    AudioEffectPhaser_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray), _lfo(settings.sample_rate_Hz)
    {
      init(settings.sample_rate_Hz);
    }

    void rate(float rate = 0.2f)
    {
      rate = rate < 0.0f ? 0.0f : rate > 10.0f ? 10.0f : rate;
      _lfo.freq(rate);
    }

    void smoothing(float ms = 20.0f)
    {
      if (ms > 0.0f)
      {
        if (ms > 10000.0f) ms = 10000.0f;
        _smooth = 1.0f - expf(-3.1699f / (_sample_rate_Hz * ms * 0.001f));
      }
      else _smooth = 1.0f;
    }

    void dry(float dry = 0.7f)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    void wet(float wet = 0.7f)
    {
      _wet = wet < 0.0f ? 0.0f : wet > 1.0f ? 1.0f : wet;
    }

    void frequency(float f1 = 50.0f, float f2 = 1000.0f)
    {
      float a0max = coeff(f2);
      float a0min = coeff(f1);

      _gz = a0max;
      _gx = a0min - a0max;
    }

    void resonance(float res = 0.0f)
    {
      _res = (res < 0.0f ? 0.0f : res > 1.0f ? 1.0f : res) * 0.9f;
    }

    void stages(int stages)
    {
      _stages = stages < 0 ? 0 : stages > _maxStages ? _maxStages : stages;
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (_stages > 0)
      {
        float *p = block->data;
        float *end = p + block->length;
        do
        {
          float sample = *p;

          _x += (_lfo.next() - _x) * _smooth;

          float g = (_x * _gx) + _gz;          
          float s_in = sample;

          int s = 0;
          float s_out = _s[s] - (g * s_in * 0.995f);
          _s[s++] = s_in + (g * s_out * 0.995f);
          s_in = s_out + _fbk;
          _fbk = 0.0f;
          
          for (int i = 1; i < _stages - 1; i++)
          {
              s_out = _s[s] - (g * s_in);
              _s[s++] = s_in + (g * s_out);
              s_in = s_out;            
          }

          s_out = _s[s] - (g * s_in * 0.98f);
          _s[s] = s_in + (g * s_out * 0.98f);
          _fbk = s_out * _res;

          *p++ = (sample * _dry) + (s_out * _wet);
        }
        while (p < end);
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    float _sample_rate_Hz;
    TriangleLfo _lfo;
    float _x;
    float _gz;
    float _gx;
    float _smooth;
    static const int _maxStages = 20;
    int _stages;
    float _s[_maxStages];
    float _fbk;
    float _res;
    float _dry;
    float _wet;

    float coeff(float f)
    {
      float K = tanf(f * (float)PI / _sample_rate_Hz);
      return (1.0f - K) / (1.0f + K);
    }

    void init(float sample_rate_Hz)
    {
      _sample_rate_Hz = sample_rate_Hz;
      rate();
      smoothing();
      dry();
      wet();
      frequency();
      resonance();
    }
};

#endif
