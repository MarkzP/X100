#ifndef _effect_distortion_f32_h_
#define _effect_distortion_f32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include "components_F32.h"
#include "arm_math.h"

#define INTERPOLATION  3
#define FIFTH

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
      _postFilter2(settings.sample_rate_Hz)
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
      color();
      tone();
      level();

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
      //gain *= (_multirate ? (float)_interpolation : 1.0f);
      _gain = gain;
    }

    void color(float color = 0.5f, float skew = 0.0f)
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

      if (bottom == 1.0f)
      {
        _preFilter1.reset().begin();
        _preFilter2.reset().begin();
      }
      else
      {
        float hpf1 = ((1.0f - bottom) * 250.0f) + 125.0f;
        _preFilter1.reset().setHighpass1p1z(hpf1).begin();
        _preFilter2.reset().setLowpass1p1z(14000.0f).begin();
      }

      if (mid == 1.0f && tone == 1.0f)
      {
        _postFilter1.reset().begin();
        _postFilter2.reset().begin();
      }
      else
      {
        float hpf1 = 150.0f;
        float hpf2 = (powf(tone, 1.50f) * 250.0f) + 150.0f;
        float pf = 1350.0f;
        float pg = (powf(mid, 0.75f) * 12.0f) - 9.0f;
        float lpf1 = (powf(tone, 0.45f) * 3000.0f) + 3000.0f;
        float lpf2 = (powf(tone, 0.45f) * 1500.0f) + 7000.0f;
        _postFilter1.reset().setHighpass1p1z(hpf1).begin();
        _postFilter2.reset().setHighpass1p1z(hpf2).setPeak(pf, pg).setLowpass1p1z(lpf1).setLowpass(lpf2).begin();
      }
    }

    void level(float level = 1.0f)
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
      if (_multirate)
      {
        // Interpolate
        arm_scale_f32(p, _interpolation, p, len);
        arm_fir_interpolate_f32(&_interpolator, p, _interpolated, len);
        len *= _interpolation;
        p = _interpolated;
      }

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

      float *end = p + len;
      do
      {
        float sample = *p;
        
        sample *= gain;

        if (sample < 0.0f)
        {
          sample *= ncomp;
          sample = ncurvep1 * sample / (1.0f - (ncurve * sample));
#ifdef FIFTH
          sample = sample < -1.0f ? -1.0f : sample;
          sample *= _threeFourths;
          sample -= (p5(sample) * _oneFifth);
          sample *= _fiveFourth;
#else
          sample = sample < -1.0f ? -1.0f : sample;
          sample *= _twoThirds;
          sample -= (p3(sample) * _oneThird);
          sample *= _threeHalfs;
#endif
          sample *= (-sample + skew) / (p2(sample) + (skewm1 * -sample) + 1.0f);
        }
        else
        {
          sample *= pcomp;
          sample = pcurvep1 * sample / (1.0f + (pcurve * sample));
#ifdef FIFTH
          sample = sample > 1.0f ? 1.0f : sample;
          sample *= _threeFourths;
          sample -= (p5(sample) * _oneFifth);
          sample *= _fiveFourth;
#else
          sample = sample > 1.0f ? 1.0f : sample;
          sample *= _twoThirds;
          sample -= (p3(sample) * _oneThird);
          sample *= _threeHalfs;
#endif
          sample *= (sample + skew) / (p2(sample) + (skewm1 * sample) + 1.0f);
        }

        sample *= level;

        *p++ = sample;
      }
      while (p < end);

      // Decimate
      if (_multirate) arm_fir_decimate_f32(&_decimator, _interpolated, block->data, len);

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
    float _dither = 0.5f;
    HQ1p1zBiquad  _preFilter1;
    CascadeBiquad<1> _preFilter2;
    HQ1p1zBiquad  _postFilter1;
    CascadeBiquad<4> _postFilter2;
    CascadeBiquad<1> _noiseFilter;

    static constexpr int _interpolation = INTERPOLATION;
#if INTERPOLATION==5
    static constexpr int _int_numtaps = 125;
#elif INTERPOLATION==7
    static constexpr int _int_numtaps = 161;
#elif INTERPOLATION==3
    static constexpr int _int_numtaps = 69;
#else
    warn Invalid interpolation
