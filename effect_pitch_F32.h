#include "wiring.h"
#ifndef _effect_pitch_F32_h_
#define _effect_pitch_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectPitchShifter_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:envelope
  public:
    AudioEffectPitchShifter_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioEffectPitchShifter_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _sample_rate_Hz(settings.sample_rate_Hz)
    {
    }

    FLASHMEM void begin()
    {
    }

    FLASHMEM void wet(float wet)
    {
      _wet = wet < 0.0f ? 0.0f : wet > 1.0f ? 1.0f : wet;
    }

    FLASHMEM void dry(float dry)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    FLASHMEM void transpose(float semi)
    {
      semi = semi < -12.0f ? -12.0f : semi > 12.0f ? 12.0f : semi;
      _stride = exp2f(semi * (1.0f / 12.0f));
    }

    FLASHMEM void enable(bool enable = false)
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

      // Low:     80Hz    12.5msec    552 samples
      // High:  1350Hz    0.74msec     33 samples

      float *p = block->data;
      float *end = p + block->length;
      do
      {
        float sample = *p;

        if (++_w >= _bufferSize) _w = 0;
        _delay[_w] = sample;

        _r += _stride);
        if (_r >= _max_f) _r -= _max_f;

        float s0 = read(_r);
        float s1 = read(_r + _half_f);

        float s = s0 + s1;

        *p++ = (sample * _dry) + (s * _wet);
      }
      while (p < end);

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;

    static constexpr uint16_t _bufferSize = 300;
    static constexpr int _max_i = (int)_bufferSize;
    static constexpr float _max_f = (float)_bufferSize;
    static constexpr float _half_f = _max_f / 2.0f;
    static constexpr float _d2v = 2.0f / _max_f;
    float _delay[_bufferSize];

    int _w = 0;
    float _r = 0.0f;
    float _stride = 1.0;

    float _dry = 0.0f;
    float _wet = 1.0f;

    bool _enable = false;

    float read(float pos)
    {
      while (pos < 0.0f) pos += _max_f;
      while (pos >= _max_f) pos -= _max_f;

      float delta = fabsf((float)_w - pos);
      float distance = min(_max_f - delta, delta);

      float i;
      float d = modff(pos, &i);

      uint16_t i0 = (int)i;
      if (i0 >= _bufferSize) i0 = 0;

      uint16_t i1 = i0 + 1;
      if (i1 >= _bufferSize) i1 = 0;

      float s0 = _delay[i0];
      float s1 = _delay[i1];

      return (d * (s1 - s0)) + s0;
    }
};

#endif
