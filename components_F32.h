#ifndef _components_F32_h_
#define _components_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include "sinTable512_f32.h"
#include "arm_math.h"
#include "smalloc.h"

class BlockOperations
{
  public:
    static void copy(audio_block_f32_t *pSrc, audio_block_f32_t *pDst)
    {
      if (pSrc == pDst) return;
      arm_copy_f32(pSrc->data, pDst->data, pSrc->length);
    }

    static void add(audio_block_f32_t *pSrcA, audio_block_f32_t *pSrcB, audio_block_f32_t *pDst = nullptr)
    {
      if (pDst == nullptr) pDst = pSrcB;
      arm_add_f32(pSrcA->data, pSrcB->data, pDst->data, pDst->length);
    }

    static void scale(audio_block_f32_t *pSrc, float scale, audio_block_f32_t *pDst = nullptr)
    {
      if (pDst == nullptr) pDst = pSrc;
      arm_scale_f32(pSrc->data, scale, pDst->data, pSrc->length);
    }
};


class FilterUtils
{
  public:
    static inline float denormFloat(float f)
    {
      return fabsf(f) < _minf ? 0.0f : f;
    }

    static inline double denormDouble(double f)
    {
      return abs(f) < _mind ? 0.0 : f;
    }

    static inline float u2dB(float u)
    {
      return u < 5.011872E-07f ? -126.0f : 20.0f * log10f(u);
    }

  private:
    static constexpr double _mind = 1.0e-18;
    static constexpr float _minf = 1.0e-18f;
};

class BiquadBase
{
  protected:
    static constexpr double _invsqrt2 = 0.7071067811865475;
    double _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;

    virtual bool coefficients(double b0, double b1, double b2, double a1, double a2)
    {
      return false;
    }

