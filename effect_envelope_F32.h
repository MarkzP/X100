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
    }

    AudioEffectEnvelope_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _sample_rate_Hz(settings.sample_rate_Hz),
      _preFilter1(settings.sample_rate_Hz),
      _preFilter2(settings.sample_rate_Hz),
      _surgeFilter(settings.sample_rate_Hz)
    {
    }

    void begin()
    {
      _preFilter1.setHighpass(100.0, 0.5);
      _preFilter2.setLowpass(1000.0, 0.5);
      _surgeFilter.setHighpass(25.0);
      enable();
      open();
      close();
      delay();
      attack();
      release();
      attenuation();
    }

    FLASHMEM void enable(bool enable = false)
    {
      _enable = enable;
    }
    
    FLASHMEM void open(float open = 0.5f)
    {
      open = open < 0.0f ? 0.0f : open > 1.0f ? 1.0f : open;
      _open = ln2unit(-8.0f + (open * 6.0f));
    }

    FLASHMEM void close(float close = 0.5f)
    {
      close = close < 0.0f ? 0.0f : close > 1.0f ? 1.0f : close;
      _close = ln2unit(-13.0f + (close * 5.0f));
    }

    FLASHMEM void delay(float delay = 0.5f)
    {
      delay = delay < 0.0f ? 0.0f : delay > 1.0f ? 1.0f : delay;
      _delay.time(_delay.maxTime() * delay);
    }

    FLASHMEM void attack(float attack = 0.5f)
    {
      _attack = timeToAlpha((attack * 1.445f) + 0.005f);
    }

    FLASHMEM void release(float release = 0.0f)
    {
      _release = timeToAlpha((release * 0.495f)+ 0.005f);
    }

    FLASHMEM void attenuation(float attenuation = 0.5f)
    {
      attenuation = attenuation < 0.0f? 0.0f : attenuation > 1.0f ? 1.0f : attenuation;
      _attmax = (attenuation * -14.0f) - 6.0f;
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

      bool opening = _opening;
      float gate = _gate;
      float fast = _fast;
      float surge = _surge;

      float open = _open;
      float close = _close;
      float attack = _attack;
      float release = _release;
      float attmin = _attmin;
      float attmax = _attmax;
      float sample, delayedDry, unit;

      float *p = block->data;
      float *end = p + block->length;
      do
      {
        delayedDry = _delay.delay(sample = *p);

        sample = _preFilter1.filter(sample);
        sample = _preFilter2.filter(sample);
        unit = fabsf(sample);

        fast += (unit - fast) * (unit > fast ? 0.75f : 0.01f);
        surge = _surgeFilter.filter(unit);//+= (fast - surge) * (fast > surge ? 0.01f : 0.001f);
        surge = surge < 0.0f ? 0.0f : surge > 1.0f ? 1.0f : surge;
        
        if (fast > open) opening = true;
        if (fast < close) opening = false;

        if (opening) gate += (attmin - gate) * attack;
        else gate += (attmax - gate) * release;

        *p++ = delayedDry * ln2unit(gate);
      }
      while (p < end);

      _opening = opening;
      _gate = gate;
      _fast = fast;
      _surge = surge;


      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

    float gate() { return ln2unit(_gate); }
    float surge() { return _surge; }
    float level() { return _fast; }

  private:
    audio_block_f32_t *inputQueueArray[1];
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;

    bool _enable = false;
    float _open;
    float _close;
    float _attack;
    float _release;
    
    RAM1Delay<1024> _delay;
    Biquad _preFilter1;
    Biquad _preFilter2;
    Biquad _surgeFilter;
    
    const float _attmin = 0.0f;
    float _attmax = -12.0f;

    bool _opening = false;
    float _gate = 0.0f;
    float _fast = 0.0f;
    float _surge = 0.0f;

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
