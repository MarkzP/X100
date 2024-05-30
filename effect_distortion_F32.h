#ifndef _effect_distortion_f32_h_
#define _effect_distortion_f32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include "components_F32.h"
#include "arm_math.h"

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
      _postFilter1(settings.sample_rate_Hz),
      _postFilter2(settings.sample_rate_Hz),
      _noiseFilter(settings.sample_rate_Hz)
    {
    }

    bool begin(bool multirate = true)
    {
      if (multirate)
      {
        if (!_multirate)
        {
          arm_status int_init = arm_fir_interpolate_init_f32(&_interpolator, _interpolation, _int_numtaps, _int_coeffs, _int_state, _raw_buffer_size);
          arm_status dec_init = arm_fir_decimate_init_f32(&_decimator, _int_numtaps, _interpolation, _int_coeffs, _decim_state, _int_buffer_size);
          _multirate = int_init == ARM_MATH_SUCCESS && dec_init == ARM_MATH_SUCCESS;
        }
      }
      else _multirate = false;

      enable();
      gain();
      bottom();
      color();
      tone();
      level();

      _noiseFilter.reset().setHighpass(13000.0f).begin();

      return _multirate;
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }

    void gain(float gain = 1.0f)
    {
      gain = gain < 0.0f ? 0.0f : gain > 1.0f ? 1.0f : gain;
      gain = (powf(gain, 1.5f) * 495.0f) + 5.0f;
      gain *= (_multirate ? (float)_interpolation : 1.0f);

      _gain = gain;
    }

    void bottom(float bottom = 1.0f)
    {
      bottom = bottom < 0.0f ? 0.0f : bottom > 1.0f ? 1.0f : bottom;
      bottom = 1.0f - bottom;

      float hpf1 = (bottom * 250.0f) + 125.0f;
      float lpf1 = 14000.0f;

      _preFilter1.reset().setHighpass(hpf1).begin();
      _preFilter2.reset().setLowpass(lpf1).begin();
    }

    void color(float color = 0.5f, float skew = 0.0f)
    {
      _pcurve = color < 0.0f ? 2.0f : color > 0.5f ? 0.0f : 2.0f - (color * 4.0f);
      _ncurve = color < 0.5f ? 2.0f : color > 1.0f ? 0.0f : 2.0f - ((color - 0.5f) * 4.0f);
      _pcomp = 1.0f / (_pcurve + 1.0f);
      _ncomp = 1.0f / (_ncurve + 1.0f);
      _skew = skew < 0.0f ? 1.0f : skew > 1.0f ? 0.25f : 1.0f - (skew * 0.75f);
    }

    void tone(float tone = 1.0f, float mid = 0.0f)
    {
      tone = tone < 0.0f ? 0.0f : tone > 1.0f ? 1.0f : tone;
      mid = mid < 0.0f ? 0.0f : mid > 1.0f ? 1.0f : mid;

      float hpf1 = 150.0f;
      float hpf2 = (powf(tone, 1.50f) * 250.0f) + 150.0f;
      float pf = 1350.0f;
      float pg = (powf(mid, 0.75f) * 12.0f) - 9.0f;
      float lpf1 = (powf(tone, 0.45f) * 3000.0f) + 3000.0f;
      float lpf2 = (powf(tone, 0.45f) * 1500.0f) + 7000.0f;

      _postFilter1.reset().setHighpass1p1z(hpf1).begin();
      _postFilter2.reset().setHighpass1p1z(hpf2).setPeak(pf, pg).setLowpass1p1z(lpf1).setLowpass(lpf2).begin();
    }

    void level(float level = 1.0f)
    {
      _level = (powf(level, 1.5f) * 0.3f) + 0.03f;
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
      BlockOperations::scale(block, _gain);

      int len = block->length;
      float *p = block->data;
      if (_multirate)
      {
        // Interpolate
        arm_fir_interpolate_f32(&_interpolator, block->data, _interpolated, block->length);
        len *= _interpolation;
        p = _interpolated;

        for (int i = 0; i < len; i++)
        {
          _noise[i] = (float)rand() * (_dither / (float)RAND_MAX);
        }
        _noiseFilter.filterArray(_noise, _noise, len);
      }

      float pcomp = _pcomp;
      float ncomp = _ncomp;
      float pcurve = _pcurve;
      float pcurvep1 = pcurve + 1.0f;
      float ncurve = _ncurve;
      float ncurvep1 = ncurve + 1.0f;
      float skew = _skew;
      float skewm1 = _skew - 1.0f;

      for (int i = 0; i < len; i++)
      {
        float sample = p[i];

        if (sample > 0.0f)
        {
          sample *= pcomp;
          sample = pcurvep1 * sample / (1.0f + (pcurve * sample));
        }
        else
        {
          sample *= ncomp;
          sample = ncurvep1 * sample / (1.0f - (ncurve * sample));
        }

        sample *= _twoThirds;
        sample = clip(sample);
        sample = (sample - (cube(sample) * _oneThird)) * _threeHalfs;

        sample *= (fabsf(sample) + skew) / (square(sample) + skewm1 * fabsf(sample) + 1.0f);

        sample += _noise[i];

        p[i] = sample;
      }

      // Decimate
      if (_multirate) arm_fir_decimate_f32(&_decimator, _interpolated, block->data, len);

      BlockOperations::scale(block, _level);

      _postFilter1.filterBlock(block);
      _postFilter2.filterBlock(block);

      AudioStream_F32::transmit(block);
      AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    bool _enable = false;
    float _gain = 0.1f;
    float _pcomp = 1.0f;
    float _ncomp = 1.0f;
    float _pcurve = 0.0f;
    float _ncurve = 0.0f;
    float _skew = 1.0f;
    float _level = 10.0f;
    float _dither = 0.33f;
    LFBiquad  _preFilter1;
    CascadeBiquad<1> _preFilter2;
    LF1p1zBiquad  _postFilter1;
    CascadeBiquad<4> _postFilter2;
    CascadeBiquad<1> _noiseFilter;

    static constexpr int _interpolation = 5;
    static constexpr int _raw_buffer_size = 128;
    static constexpr int _int_buffer_size = _raw_buffer_size * _interpolation;
    static constexpr int _int_numtaps = 125;
    static constexpr float _oneThird = 1.0f / 3.0f;
    static constexpr float _threeHalfs = 3.0f / 2.0f;
    static constexpr float _twoThirds = 2.0f / 3.0f;

    arm_fir_interpolate_instance_f32 _interpolator;
    arm_fir_decimate_instance_f32 _decimator;
    float _interpolated[_int_buffer_size];
    float _noise[_int_buffer_size];
    float _int_state[(_int_numtaps / _interpolation) + _raw_buffer_size - 1];
    float _decim_state[_int_numtaps + _int_buffer_size - 1];
    bool _multirate = false;

    inline float clip(float x)
    {
      return x < -1.0f ? -1.0f : x > 1.0f ? 1.0f : x;
    }

    inline float square(float x)
    {
      //double d = x; return (float)(d * d);
      return x * x;
    }

    inline float cube(float x)
    {
      double d = x; return (float)(d * d * d);
      //return x * x * x;
    }

    float _int_coeffs[_int_numtaps] = {

      -0.000029,
      -0.000017,
      0.000014,
      0.000063,
      0.000119,
      0.000165,
      0.000179,
      0.000142,
      0.000044,
      -0.000109,
      -0.000291,
      -0.000456,
      -0.000551,
      -0.000523,
      -0.000342,
      -0.000012,
      0.000420,
      0.000864,
      0.001202,
      0.001317,
      0.001119,
      0.000584,
      -0.000232,
      -0.001180,
      -0.002049,
      -0.002596,
      -0.002612,
      -0.001981,
      -0.000725,
      0.000966,
      0.002759,
      0.004227,
      0.004943,
      0.004588,
      0.003049,
      0.000482,
      -0.002674,
      -0.005756,
      -0.008005,
      -0.008725,
      -0.007470,
      -0.004187,
      0.000696,
      0.006301,
      0.011416,
      0.014732,
      0.015121,
      0.011931,
      0.005210,
      -0.004173,
      -0.014575,
      -0.023783,
      -0.029367,
      -0.029113,
      -0.021472,
      -0.005922,
      0.016820,
      0.044792,
      0.075013,
      0.103894,
      0.127781,
      0.143534,
      0.149034,
      0.143534,
      0.127781,
      0.103894,
      0.075013,
      0.044792,
      0.016820,
      -0.005922,
      -0.021472,
      -0.029113,
      -0.029367,
      -0.023783,
      -0.014575,
      -0.004173,
      0.005210,
      0.011931,
      0.015121,
      0.014732,
      0.011416,
      0.006301,
      0.000696,
      -0.004187,
      -0.007470,
      -0.008725,
      -0.008005,
      -0.005756,
      -0.002674,
      0.000482,
      0.003049,
      0.004588,
      0.004943,
      0.004227,
      0.002759,
      0.000966,
      -0.000725,
      -0.001981,
      -0.002612,
      -0.002596,
      -0.002049,
      -0.001180,
      -0.000232,
      0.000584,
      0.001119,
      0.001317,
      0.001202,
      0.000864,
      0.000420,
      -0.000012,
      -0.000342,
      -0.000523,
      -0.000551,
      -0.000456,
      -0.000291,
      -0.000109,
      0.000044,
      0.000142,
      0.000179,
      0.000165,
      0.000119,
      0.000063,
      0.000014,
      -0.000017,
      -0.000029

    };
};

#endif