    bool lowpass1p1z(double frequency)
    {
      if (frequency <= 0.0f) return false;

      double k = calc_k(frequency);
      double norm = 1.0 / (1.0 / k + 1.0);
      double b0 = norm;
      double b1 = norm;
      double b2 = 0.0;
      double a1 = -(1.0 - 1.0 / k) * norm;
      double a2 = 0.0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool highpass1p1z(double frequency)
    {
      if (frequency <= 0.0f) return false;

      double k = calc_k(frequency);
      double norm = 1.0 / (k + 1.0);
      double b0 = norm;
      double b1 = -norm;
      double b2 = 0.0;
      double a1 = -(k - 1.0) * norm;
      double a2 = 0.0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool lowpass(double frequency, double q = _invsqrt2)
    {
      if (frequency <= 0.0f || q <= 0.0f) return false;

      double w0 = calc_w0(frequency);
      double sinW0 = sin(w0);
      double alpha = sinW0 / (q * 2.0);
      double cosW0 = cos(w0);
      double inv_a0 = 1.0 / (1.0 + alpha);
      double b0 = ((1.0 - cosW0) / 2.0) * inv_a0;
      double b1 = (1.0 - cosW0) * inv_a0;
      double b2 = b0;
      double a1 = -(-2.0 * cosW0) * inv_a0;
      double a2 = -(1.0 - alpha) * inv_a0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool highpass(double frequency, double q = _invsqrt2)
    {
      if (frequency <= 0.0f || q <= 0.0f) return false;

      double w0 = calc_w0(frequency);
      double sinW0 = sin(w0);
      double alpha = sinW0 / (q * 2.0);
      double cosW0 = cos(w0);
      double inv_a0 = 1.0 / (1.0 + alpha);
      double b0 = ((1.0 + cosW0) / 2.0) * inv_a0;
      double b1 = -(1.0 + cosW0) * inv_a0;
      double b2 = b0;
      double a1 = -(-2.0 * cosW0) * inv_a0;
      double a2 = -(1.0 - alpha) * inv_a0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool bandpass(double frequency, double q = _invsqrt2)
    {
      if (frequency <= 0.0f || q <= 0.0f) return false;

      double w0 = calc_w0(frequency);
      double sinW0 = sin(w0);
      double alpha = sinW0 / (q * 2.0);
      double cosW0 = cos(w0);
      double inv_a0 = 1.0 / (1.0 + alpha);
      double b0 = alpha * inv_a0;
      double b1 = 0.0;
      double b2 = -b0;
      double a1 = -(-2.0 * cosW0) * inv_a0;
      double a2 = -(1.0 - alpha) * inv_a0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool notch(double frequency, double q = _invsqrt2)
    {
      if (frequency <= 0.0f || q <= 0.0f) return false;

      double w0 = calc_w0(frequency);
      double sinW0 = sin(w0);
      double alpha = sinW0 / (q * 2.0);
      double cosW0 = cos(w0);
      double inv_a0 = 1.0 / (1.0 + alpha);
      double b0 = inv_a0;
      double b1 = (-2.0 * cosW0) * inv_a0;
      double b2 = inv_a0;
      double a1 = -(-2.0 * cosW0) * inv_a0;
      double a2 = -(1.0 - alpha) * inv_a0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool lowShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      if (frequency <= 0.0f) return false;

      double b0, b1, b2, a1, a2;

      if (slope <= 0.0)
      {
        double v = calc_v(gain);
        double k = calc_k(frequency);
        if (gain >= 0.0)
        {
          double norm = 1.0 / (k + 1.0);
          b0 = (k * v + 1.0) * norm;
          b1 = (k * v - 1.0) * norm;
          b2 = 0.0;
          a1 = -(k - 1.0) * norm;
          a2 = 0.0;
        }
        else
        {	
          double norm = 1.0 / (k * v + 1.0);
          b0 = (k + 1.0) * norm;
          b1 = (k - 1.0) * norm;
          b2 = 0.0;
          a1 = -(k * v - 1.0) * norm;
          a2 = 0.0;
        }
      }
      else
      {
        double a = calc_a(gain);
        double w0 = calc_w0(frequency);
        double sinW0 = sin(w0);
        double cosW0 = cos(w0);
        double sinsq = sinW0 * sqrt((pow(a, 2.0) + 1.0) * (1.0 / slope - 1.0) + 2.0 * a);
        double aMinus = (a - 1.0) * cosW0;
        double aPlus = (a + 1.0) * cosW0;
        double inv_a0 = 1.0 / ((a + 1.0) + aMinus + sinsq);

        b0 = a * ((a + 1.0) - aMinus + sinsq) * inv_a0;
        b1 = 2.0 * a * ((a - 1.0) - aPlus) * inv_a0;
        b2 = a * ((a + 1.0) - aMinus - sinsq) * inv_a0;
        a1 = 2.0 * ((a - 1.0) + aPlus) * inv_a0;
        a2 = -((a + 1.0) + aMinus - sinsq) * inv_a0;
      }

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool highShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      if (frequency <= 0.0f) return false;

      double b0, b1, b2, a1, a2;

      if (slope <= 0.0)
      {
        double v = calc_v(gain);
        double k = calc_k(frequency);
        if (gain >= 0.0)
        {
          double norm = 1.0 / (k + 1.0);
          b0 = (k + v) * norm;
          b1 = (k - v) * norm;
          b2 = 0.0;
          a1 = -(k - 1.0) * norm;
          a2 = 0.0;
        }
        else
        {	
          double norm = 1.0 / (k + v);
          b0 = (k + 1.0) * norm;
          b1 = (k - 1.0) * norm;
          b2 = 0.0;
          a1 = -(k - v) * norm;
          a2 = 0.0;
        }
      }
      else
      {
        double w0 = calc_w0(frequency);
        double a = calc_a(gain);
        double sinW0 = sin(w0);
        double cosW0 = cos(w0);
        double sinsq = sinW0 * sqrt((pow(a, 2.0) + 1.0) * (1.0 / slope - 1.0) + 2.0 * a);
        double aMinus = (a - 1.0) * cosW0;
        double aPlus = (a + 1.0) * cosW0;
        double inv_a0 = 1.0 / ((a + 1.0) - aMinus + sinsq);

        b0 = a * ((a + 1.0) + aMinus + sinsq) * inv_a0;
        b1 = -2.0 * a * ((a - 1.0) + aPlus) * inv_a0;
        b2 = a * ((a + 1.0) + aMinus - sinsq) * inv_a0;
        a1 = -2.0 * ((a - 1.0) - aPlus) * inv_a0;
        a2 = -((a + 1.0) - aMinus - sinsq) * inv_a0;
      }

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool peak(double frequency, double gain, double q = _invsqrt2)
    {
      if (frequency <= 0.0f || q <= 0.0f) return false;

      double a = calc_a(gain);
      double w0 = calc_w0(frequency);
      double sinW0 = sin(w0);
      double alpha = sinW0 / (q * 2.0);
      double cosW0 = cos(w0);
      double inv_a0 = 1.0 / (1.0 + (alpha / a));
      double b0 = (1.0 + (alpha * a)) * inv_a0;
      double b1 = -2.0 * cosW0 * inv_a0;
      double b2 = (1.0 - (alpha * a)) * inv_a0;
      double a1 = -b1;
      double a2 = -(1 - (alpha / a)) * inv_a0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    double calc_w0(double freq)
    {
      return freq * 2.0 * PI / _sample_rate_Hz;
    }

    double calc_k(double freq)
    {
      return tan(freq * PI / _sample_rate_Hz);
    }

    double calc_a(double gain)
    {
      return pow(10.0, gain / 40.0);
    }

    double calc_v(double gain)
    {
      return pow(10.0, abs(gain) / 20.0);
    }
};


template <int N = 4>
class CascadeBiquad: public BiquadBase
{
  public:
    CascadeBiquad(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      _sample_rate_Hz = sample_rate_Hz;
      reset();
    }

    CascadeBiquad& reset()
    {
      if (!_active) for (int i = 0; i < N * _states_per_stages; i++) _states[i] = 0.0f;
      _stages = 0;
      return *this;
    }

    void begin()
    {
      _active = _stages > 0;
      if (_active) arm_biquad_cascade_df1_init_f32(&_iir, _stages, _coeffs, _states);
    }


    CascadeBiquad& setLowpass1p1z(double frequency)
    {
      lowpass1p1z(frequency);
      return *this;
    }

    CascadeBiquad& setHighpass1p1z(double frequency)
    {
      highpass1p1z(frequency);
      return *this;
    }

    CascadeBiquad& setLowpass(double frequency, double q = _invsqrt2)
    {
      lowpass(frequency, q);
      return *this;
    }

    CascadeBiquad& setHighpass(double frequency, double q = _invsqrt2)
    {
      highpass(frequency, q);
      return *this;
    }

    CascadeBiquad& setBandpass(double frequency, double q = _invsqrt2)
    {
      bandpass(frequency, q);
      return *this;
    }

    CascadeBiquad& setNotch(double frequency, double q = _invsqrt2)
    {
      notch(frequency, q);
      return *this;
    }

    CascadeBiquad& setLowShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      lowShelf(frequency, gain, slope);
      return *this;
    }

    CascadeBiquad& setHighShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      highShelf(frequency, gain, slope);
      return *this;
    }

    CascadeBiquad& setPeak(double frequency, double gain, double q = _invsqrt2)
    {
      peak(frequency, gain, q);
      return *this;
    }

    float filter(float sample)
    {
      if (!_active) return sample;

      arm_biquad_cascade_df1_f32(&_iir, &sample, &sample, 1);
      denormStates();

      return sample;
    }

    inline void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      if (!_active)
      {
        if (pSrc != pDst) arm_copy_f32(pSrc, pDst, len);
        return;
      }

      arm_biquad_cascade_df1_f32(&_iir, pSrc, pDst, len);
      denormStates();
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

    virtual bool coefficients(double b0, double b1, double b2, double a1, double a2)
    {
      if (_stages == N) return false;

      int c_offset = _stages++ * _coeffs_per_stages;

      b0 = FilterUtils::denormDouble(b0);
      b1 = FilterUtils::denormDouble(b1);
      b2 = FilterUtils::denormDouble(b2);
      a1 = FilterUtils::denormDouble(a1);
      a2 = FilterUtils::denormDouble(a2);
      
      __disable_irq();
      _coeffs[c_offset + 0] = b0;
      _coeffs[c_offset + 1] = b1;
      _coeffs[c_offset + 2] = b2;
      _coeffs[c_offset + 3] = a1;
      _coeffs[c_offset + 4] = a2;
      __enable_irq();

      return true;
    }

  private:
    static constexpr int _coeffs_per_stages = 5;
    static constexpr int _states_per_stages = 4;
    bool _active = false;
    int _stages = 0;
    float _states[_states_per_stages * N];
    float _coeffs[_coeffs_per_stages * N];
    arm_biquad_casd_df1_inst_f32 _iir;

    inline void denormStates()
    {
      if (!_active) return;
      int stateCount = _states_per_stages * _stages;
      for (int i = 0; i < stateCount; i++)
      {
        _states[i] = FilterUtils::denormFloat(_states[i]);
      }
    }
};


class HQBiquad: public BiquadBase
{
  public:
    HQBiquad(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      _sample_rate_Hz = sample_rate_Hz;
      reset();
    }

    HQBiquad& reset()
    {
      return *this;
    }

    void begin()
    {

    }

    HQBiquad& setLowpass(double frequency, double q = _invsqrt2)
    {
      lowpass(frequency, q);
      return *this;
    }

    HQBiquad& setHighpass(double frequency, double q = _invsqrt2)
    {
      highpass(frequency, q);
      return *this;
    }

    HQBiquad& setBandpass(double frequency, double q = _invsqrt2)
    {
      bandpass(frequency, q);
      return *this;
    }

    HQBiquad& setNotch(double frequency, double q = _invsqrt2)
    {
      notch(frequency, q);
      return *this;
    }

    HQBiquad& setLowShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      lowShelf(frequency, gain, slope);
      return *this;
    }

    HQBiquad& setHighShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      highShelf(frequency, gain, slope);
      return *this;
    }

    HQBiquad& setPeak(double frequency, double gain, double q = _invsqrt2)
    {
      peak(frequency, gain, q);
      return *this;
    }

    float filter(float x)
    {
      double y = (_b0 * x) + (_b1 * _xn1) + (_b2 * _xn2) + (_a1 * _yn1) + (_a2 * _yn2);
      _xn2 = _xn1;
      _xn1 = x;
      _yn2 = _yn1;
      _yn1 = y;

      _yn1 = FilterUtils::denormDouble(_yn1);

      return (float)y;
    }

    void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      float *pSrcEnd = pSrc + len;
      do
      {
        double x = *pSrc++;
        double y = *pDst++ = (_b0 * x) + (_b1 * _xn1) + (_b2 * _xn2) + (_a1 * _yn1) + (_a2 * _yn2);
        _xn2 = _xn1;
        _xn1 = x;
        _yn2 = _yn1;
        _yn1 = y;
      }
      while (pSrc < pSrcEnd);

      _yn1 = FilterUtils::denormDouble(_yn1);
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

    virtual bool coefficients(double b0, double b1, double b2, double a1, double a2)
    {
      b0 = FilterUtils::denormDouble(b0);
      b1 = FilterUtils::denormDouble(b1);
      b2 = FilterUtils::denormDouble(b2);
      a1 = FilterUtils::denormDouble(a1);
      a2 = FilterUtils::denormDouble(a2);

      __disable_irq();
      _b0 = b0;
      _b1 = b1;
      _b2 = b2;
      _a1 = a1;
      _a2 = a2;
      __enable_irq();

      return true;
    }

  private:
    double _b0 = 1.0;
    double _b1 = 0.0;
    double _b2 = 0.0;
    double _a1 = 0.0;
    double _a2 = 0.0;
    double _xn1 = 0.0;
    double _xn2 = 0.0;
    double _yn1 = 0.0;
    double _yn2 = 0.0;
};


class HQ1p1zBiquad: public BiquadBase
{
  public:
    HQ1p1zBiquad(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      _sample_rate_Hz = sample_rate_Hz;
      reset();
    }

    HQ1p1zBiquad& reset()
    {
      return *this;
    }

    void begin()
    {

    }

    HQ1p1zBiquad& setLowpass1p1z(double frequency)
    {
      lowpass1p1z(frequency);
      return *this;
    }

    HQ1p1zBiquad& setLowpass(double frequency)
    {
      lowpass1p1z(frequency);
      return *this;
    }

    HQ1p1zBiquad& setHighpass1p1z(double frequency)
    {
      highpass1p1z(frequency);
      return *this;
    }

    HQ1p1zBiquad& setHighpass(double frequency)
    {
      highpass1p1z(frequency);
      return *this;
    }

    float filter(float x)
    {
      double dx = (double)x;
      double y = (_b0 * dx) + (_b1 * _xn1) + (_a1 * _yn1);
      _xn1 = dx;
      _yn1 = FilterUtils::denormDouble(y);
      return (float)y;
    }

    inline void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      float *pSrcEnd = pSrc + len;
      do
      {
        double x = (double)*pSrc++;
        double y = (_b0 * x) + (_b1 * _xn1) + (_a1 * _yn1);
        _xn1 = x;
        _yn1 = y;
        *pDst++ = (float)y;
      }
      while (pSrc < pSrcEnd);

      _yn1 = FilterUtils::denormDouble(_yn1);
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

  protected:

    virtual bool coefficients(double b0, double b1, double b2, double a1, double a2)
    {
      b0 = FilterUtils::denormDouble(b0);
      b1 = FilterUtils::denormDouble(b1);
      a1 = FilterUtils::denormDouble(a1);

      __disable_irq();
      _b0 = b0;
      _b1 = b1;
      _a1 = a1;
      __enable_irq();
      
      return true;
    }

  private:
    double _b0 = 1.0;
    double _b1 = 0.0;
    double _a1 = 0.0;
    double _xn1 = 0.0;
    double _yn1 = 0.0;
};

class LPFirstOrder
{
  public:
    LPFirstOrder(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      _sample_rate_Hz = sample_rate_Hz;
    }

    void begin()
    {
    }

    void coefficient(float b)
    {
      b = b < 0.0f ? 0.0f : b > 1.0f ? 1.0f : b;
      _b = FilterUtils::denormFloat(b);
    }

    void frequency(double freq)
    {
      double hw0 = freq * PI / (double)_sample_rate_Hz;
      double k = tan(hw0);
      coefficient((float)((2.0 * k) / (k + 1.0)));
    }

    LPFirstOrder& reset()
    {
      return *this;
    }

    LPFirstOrder& setLowpass(double freq)
    {
      frequency(freq);
      return *this;
    }   

    LPFirstOrder& setLowpass1p1z(double freq)
    {
      frequency(freq);
      return *this;
    }   

    float filter(float x)
    {
      _y += _b * (x - _y);
      _y = FilterUtils::denormFloat(_y);
      return _y;
    }

    inline void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      float *pSrcEnd = pSrc + len;
      do
      {
        *pDst++ = _y += _b * (*pSrc++ - _y);
      }
      while (pSrc < pSrcEnd);

      _y = FilterUtils::denormFloat(_y);
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

  private:
    float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    float _b = 1.0;
    float _y = 0.0f;
};

template <int N = 4>
class StateVariableFilter
{
  public:
    StateVariableFilter(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      _sample_rate_Hz = sample_rate_Hz;
    }

    void frequency(double freq)
    {
      _center = freq * (2.0f / (float)N) * (float)PI / _sample_rate_Hz;
    }

    void control(float control)
    {
      _octave = fasterexp2f(control);
    }

    void resonance(float q)
    {
      _q = 1.0f / (q < 0.1f ? 0.1f : q > 10.0f ? 10.0f : q);
    }

    void mix(float dry, float low, float band, float high)
    {
      __disable_irq();
      _mix_dry = dry;
      _mix_low = low;
      _mix_band = band;
      _mix_high = high;
      __enable_irq();
    }

    void split(float input)
    {
      float f = _center * _octave;
      f = f < 0.0005f ? 0.0005f : f > 0.9995f ? 0.9995f : f;

      for (int i = 0; i < N; i++) {
        _low += f * _band;
        _high = input - _low - _q * _band;
        _band += f * _high;
      }

      _low = FilterUtils::denormFloat(_low);
      _band = FilterUtils::denormFloat(_band);
      _high = FilterUtils::denormFloat(_high);
    }

    inline float filter(float input)
    {
      split(input);
      return (input * _mix_dry) + (_low * _mix_low) + (_band * _mix_band) + (_high * _mix_high);
    }

    inline float lowpass(float input)
    {
      split(input);
      return _low;
    }

    inline float bandpass(float input)
    {
      split(input);
      return _band;
    }

    inline float hipass(float input)
    {
      split(input);
      return _high;
    }

  private:
    float _sample_rate_Hz;
    float _mix_dry = 0.0f;
    float _mix_low = 0.0f;
    float _mix_band = 1.0f;
    float _mix_high = 0.0f;
    float _q = 1.0;
    float _center = 0.1;
    float _octave = 1.0;
    float _low = 0.0;
    float _band = 0.0;
    float _high = 0.0;
    inline float fasterexp2f(float p)
    {
      union {
        uint32_t i;
        float f;
      } v = { uint32_t((1 << 23) * (p + 126.94269504f)) };
      return v.f;
    }
};


class DcBlock
{
  public:
    float filter(float x)
    {
      _yn1 = FilterUtils::denormFloat(x - _xn1 + _a * _yn1);
      _xn1 = x;
      return _yn1;
    }

    inline void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      float *pSrcEnd = pSrc + len;
      do
      {
        float x = *pSrc++;
        *pDst++ = _yn1 = x - _xn1 + _a * _yn1;
        _xn1 = x;
      }
      while (pSrc < pSrcEnd);

      _yn1 = FilterUtils::denormFloat(_yn1);
    }

    inline void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }
  private:
    static constexpr float _a = 0.99725f;
    float _xn1 = 0.0f;
    float _yn1 = 0.0f;
};


class Detector
{
  public:
    inline float detect(float x)
    {
      _yn1 = FilterUtils::denormFloat(x - _xn1 + _a * _yn1);
      _xn1 = x;

      float level = fabsf(_yn1);
      return _level += (level - _level) * (level > _level ? _attack : _decay);
    }

