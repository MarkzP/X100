#ifndef _effect_envelope_F32_h_
#define _effect_envelope_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectEnvelope_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:envelope
  public:
    AudioEffectEnvelope_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
      begin();
    }

    AudioEffectEnvelope_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _sample_rate_Hz(settings.sample_rate_Hz),
      _preFilter(settings.sample_rate_Hz),
      _surgeFilter(settings.sample_rate_Hz)
    {
      begin();
    }

    void begin()
    {
      _preFilter.setHighpass(100.0, 0.5).setLowpass(1000.0, 0.5).begin();
      _surgeFilter.setHighpass(2000.0).begin();
      enable();
      open();
      close();
      attack();
      release();
      sensitivity();
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }
    
    void open(float open = 0.5f)
    {
      open = open < 0.0f ? 0.0f : open > 1.0f ? 1.0f : open;
      _open = ln2unit(-8.0f + (open * 6.0f));
    }

    void close(float close = 0.5f)
    {
      close = close < 0.0f ? 0.0f : close > 1.0f ? 1.0f : close;
      _close = ln2unit(-13.0f + (close * 5.0f));
    }

    void attack(float attack = 0.3f)
    {
      _attack = timeToAlpha(attack + 0.01f);
    }

    void release(float release = 0.0f)
    {
      _release = timeToAlpha(release + 0.01f);
    }

    void sensitivity(float sensitivity = 0.5f)
    {
      sensitivity = sensitivity < 0.0f ? 0.0f : sensitivity > 1.0f ? 1.0f : sensitivity;
      _sensitivity = sensitivity * 2000.0f;
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (_enable)
      {
        for (uint16_t i = 0; i < block->length; i++)
        {
          float sample = block->data[i];
          float unit = fabsf(_preFilter.filter(sample));

          _fast += (unit - _fast) * (unit > _fast ? 1.0f : 0.01f);
          _slow += (_fast - _slow) * (_fast > _slow ? 0.01f : 0.001f);
         
          if (_fast > _open) _opening = true;
          if (_fast < _close) _opening = false;

          if (_opening) _gate += (_attmin - _gate) * _attack;
          else _gate += (_attmax - _gate) * _release;

          //sample = _delay.delay(sample);
          block->data[i] = sample * ln2unit(_gate);
        }
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

    float gate() { return ln2unit(_gate); }
    float surge() { return _slow * 10.0f; }
    float level() { return _fast * 10.0f; }

  private:
    audio_block_f32_t *inputQueueArray[1];
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;

    bool _enable = false;
    float _sensitivity;
    float _open;
    float _close;
    float _attack;
    float _release;
    
    CascadeBiquad<2> _preFilter;
    HQBiquad _surgeFilter;
    //StaticDelay<256> _delay;
    
    const float _attmin = 0.0f;
    const float _attmax = -7.5f;

    bool _opening = false;
    float _gate = 0.0f;
    float _fast = 0.0f;
    float _slow = 0.0f;
    float _slowest = 0.0f;

    float timeToAlpha(float time)
    {
      if (time <= 0.0f) return 1.0f;
      return 1.0f - expf(-3.1699f / (_sample_rate_Hz * time));
    }

    inline float unit2ln(float u)
    {
      if (u < 1.17549e-38f) return -126.0f;
      union { float f; uint32_t i; } vx = { u };
      union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
      float y = vx.i;
      y *= 1.1920928955078125e-7f;
      return y - 124.22551499f - 1.498030302f * mx.f - 1.72587999f / (0.3520887068f + mx.f);
    }

    inline float ln2unit(float ln)
    {
      float offset = (ln < 0.0f) ? 1.0f : 0.0f;
      float clipp = (ln < -126.0f) ? -126.0f : ln;
      int w = clipp;
      float z = clipp - w + offset;
      union { uint32_t i; float f; } v = { uint32_t ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };
      return v.f;
    }
};

#endif