#endif
    static constexpr int _raw_buffer_size = 128;
    static constexpr int _int_buffer_size = _raw_buffer_size * _interpolation;

    static constexpr float _threeHalfs = 3.0f / 2.0f;
    static constexpr float _oneThird = 1.0f / 3.0f;
    static constexpr float _twoThirds = 2.0f / 3.0f;
    static constexpr float _fourThirds = 4.0f / 3.0f;
    static constexpr float _fiveFourth = 5.0f / 4.0f;
    static constexpr float _threeFourths = 3.0f / 4.0f;
    static constexpr float _oneFifth = 1.0f / 5.0f;


    arm_fir_interpolate_instance_f32 _interpolator;
    arm_fir_decimate_instance_f32 _decimator;
    float _interpolated[_int_buffer_size];
    float _int_state[(_int_numtaps / _interpolation) + _raw_buffer_size - 1];
    float _decim_state[_int_numtaps + _int_buffer_size - 1];
    bool _multirate = false;

    inline float clip(float x)
    {
      return x < -1.0f ? -1.0f : x > 1.0f ? 1.0f : x;
    }

    inline float p2(float x)
    {
      if (fabsf(x) < 1.0e-9f) return 0.0f;
      double d1 = x;
      return (float)(d1 * d1);
    }

    inline float p3(float x)
    {
      if (fabsf(x) < 1.0e-6f) return 0.0f;
      double d1 = x;
      return (float)(d1 * d1 * d1);
    }

    inline float p5(float x)
    {
      if (fabsf(x) < 2.5e-4f) return 0.0f;
      double d1 = (double)x;
      double d2 = d1 * d1;
      double d3 = d2 * d1;
      return (float)(d3 * d2);
    }

#if INTERPOLATION==5
    //https://www.arc.id.au/FilterDesign.html
    //Fs=220500, Fb=16431, M=125, Att=69 => dF/2=3777, fc=20208

    //Fs=220500, Fb=18300, M=135, Att=76 => dF/2=3896

    //Fs=220500, Fb=18250, M=125, Att=69 => dF/2=3777
    float _int_coeffs[125] = {
      0.000031, 
      0.000018, 
      -0.000017, 
      -0.000070, 
      -0.000126, 
      -0.000162, 
      -0.000152, 
      -0.000079, 
      0.000056, 
      0.000228, 
      0.000388, 
      0.000472, 
      0.000426, 
      0.000220, 
      -0.000126, 
      -0.000537, 
      -0.000896, 
      -0.001070, 
      -0.000950, 
      -0.000497, 
      0.000229, 
      0.001066, 
      0.001775, 
      0.002102, 
      0.001859, 
      0.000992, 
      -0.000366, 
      -0.001904, 
      -0.003187, 
      -0.003772, 
      -0.003341, 
      -0.001825, 
      0.000529, 
      0.003175, 
      0.005375, 
      0.006388, 
      0.005696, 
      0.003194, 
      -0.000704, 
      -0.005097, 
      -0.008778, 
      -0.010536, 
      -0.009510, 
      -0.005498, 
      0.000871, 
      0.008170, 
      0.014445, 
      0.017671, 
      0.016309, 
      0.009818, 
      -0.001010, 
      -0.014011, 
      -0.025951, 
      -0.033131, 
      -0.032188, 
      -0.020913, 
      0.001102, 
      0.032160, 
      0.068596, 
      0.105339, 
      0.136825, 
      0.158046, 
      0.165533, 
      0.158046, 
      0.136825, 
      0.105339, 
      0.068596, 
      0.032160, 
      0.001102, 
      -0.020913, 
      -0.032188, 
      -0.033131, 
      -0.025951, 
      -0.014011, 
      -0.001010, 
      0.009818, 
      0.016309, 
      0.017671, 
      0.014445, 
      0.008170, 
      0.000871, 
      -0.005498, 
      -0.009510, 
      -0.010536, 
      -0.008778, 
      -0.005097, 
      -0.000704, 
      0.003194, 
      0.005696, 
      0.006388, 
      0.005375, 
      0.003175, 
      0.000529, 
      -0.001825, 
      -0.003341, 
      -0.003772, 
      -0.003187, 
      -0.001904, 
      -0.000366, 
      0.000992, 
      0.001859, 
      0.002102, 
      0.001775, 
      0.001066, 
      0.000229, 
      -0.000497, 
      -0.000950, 
      -0.001070, 
      -0.000896, 
      -0.000537, 
      -0.000126, 
      0.000220, 
      0.000426, 
      0.000472, 
      0.000388, 
      0.000228, 
      0.000056, 
      -0.000079, 
      -0.000152, 
      -0.000162, 
      -0.000126, 
      -0.000070, 
      -0.000017, 
      0.000018, 
      0.000031
    };