    inline float level()
    {
      return _level;
    }

  private:
    static constexpr float _a = 0.99725f;
    float _xn1 = 0.0f;
    float _yn1 = 0.0f;

    static constexpr float _attack = 1.0f;
    static constexpr float _decay = 0.001f;
    float _level = 0.0f;
};


class BasicLfo
{
  public:
    BasicLfo(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      _sample_rate_Hz = sample_rate_Hz;
      _nyquist = _sample_rate_Hz * 0.5f;
    }

    void freq(float freq)
    {
      freq = freq < 0.0f ? 0.0f : freq > _nyquist ? _nyquist : freq;
      _phaseIncrement = FullRotation * freq / _sample_rate_Hz;
    }

    inline float peek(float phase = 0.0f)
    {
      phase += _phase;
      if (phase > Half) phase -= FullRotation;

      return value(phase);
    }

    inline float next()
    {
      _phase += _phaseIncrement;
      if (_phase > Half) _phase -= FullRotation;
      return value(_phase);
    }

    static constexpr float FullRotation = 4.0f;
    static constexpr float Half = FullRotation / 2.0f;
    static constexpr float Quarter = FullRotation / 4.0f;
    static constexpr float Degree = FullRotation / 360.0f;

  protected:
    float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    float _nyquist = AUDIO_SAMPLE_RATE_EXACT * 0.5f;
    float _phase = 0.0f;
    float _phaseIncrement = 0.0f;
    virtual inline float value(float phase)
    {
      return phase;
    }
};


class TriangleLfo: public BasicLfo
{
  public:
    TriangleLfo(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT) : BasicLfo(sample_rate_Hz) {}
  protected:
    static constexpr float _oneOverQuarter = 1.0f / Quarter;
    virtual inline float value(float phase)
    {
      return (fabsf(phase) - Quarter) * _oneOverQuarter;
    }
};

class SineLfo: public BasicLfo
{
  public:
    SineLfo(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT) : BasicLfo(sample_rate_Hz) {}
  protected:
    static constexpr float _piOverHalf = (float)PI / Half;
    virtual inline float value(float phase)
    {
      return -cosf(phase * _piOverHalf);
    }
};

class FastSineLfo: public BasicLfo
{
  public:
    FastSineLfo(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT) : BasicLfo(sample_rate_Hz) {}
  protected:
    static constexpr float _phaseToIndex = 512.0f / FullRotation;
    virtual inline float value(float phase)
    {
      phase += Half;
      phase *= _phaseToIndex;

      float mod_number;
      float mod_fraction = modff(phase, &mod_number);

      int index = (int)mod_number;
      float a = sinTable512_f32[index];
      float b = sinTable512_f32[index + 1];

      return a + ((b - a) * mod_fraction);
    }
};


//#define CUBIC
#define LAGRANGE
class Delay
{
  public:
    typedef float CDT;

