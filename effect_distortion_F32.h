#ifndef _effect_distortion_f32_h_
#define _effect_distortion_f32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include "components_F32.h"
#include "arm_math.h"

#define INTERPOLATION   6

class AudioEffectDistortion_F32 : public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:effect_distortion
  public:
    AudioEffectDistortion_F32(void): AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioEffectDistortion_F32(const AudioSettings_F32 &settings) :
      AudioStream_F32(1, inputQueueArray),
      _preFilter1(settings.sample_rate_Hz),
      _preFilter2(settings.sample_rate_Hz),
      _innerFilter(settings.sample_rate_Hz),
      _postFilter1(settings.sample_rate_Hz),
      _postFilter2(settings.sample_rate_Hz),
      _postFilter3(settings.sample_rate_Hz),
      _postFilter4(settings.sample_rate_Hz)
    {
    }

    FLASHMEM void begin()
    {
      _preFilter1.setHighpassFirstOrder(100.0);
      _innerFilter.setLowpass(15000.0);
      _postFilter1.setHighpassFirstOrder(100.0);

      enable();
      gain();
      color();
      tone();
      level();
    }

    FLASHMEM void enable(bool enable = false)
    {
      _enable = enable;
      if (!_enable)
      {
        _preFilter1.reset();
        _preFilter2.reset();
        _innerFilter.reset();
        _postFilter1.reset();
        _postFilter2.reset();
        _postFilter3.reset();
        _postFilter4.reset();
      }
    }

    FLASHMEM void gain(float gain = 1.0f)
    {
      gain = gain < 0.0f ? 0.0f : gain > 1.0f ? 1.0f : gain;
      _gain = (powf(gain, 1.5f) * 495.0f) + 5.0f;
    }

    FLASHMEM void color(float color = 0.5f, float skew = 0.0f)
    {
      _pcurve = color < 0.0f ? 2.0f : color > 0.5f ? 0.0f : 2.0f - (color * 4.0f);
      _ncurve = color < 0.5f ? 2.0f : color > 1.0f ? 0.0f : 2.0f - ((color - 0.5f) * 4.0f);
      _pcomp = 1.0f / (_pcurve + 1.0f);
      _ncomp = 1.0f / (_ncurve + 1.0f);
      _skew = skew < 0.0f ? 1.0f : skew > 1.0f ? 0.25f : 1.0f - (skew * 0.75f);
    }

    void tone(float bottom = 1.0f, float mid = 0.0f, float tone = 1.0f)
    {
      bottom = bottom < 0.0f ? 0.0f : bottom > 1.0f ? 1.0f : bottom;
      mid = mid < 0.0f ? 0.0f : mid > 1.0f ? 1.0f : mid;
      tone = tone < 0.0f ? 0.0f : tone > 1.0f ? 1.0f : tone;

      float hpf1 = ((1.0f - bottom) * 250.0f) + 150.0f;
      _preFilter2.setHighpassFirstOrder(hpf1);

      float pg = (powf(mid, 0.75f) * 12.0f) - 9.0f;
      float lpf1 = (powf(tone, 0.45f) * 3000.0f) + 3000.0f;
      float lpf2 = (powf(tone, 0.45f) * 1500.0f) + 7000.0f;
      _postFilter2.setPeak(1350.0, pg);
      _postFilter3.setLowpassFirstOrder(lpf1);
      _postFilter4.setLowpass(lpf2);
    }

    FLASHMEM void level(float level = 1.0f)
    {
      level = level < 0.0f ? 0.0f : level > 1.0f ? 1.0f : level;
      _level = powf(level, 1.5f) * 0.5f;
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (!_enable)
      {
        AudioStream_F32::transmit(block);
        AudioStream_F32::release(block);
        return;
      }

      _preFilter1.filterBlock(block);
      _preFilter2.filterBlock(block);

      int len = block->length;
      float *p = block->data;

      //Interpolate
      len = _upscaler.interpolate(p, _interpolated, len);
      p = _interpolated;

      float gain = _gain;
      float ncomp = _ncomp;
      float ncurve = _ncurve;
      float ncurvep1 = _ncurve + 1.0f;
      float pcomp = _pcomp;
      float pcurve = _pcurve;
      float pcurvep1 = _pcurve + 1.0f;
      float skew = _skew;
      float skewm1 = _skew - 1.0f;
      float level = _level;
      float sample, msample;

      float *end = p + len;
      while (p < end)
      {
        sample = *p;          
        sample *= gain;

        if (sample < 0.0f)
        {
          sample *= ncomp;
          sample = ncurvep1 * sample / (1.0f - (ncurve * sample));
          sample = sample * _twoThirds;
          sample = sample < -1.0f ? -1.0f : sample;
          sample -= p3(sample) * _oneThird;
          sample *= _threeHalfs;
          sample = _innerFilter.filter(sample);
          msample = -sample;
          sample *= (msample + skew) / (p2(sample) + (skewm1 * msample) + 1.0f);
        }
        else
        {
          sample *= pcomp;
          sample = pcurvep1 * sample / (1.0f + (pcurve * sample));
          sample = sample * _twoThirds;
          sample = sample > 1.0f ? 1.0f : sample;
          sample -= p3(sample) * _oneThird;
          sample *= _threeHalfs;
          sample = _innerFilter.filter(sample);
          sample *= (sample + skew) / (p2(sample) + (skewm1 * sample) + 1.0f);
        }

        sample *= level;
        *p++ = sample;
      }

      // Decimate
      _upscaler.decimate(_interpolated, block->data, len);

      _postFilter1.filterBlock(block);
      _postFilter2.filterBlock(block);
      _postFilter3.filterBlock(block);
      _postFilter4.filterBlock(block);

      AudioStream_F32::transmit(block);
      AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    bool _enable = false;
    bool _bypass = true;
    bool _hard = false;
    float _gain = 0.1f;
    float _pcomp = 1.0f;
    float _ncomp = 1.0f;
    float _pcurve = 0.0f;
    float _ncurve = 0.0f;
    float _skew = 1.0f;
    float _level = 10.0f;
    float _dither = 0.5f;
    HQFirstOrder  _preFilter1;
    FirstOrder  _preFilter2;
    Biquad _innerFilter;
    FirstOrder  _postFilter1;
    Biquad _postFilter2;
    FirstOrder _postFilter3;
    Biquad _postFilter4;

    Multirate<INTERPOLATION> _upscaler;
    __attribute__((aligned(8))) float _interpolated[INTERPOLATION * 128];

    static constexpr float _threeHalfs = 3.0f / 2.0f;
    static constexpr float _oneThird = 1.0f / 3.0f;
    static constexpr float _twoThirds = 2.0f / 3.0f;

    inline float p2(float x) const
    {
      double d1 = x;
      return (float)(d1 * d1);
    }

    inline float p3(float x) const
    {
      double d1 = x;
      return (float)(d1 * d1 * d1);
    }
};

#else
#endif
