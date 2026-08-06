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
      AudioStream_F32(1, inputQueueArray),
      _sample_rate_Hz(AUDIO_SAMPLE_RATE_EXACT)
    {
    }

    AudioEffectPhaser_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _sample_rate_Hz(settings.sample_rate_Hz),
      _lfo(settings.sample_rate_Hz)
    {
    }

    FLASHMEM void begin()
    {
      rate();
      smoothing();
      dry();
      wet();
      frequency();
      resonance();
    }

    FLASHMEM void rate(float rate = 0.2f)
    {
      rate = rate < 0.0f ? 0.0f : rate > 1.0f ? 1.0f : rate;
      _lfo.freq((rate * 9.9f) + 0.1f);
    }

    FLASHMEM void smoothing(float ms = 20.0f)
    {
      if (ms > 0.0f)
      {
        if (ms > 10000.0f) ms = 10000.0f;
        _smooth = 1.0f - expf(-3.1699f / (_sample_rate_Hz * ms * 0.001f));
      }
      else _smooth = 1.0f;
    }

    FLASHMEM void dry(float dry = 0.7f)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    FLASHMEM void wet(float wet = 0.7f)
    {
      _wet = wet < 0.0f ? 0.0f : wet > 1.0f ? 1.0f : wet;
    }

    FLASHMEM void frequency(float f1 = 50.0f, float f2 = 1000.0f)
    {
      float a0max = coeff(f2);
      float a0min = coeff(f1);

      _gz = a0max;
      _gx = a0min - a0max;
    }

    FLASHMEM void resonance(float res = 0.0f)
    {
      _res = (res < 0.0f ? 0.0f : res > 1.0f ? 1.0f : res) * 0.5f;
    }

    FLASHMEM void stages(int stages)
    {
      _stages = stages < 2 ? 2 : stages > _maxStages ? _maxStages : stages;
    }

    FLASHMEM void enable(bool enable)
    {
      _enable = enable;
    }

    virtual void update(void)
    {
      if (!_enable)
      {
        audio_block_f32_t *bp = AudioStream_F32::receiveReadOnly_f32(0);
        if (!bp) return;
        AudioStream_F32::transmit(bp, 0);
        AudioStream_F32::release(bp);
        return;
      }

      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      int stages = _stages;
      float state[_maxStages];
      for (int i = 0; i < stages; i++) state[i] = _state[i];

      float x = _x;
      float gx = _gx;
      float gz = _gz;
      float smooth = _smooth;

      float fbk = _fbk;
      float res = _res;
      float dry = _dry;
      float wet = _wet;

      float sample, g, s_in, s_out;

      float *p = block->data;
      float *end = p + block->length;
      do
      {
        sample = *p;

        x += (_lfo.next() - x) * smooth;

        g = (x * gx) + gz;          
        s_in = sample;

        s_out = state[0] - (g * s_in);
        state[0] = (g * s_out) + s_in;
        s_in = s_out - fbk;

        for (int s = 1; s < stages; s++)
        {
            s_out = state[s] - (g * s_in);
            state[s] = (g * s_out) + s_in;
            s_in = s_out;
        }
       
        fbk = s_out * res;

        *p++ = (sample * dry) + (s_out * wet);
      }
      while (p < end);

      for (int i = 0; i < stages; i++) _state[i] = state[i];
      _x = x;
      _fbk = fbk;

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    const float _sample_rate_Hz;
    TriangleLfo _lfo;
    float _x;
    float _gz;
    float _gx;
    float _smooth;
    static const int _maxStages = 20;
    int _stages;
    float _state[_maxStages];
    float _fbk;
    float _res;
    float _dry;
    float _wet;
    bool _enable;

    float coeff(float f)
    {
      float K = tanf(f * (float)PI / _sample_rate_Hz);
      return (1.0f - K) / (1.0f + K);
    }
};

#endif