    bool begin(float *delay, int d_length)
    {
      if (delay == _delay && d_length == _max_index) return true;

      if (!delay || d_length < 3) return false;

      _w_index = 0;
      _max_index = d_length;
      _max_delay = (float)(_max_index - 2);
      _dts = _max_index - 1;

      memset(delay, 0, _max_index * sizeof(float));

      _delay = delay;

      return true;
    }

    void time(float sec)
    {
      float samples = AUDIO_SAMPLE_RATE_EXACT * sec;
      _dts = (int)(samples < 0.0f ? 0.0f : samples > _max_delay ? _max_delay : samples);
    }

    void feedback(float feedback)
    {
      _feedback = feedback < -1.0f ? -1.0f : feedback > 1.0f ? 1.0f : feedback;
    }

    void write(float sample)
    {
      if (!_delay) return;
      _delay[_w_index] = sample;
      if (++_w_index >= _max_index) _w_index = 0;
    }

#if defined(CUBIC)
    float read(float delay_samples)
    {
      if (!_delay) return 0.0f;

      delay_samples = delay_samples < 1.0f ? 1.0f : delay_samples > _max_delay ? _max_delay : delay_samples;

      float i;
      float d =  1.0f - modff(delay_samples, &i);
      d = FilterUtils::denormFloat(d);

      int im1 = _w_index - (int)i - 2;
      if (im1 < 0) im1 += _max_index;

      int i0 = im1 + 1;
      if (i0 >= _max_index) i0 = 0;

      int i1 = i0 + 1;
      if (i1 >= _max_index) i1 = 0;

      int i2 = i1 + 1;
      if (i2 >= _max_index) i2 = 0;

      float xm1 = _delay[im1];
      float x0  = _delay[i0];
      float x1  = _delay[i1];
      float x2  = _delay[i2];
      float a = (3.0f * (x0 - x1) - xm1 + x2) * 0.5f;
      float b = 2.0f * x1 + xm1 - (5.0f * x0 + x2) * 0.5f;
      float c = (x1 - xm1) * 0.5f;
      return (((a * d) + b) * d + c) * d + x0;
    }
#elif defined (LAGRANGE)
    float read(float delay_samples)
    {
      if (!_delay) return 0.0f;

      delay_samples = delay_samples < 1.0f ? 1.0f : delay_samples > _max_delay ? _max_delay : delay_samples;

      float i;
      float d = 1.0f - modff(delay_samples, &i);
      d = FilterUtils::denormFloat(d);

      int i0 = _w_index - (int)i - 1;
      if (i0 < 0) i0 += _max_index;

      int i1 = i0 + 1;
      if (i1 >= _max_index) i1 = 0;

      int i2 = i1 + 1;
      if (i2 >= _max_index) i2 = 0;

      float x0  = _delay[i0];
      float x1  = _delay[i1];
      float x2  = _delay[i2];

      float hdm1 = (d - 1.0f) * 0.5f;
      float dm2 = d - 2.0f;

      float d0 = dm2 * hdm1;
      float d1 = -d * dm2;
      float d2 = d * hdm1;

      return x0 * d0 + x1 * d1 + x2 * d2;
    }
#else
    float read(float delay_samples)
    {
      if (!_delay) return 0.0f;

      delay_samples = delay_samples < 0.0f ? 0.0f : delay_samples > _max_delay ? _max_delay : delay_samples;

      float i;
      float d = modff(delay_samples, &i);
      d = FilterUtils::denormFloat(d);

      int i0 = _w_index - (int)i - 1;
      if (i0 < 0) i0 += _max_index;

      int i1 = i0 + 1;
      if (i1 >= _max_index) i1 = 0;

      float s0 = _delay[i0];
      float s1 = _delay[i1];

      return (d * (s0 - s1)) + s1;
    }
#endif

