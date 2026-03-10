#ifndef _effect_tremolo_F32_h_
#define _effect_tremolo_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectTremolo_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:tremolo
  public:
    AudioEffectTremolo_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    }

    AudioEffectTremolo_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray), _lfo(settings.sample_rate_Hz)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
    }

    void depth(float depth = 0.0f)
    {
      if (depth <= 0.0f)
      {
        _enable = false;
        _depth = 0.0f;
        _offset = 1.0f;
        _smm = 1.0f;
      }
      else
      {
        _depth = (depth > 1.0f ? 1.0f : powf(depth, 4.0f)) + 0.2f;
        _offset = 1.0f - _depth;
        _enable = true;
      }
    }

    void rate(float rate = 0.2f)
    {
      rate = rate < 0.0f ? 0.0f : rate > 10.0f ? 10.0f : rate;
      _lfo.freq(rate);
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);

      if (!block) return;

      if (_enable)
      {
        float *p = block->data;
        float *end = p + block->length;
        do
        {
          float mod = (_lfo.next() * _depth) + _offset;
          mod = mod < 0.0f ? 0.0f : mod > 1.0f ? 1.0f : mod;
          _smm += (mod - _smm) * _smooth;
          *p++ *= _smm;
        }
        while (p < end);
      }

      _smm = FilterUtils::denormFloat(_smm);

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    TriangleLfo _lfo;
    float _sample_rate_Hz;
    float _smooth = 0.005f;
    float _smm = 0.0f;
    float _depth = 0.0f;
    float _offset = 0.0f;
    bool _enable = false;
};

#endif