#elif INTERPOLATION==7
    //https://www.arc.id.au/FilterDesign.html
    //Fs=308700, Fb=21000, M=161, Att=75 => dF/2=4501
    float _int_coeffs[161] = {
      0.000006, 
      0.000017, 
      0.000030, 
      0.000041, 
      0.000046, 
      0.000039, 
      0.000017, 
      -0.000020, 
      -0.000067, 
      -0.000116, 
      -0.000154, 
      -0.000169, 
      -0.000147, 
      -0.000085, 
      0.000017, 
      0.000146, 
      0.000279, 
      0.000385, 
      0.000435, 
      0.000402, 
      0.000273, 
      0.000053, 
      -0.000230, 
      -0.000529, 
      -0.000782, 
      -0.000925, 
      -0.000903, 
      -0.000687, 
      -0.000285, 
      0.000256, 
      0.000848, 
      0.001377, 
      0.001721, 
      0.001775, 
      0.001477, 
      0.000827, 
      -0.000102, 
      -0.001166, 
      -0.002172, 
      -0.002904, 
      -0.003170, 
      -0.002844, 
      -0.001897, 
      -0.000427, 
      0.001353, 
      0.003131, 
      0.004552, 
      0.005281, 
      0.005070, 
      0.003825, 
      0.001643, 
      -0.001179, 
      -0.004174, 
      -0.006777, 
      -0.008419, 
      -0.008638, 
      -0.007187, 
      -0.004107, 
      0.000238, 
      0.005186, 
      0.009861, 
      0.013310, 
      0.014672, 
      0.013347, 
      0.009151, 
      0.002406, 
      -0.006036, 
      -0.014865, 
      -0.022464, 
      -0.027130, 
      -0.027331, 
      -0.021960, 
      -0.010548, 
      0.006603, 
      0.028394, 
      0.053014, 
      0.078145, 
      0.101233, 
      0.119824, 
      0.131879, 
      0.136054, 
      0.131879, 
      0.119824, 
      0.101233, 
      0.078145, 
      0.053014, 
      0.028394, 
      0.006603, 
      -0.010548, 
      -0.021960, 
      -0.027331, 
      -0.027130, 
      -0.022464, 
      -0.014865, 
      -0.006036, 
      0.002406, 
      0.009151, 
      0.013347, 
      0.014672, 
      0.013310, 
      0.009861, 
      0.005186, 
      0.000238, 
      -0.004107, 
      -0.007187, 
      -0.008638, 
      -0.008419, 
      -0.006777, 
      -0.004174, 
      -0.001179, 
      0.001643, 
      0.003825, 
      0.005070, 
      0.005281, 
      0.004552, 
      0.003131, 
      0.001353, 
      -0.000427, 
      -0.001897, 
      -0.002844, 
      -0.003170, 
      -0.002904, 
      -0.002172, 
      -0.001166, 
      -0.000102, 
      0.000827, 
      0.001477, 
      0.001775, 
      0.001721, 
      0.001377, 
      0.000848, 
      0.000256, 
      -0.000285, 
      -0.000687, 
      -0.000903, 
      -0.000925, 
      -0.000782, 
      -0.000529, 
      -0.000230, 
      0.000053, 
      0.000273, 
      0.000402, 
      0.000435, 
      0.000385, 
      0.000279, 
      0.000146, 
      0.000017, 
      -0.000085, 
      -0.000147, 
      -0.000169, 
      -0.000154, 
      -0.000116, 
      -0.000067, 
      -0.000020, 
      0.000017, 
      0.000039, 
      0.000046, 
      0.000041, 
      0.000030, 
      0.000017, 
      0.000006
    };
#elif INTERPOLATION==3

    //Fs=132300, Fb=18000, M=39, Att=46 => dF/2=4607


    //Fs=132300, Fb=18800, M=99, Att=76 => dF/2=3196


    //Fs=132300, Fb=18500, M=69, Att=60 => dF/2=3523

    float _int_coeffs[69] = {
      -0.000191, 
      -0.000197, 
      0.000069, 
      0.000519, 
      0.000758, 
      0.000357, 
      -0.000679, 
      -0.001659, 
      -0.001564, 
      0.000066, 
      0.002412, 
      0.003592, 
      0.002015, 
      -0.002001, 
      -0.005785, 
      -0.005877, 
      -0.000873, 
      0.006609, 
      0.010898, 
      0.007236, 
      -0.003802, 
      -0.015129, 
      -0.017176, 
      -0.005222, 
      0.015137, 
      0.029427, 
      0.023381, 
      -0.005403, 
      -0.041498, 
      -0.057169, 
      -0.027972, 
      0.050390, 
      0.155016, 
      0.244507, 
      0.279667, 
      0.244507, 
      0.155016, 
      0.050390, 
      -0.027972, 
      -0.057169, 
      -0.041498, 
      -0.005403, 
      0.023381, 
      0.029427, 
      0.015137, 
      -0.005222, 
      -0.017176, 
      -0.015129, 
      -0.003802, 
      0.007236, 
      0.010898, 
      0.006609, 
      -0.000873, 
      -0.005877, 
      -0.005785, 
      -0.002001, 
      0.002015, 
      0.003592, 
      0.002412, 
      0.000066, 
      -0.001564, 
      -0.001659, 
      -0.000679, 
      0.000357, 
      0.000758, 
      0.000519, 
      0.000069, 
      -0.000197, 
      -0.000191
    };
    #endif
};

#else
#endif