    float readSample(int delay_samples)
    {
      if (!_delay) return 0.0f;

      delay_samples = delay_samples < 0 ? 0 : delay_samples > _max_delay ? _max_delay : delay_samples;

      int index = _w_index - delay_samples - 1;
      if (index < 0) index += _max_index;

      return _delay[index];
    }

    float delay(float input)
    {
      write(input);
      return readSample(_dts);
    }

    float allpass(float input)
    {
      float bufout = readSample(_dts);
      float temp = input * -_feedback;
      float output = bufout + temp;
      write(input + (output * _feedback));
      return output;
    }

    float allpass(float input, float delay_samples)
    {
      float bufout = read(delay_samples);
      float temp = input * -_feedback;
      float output = bufout + temp;
      write(input + (output * _feedback));
      return output;
    }

    inline void delayArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;
      
      float *pSrcEnd = pSrc + len;
      do
      {
        write(*pSrc++);
        *pDst++ = readSample(_dts);
      }
      while (pSrc < pSrcEnd);
    }

    void delayBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (pSrc == nullptr) return;
      if (pDst == nullptr) pDst = pSrc;

      delayArray(pSrc->data, pDst->data, pSrc->length);
    }

    float maxTime()
    {
      return _max_delay / AUDIO_SAMPLE_RATE_EXACT;
    }

    static constexpr uint16_t bufferSizeMs(float ms)
    {
      return (uint16_t)(AUDIO_SAMPLE_RATE_EXACT * ms * 0.001f) + 2;
    }

    static constexpr uint16_t bufferSize(float sec)
    {
      return (uint16_t)(AUDIO_SAMPLE_RATE_EXACT * sec) + 2;
    }

  protected:
    uint16_t _max_index = 0;
    float _max_delay = 0.0f;
    uint16_t _w_index = 0;
    float *_delay = nullptr;
    int _dts = 0;
    float _feedback = 0.0f;
    float _vn_1 = 0.0f;
};


template <uint16_t N>
class StaticDelay: public Delay
{
  public:
    StaticDelay()
    {
      _delay = _static_delay;
      _max_index = N;
      _max_delay = (float)(N - 2);
      _dts = N - 1;
    }

  protected:
    float _static_delay[N];
};


class LQDelay
{
  public:
    typedef int16_t CDT;

    bool begin(int16_t *delay, int d_length, float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      if (delay == _delay && d_length == _max_index) return true;

      if (!delay || d_length < 2) return false;

      _f.reset().setLowpass(sample_rate_Hz * 0.20f).begin();

      _w_index = 0;
      _max_index = d_length;
      _max_delay = (float)(_max_index - 2);

      memset(delay, 0, _max_index * sizeof(int16_t));

      _delay = delay;

      return true;
    }

    void write(float sample)
    {
      if (!_delay) return;

      sample = _dc.filter(sample);
      sample = _f.filter(sample);

      _skip = !_skip;
      if (_skip) return;

      int32_t is = (int32_t)(sample * _ftoi);
      is = is < INT16_MIN ? INT16_MIN : is > INT16_MAX ? INT16_MAX : is;

      _delay[_w_index] = is;

      if (++_w_index >= _max_index) _w_index = 0;
    }

    float read(float delay_samples)
    {
      if (!_delay) return 0.0f;

      delay_samples *= 0.5f;
      delay_samples = delay_samples < 0.0f ? 0.0f : delay_samples > _max_delay ? _max_delay : delay_samples;

      float mod_number;
      float mod_fraction = modff(delay_samples, &mod_number);

      int index = _w_index - (int)mod_number - 2;
      while (index < 0) index += _max_index;

      int next = index + 1;
      if (next >= _max_index) next = 0;

      float s0 = (float)_delay[index] * _itof;
      float s1 = (float)_delay[next] * _itof;

      return (mod_fraction * (s0 - s1)) + s1;
    }

    static constexpr uint16_t bufferSizeMs(float ms)
    {
      return (uint16_t)(AUDIO_SAMPLE_RATE_EXACT * ms * 0.001f) + 2;
    }

    static constexpr uint16_t bufferSize(float sec)
    {
      return (uint16_t)(AUDIO_SAMPLE_RATE_EXACT * sec) + 2;
    }

  protected:
    static constexpr float _ftoi = 32768.0f / 1.0f;
    static constexpr float _itof = 1.0f / _ftoi;
    DcBlock _dc;
    CascadeBiquad<1> _f;
    bool _skip = true;
    uint16_t _max_index = 0;
    float _max_delay = 0.0f;
    uint16_t _w_index = 0;
    int16_t* _delay = nullptr;
    float _last = 0.0f;
};


template <uint16_t N>
class DelayFilter
{
  public:
    void damping(float damping)
    {
      _damping = damping < 0.0f ? 0.0f : damping > 1.0f ? 1.0f : damping;
    }

    void feedback(float feedback)
    {
      _feedback = feedback < -1.0f ? -1.0f : feedback > 1.0f ? 1.0f : feedback;
    }

    float ap_mverb(float input)
    {
      if (++_index >= N) _index = 0;
      float bufout = _delay[_index];
      float temp = input * -_feedback;
      float output = bufout + temp;
      _delay[_index] = input + (output * _feedback);
      return output;
    }

    float ap_freeverb(float input)
    {
      if (++_index >= N) _index = 0;
      float bufout = _delay[_index];
      float output = -input + bufout;
      _delay[_index] = input + (bufout * _feedback);
      return output;
    }

    // Dattorro
    float allpass(float input)
    {
      if (++_index >= N) _index = 0;
      float bufout = _delay[_index];
      float bufin = _delay[_index] = input - (bufout * _feedback);
      return bufout + (bufin * _feedback);
    }

    // Freeverb
    float comb(float input)
    {
      if (++_index >= N) _index = 0;
      float output = _delay[_index];
      _state += (output - _state) * _damping;
      _delay[_index] = (_state * _feedback) + input;
      return output;
    }

    float delay(float input)
    {
      if (++_index >= N) _index = 0;
      float output = _delay[_index];
      _delay[_index] = input;
      return output;
    }

    float readSample(int t)
    {
      //t = t < 0 ? 0 : t > (N - 1) ? (N - 1) : t;
      int i = _index - t;
      while (i < 0) i += N;
      return _delay[i];
    }

    float operator [](int t)
    {
      int i = _index - t;
      while (i < 0) i += N;
      return _delay[i];
    }

  protected:
    float _delay[N];
    float _feedback = 0.0f;
    float _damping = 0.5f;
    float _state = 0.0f;
    int _index = -1;
};

template <int N = 0>
class Freeverb
{
  public:
    Freeverb()
    {
      _ap1.feedback(0.5f);
      _ap2.feedback(0.5f);
      _ap3.feedback(0.5f);
      _ap4.feedback(0.5f);
      size();
      damping();
    }

    void size(float n = 0.5f)
    {
      if (n > 1.0f) n = 1.0f;
      else if (n < 0.0f) n = 0.0f;
      float feeback = (n * 0.28f) + 0.7f;

      _cf1.feedback(feeback);
      _cf2.feedback(feeback);
      _cf3.feedback(feeback);
      _cf4.feedback(feeback);
      _cf5.feedback(feeback);
      _cf6.feedback(feeback);
      _cf7.feedback(feeback);
      _cf8.feedback(feeback);
    }

    void damping(float n = 0.5f)
    {
      if (n > 1.0f) n = 1.0f;
      else if (n < 0.0f) n = 0.0f;
      float damping = 1.0f - (n * 0.4f);

      _cf1.damping(damping);
      _cf2.damping(damping);
      _cf3.damping(damping);
      _cf4.damping(damping);
      _cf5.damping(damping);
      _cf6.damping(damping);
      _cf7.damping(damping);
      _cf8.damping(damping);
    }

    void processArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (pSrc == nullptr || pDst == nullptr || len == 0) return;

      float *pSrcEnd = pSrc + len;
      do
      {
        float input = *pSrc++;

        float output = _cf1.comb(input);
        output += _cf2.comb(input);
        output += _cf3.comb(input);
        output += _cf4.comb(input);
        output += _cf5.comb(input);
        output += _cf6.comb(input);
        output += _cf7.comb(input);
        output += _cf8.comb(input);

        output = _ap1.allpass(output);
        output = _ap2.allpass(output);
        output = _ap3.allpass(output);
        output = _ap4.allpass(output);

        *pDst++ = output;
      }
      while (pSrc < pSrcEnd);
    }

    void processBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (pSrc == nullptr) return;
      if (pDst == nullptr) pDst = pSrc;

      processArray(pSrc->data, pDst->data, pSrc->length);
    }
    
  protected:
    DelayFilter < 1116 + N > _cf1;
    DelayFilter < 1188 + N > _cf2;
    DelayFilter < 1277 + N > _cf3;
    DelayFilter < 1356 + N > _cf4;
    DelayFilter < 1422 + N > _cf5;
    DelayFilter < 1491 + N > _cf6;
    DelayFilter < 1557 + N > _cf7;
    DelayFilter < 1617 + N > _cf8;
    DelayFilter < 225 + N > _ap1;
    DelayFilter < 556 + N > _ap2;
    DelayFilter < 441 + N > _ap3;
    DelayFilter < 341 + N > _ap4;
};

typedef Freeverb<0> FreeverbL;
typedef Freeverb<23> FreeverbR;

#endif
