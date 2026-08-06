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
    static inline float u2dB(float u)
    {
      return u < 5.011872E-07f ? -126.0f : 20.0f * log10f(u);
    }

    static inline float dB2u(float db)
    {
      return powf(10.0f, db * (1.0f / 20.0f));
    }
};

template <uint32_t N>
class Multirate
{
  public:
    Multirate():
      _gain((float)N),
      _w0(0.9 * PI / (double)N),
      _alpha(sin(_w0) * 0.707106781),
      _cosW0(cos(_w0)),
      _norm(1.0 / (1.0 + _alpha)),
      _b0(((1.0 - _cosW0) / 2.0) * _norm),
      _b1((1.0 - _cosW0) * _norm),
      _b2(_b0),
      _a1((2.0 * _cosW0) * _norm),
      _a2(-(1.0 - _alpha) * _norm)
    {
      static_assert(N > 1);
    }

    Multirate(Multirate const&) = delete;
    Multirate& operator=(Multirate const&) = delete;

    void reset()
    {
      _up_xn1 = 0.0f;
      _up_xn2 = 0.0f;
      _up_yn1 = 0.0f;
      _up_yn2 = 0.0f;

      _down_xn1 = 0.0f;
      _down_xn2 = 0.0f;
      _down_yn1 = 0.0f;
      _down_yn2 = 0.0f;
    }

    uint32_t interpolate(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return 0;

      float xn1 = _up_xn1;
      float xn2 = _up_xn2;
      float yn1 = _up_yn1;
      float yn2 = _up_yn2;
      float x, y;

      float *pSrcEnd = pSrc + len;
      while (pSrc < pSrcEnd)
      {
        x = *pSrc++ * _gain;

        y = (_b0 * x) + (_b1 * xn1) + (_b2 * xn2) + (_a1 * yn1) + (_a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = y;

        for (uint32_t i = 1; i < N; i++)
        {
          y = (_b1 * xn1) + (_b2 * xn2) + (_a1 * yn1) + (_a2 * yn2);
          xn2 = xn1;
          xn1 = 0.0f;
          yn2 = yn1;
          yn1 = y;
          *pDst++ = y;
        }
      }

      _up_xn1 = xn1;
      _up_xn2 = xn2;
      _up_yn1 = yn1;
      _up_yn2 = yn2;

      return len * N;
    }

    uint32_t decimate(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return 0;

      float xn1 = _down_xn1;
      float xn2 = _down_xn2;
      float yn1 = _down_yn1;
      float yn2 = _down_yn2;
      float x, y;

      float *pSrcEnd = pSrc + len;
      while (pSrc < pSrcEnd)
      {
        for (uint32_t i = 0; i < N; i++)
        {
          x = *pSrc++;
          y = (_b0 * x) + (_b1 * xn1) + (_b2 * xn2) + (_a1 * yn1) + (_a2 * yn2);
          xn2 = xn1;
          xn1 = x;
          yn2 = yn1;
          yn1 = y;
        }
        
        *pDst++ = y;
      }

      _down_xn1 = xn1;
      _down_xn2 = xn2;
      _down_yn1 = yn1;
      _down_yn2 = yn2;

      return len / N;
    }

  private:
    const float _gain = 1.0f;
    const double _w0;
    const double _alpha;
    const double _cosW0;
    const double _norm;
    const float _b0;
    const float _b1;
    const float _b2;
    const float _a1;
    const float _a2;

    float _up_xn1 = 0.0f;
    float _up_xn2 = 0.0f;
    float _up_yn1 = 0.0f;
    float _up_yn2 = 0.0f;

    float _down_xn1 = 0.0f;
    float _down_xn2 = 0.0f;
    float _down_yn1 = 0.0f;
    float _down_yn2 = 0.0f;
};


class BiquadBase
{
  public:
    BiquadBase(double sample_rate_Hz = (double)AUDIO_SAMPLE_RATE_EXACT):
      _sample_rate_Hz(sample_rate_Hz)
    {
    }

    virtual ~BiquadBase() {}
    BiquadBase(BiquadBase const&) = delete;
    BiquadBase& operator=(BiquadBase const&) = delete;

  protected:
    static constexpr double _invsqrt2 = 0.7071067811865475;
    const double _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;

    virtual bool coefficients(double b0, double b1, double b2, double a1, double a2) = 0;

    bool lowpassFirstOrder(double frequency)
    {
      if (frequency <= 0.0f) return false;

      double k = calc_k(frequency);
      double norm = 1.0 / (1.0 / k + 1.0);
      double b0 = norm;
      double b1 = norm;
      double a1 = -(1.0 - 1.0 / k) * norm;

      return coefficients(b0, b1, 0.0, a1, 0.0);
    }

    bool highpassFirstOrder(double frequency)
    {
      if (frequency <= 0.0f) return false;

      double k = calc_k(frequency);
      double norm = 1.0 / (k + 1.0);
      double b0 = norm;
      double b1 = -norm;
      double a1 = -(k - 1.0) * norm;

      return coefficients(b0, b1, 0.0, a1, 0.0);
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

      double a = calc_a(gain);
      double w0 = calc_w0(frequency);
      double sinW0 = sin(w0);
      double cosW0 = cos(w0);
      double sinsq = sinW0 * sqrt((pow(a, 2.0) + 1.0) * (1.0 / slope - 1.0) + 2.0 * a);
      double aMinus = (a - 1.0) * cosW0;
      double aPlus = (a + 1.0) * cosW0;
      double inv_a0 = 1.0 / ((a + 1.0) + aMinus + sinsq);

      double b0 = a * ((a + 1.0) - aMinus + sinsq) * inv_a0;
      double b1 = 2.0 * a * ((a - 1.0) - aPlus) * inv_a0;
      double b2 = a * ((a + 1.0) - aMinus - sinsq) * inv_a0;
      double a1 = 2.0 * ((a - 1.0) + aPlus) * inv_a0;
      double a2 = -((a + 1.0) + aMinus - sinsq) * inv_a0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool lowShelfFirstOrder(double frequency, double gain)
    {
      if (frequency <= 0.0f) return false;

      double b0, b1, a1;
      double v = calc_v(gain);
      double k = calc_k(frequency);
      if (gain >= 0.0)
      {
        double norm = 1.0 / (k + 1.0);
        b0 = (k * v + 1.0) * norm;
        b1 = (k * v - 1.0) * norm;
        a1 = -(k - 1.0) * norm;
      }
      else
      {	
        double norm = 1.0 / (k * v + 1.0);
        b0 = (k + 1.0) * norm;
        b1 = (k - 1.0) * norm;
        a1 = -(k * v - 1.0) * norm;
      }

      return coefficients(b0, b1, 0.0, a1, 0.0);
    }

    bool highShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      if (frequency <= 0.0f) return false;

      double w0 = calc_w0(frequency);
      double a = calc_a(gain);
      double sinW0 = sin(w0);
      double cosW0 = cos(w0);
      double sinsq = sinW0 * sqrt((pow(a, 2.0) + 1.0) * (1.0 / slope - 1.0) + 2.0 * a);
      double aMinus = (a - 1.0) * cosW0;
      double aPlus = (a + 1.0) * cosW0;
      double inv_a0 = 1.0 / ((a + 1.0) - aMinus + sinsq);

      double b0 = a * ((a + 1.0) + aMinus + sinsq) * inv_a0;
      double b1 = -2.0 * a * ((a - 1.0) + aPlus) * inv_a0;
      double b2 = a * ((a + 1.0) + aMinus - sinsq) * inv_a0;
      double a1 = -2.0 * ((a - 1.0) - aPlus) * inv_a0;
      double a2 = -((a + 1.0) - aMinus - sinsq) * inv_a0;

      return coefficients(b0, b1, b2, a1, a2);
    }

    bool highShelfFirstOrder(double frequency, double gain)
    {
      if (frequency <= 0.0f) return false;

      double b0, b1, a1;
      double v = calc_v(gain);
      double k = calc_k(frequency);
      if (gain >= 0.0)
      {
        double norm = 1.0 / (k + 1.0);
        b0 = (k + v) * norm;
        b1 = (k - v) * norm;
        a1 = -(k - 1.0) * norm;
      }
      else
      {	
        double norm = 1.0 / (k + v);
        b0 = (k + 1.0) * norm;
        b1 = (k - 1.0) * norm;
        a1 = -(k - v) * norm;
      }

      return coefficients(b0, b1, 0.0, a1, 0.0);
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


class Biquad: public BiquadBase
{
  public:
    Biquad(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT):
      BiquadBase(sample_rate_Hz)
    {
    }

    virtual ~Biquad() {}
    Biquad(Biquad const&) = delete;
    Biquad& operator=(Biquad const&) = delete;

    void reset()
    {
      _xn2 = 0.0f;
      _xn1 = 0.0f;
      _yn2 = 0.0f;
      _yn1 = 0.0f;
    }

    void setBypass()
    {
      coefficients(1.0, 0.0, 0.0, 0.0, 0.0);
    }

    void setLowpass(double frequency, double q = _invsqrt2)
    {
      lowpass(frequency, q);
    }

    void setHighpass(double frequency, double q = _invsqrt2)
    {
      highpass(frequency, q);
    }

    void setBandpass(double frequency, double q = _invsqrt2)
    {
      bandpass(frequency, q);
    }

    void setNotch(double frequency, double q = _invsqrt2)
    {
      notch(frequency, q);
    }

    void setLowShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      lowShelf(frequency, gain, slope);
    }

    void setHighShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      highShelf(frequency, gain, slope);
    }

    void setPeak(double frequency, double gain, double q = _invsqrt2)
    {
      peak(frequency, gain, q);
    }

    float filter(float x)
    {
      float y = (_b0 * x) + (_b1 * _xn1) + (_b2 * _xn2) + (_a1 * _yn1) + (_a2 * _yn2);
      _xn2 = _xn1;
      _xn1 = x;
      _yn2 = _yn1;
      _yn1 = y;

      return y;
    }

    void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      if (!_active)
      {
        if (pSrc != pDst) arm_copy_f32(pSrc, pDst, len);
        return;
      }

      float b0 = _b0;
      float b1 = _b1;
      float b2 = _b2;
      float a1 = _a1;
      float a2 = _a2;
      float xn1 = _xn1;
      float xn2 = _xn2;
      float yn1 = _yn1;
      float yn2 = _yn2;
      float x, y;
      float *pSrcEnd = pSrc + len;

      uint32_t batches = len / 4U;
      while (batches-- > 0U)
      {
        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = y;

        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = y;

        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = y;

        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = y;
      }

      while (pSrc < pSrcEnd)
      {
        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = y;
      }
      
      _xn1 = xn1;
      _xn2 = xn2;
      _yn1 = yn1;
      _yn2 = yn2;
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

    bool coefficients(double b0, double b1, double b2, double a1, double a2) override
    {
      __disable_irq();
      _b0 = b0;
      _b1 = b1;
      _b2 = b2;
      _a1 = a1;
      _a2 = a2;
      __enable_irq();

      return _active = _b0 != 1.0 || _b1 != 0.0 || _b2 != 0.0 || _a1 != 0.0 || _a2 != 0.0;
    }

  private:
    bool _active = false;
    float _b0 = 1.0f;
    float _b1 = 0.0f;
    float _b2 = 0.0f;
    float _a1 = 0.0f;
    float _a2 = 0.0f;
    float _xn1 = 0.0f;
    float _xn2 = 0.0f;
    float _yn1 = 0.0f;
    float _yn2 = 0.0f;
};


class HQBiquad: public BiquadBase
{
  public:
    HQBiquad(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT):
      BiquadBase(sample_rate_Hz)
    {
    }

    virtual ~HQBiquad() {}
    HQBiquad(HQBiquad const&) = delete;
    HQBiquad& operator=(HQBiquad const&) = delete;

    void reset()
    {
      _xn2 = 0.0;
      _xn1 = 0.0;
      _yn2 = 0.0;
      _yn1 = 0.0;
    }

    void setBypass()
    {
      coefficients(1.0, 0.0, 0.0, 0.0, 0.0);
    }

    void setLowpass(double frequency, double q = _invsqrt2)
    {
      lowpass(frequency, q);
    }

    void setHighpass(double frequency, double q = _invsqrt2)
    {
      highpass(frequency, q);
    }

    void setBandpass(double frequency, double q = _invsqrt2)
    {
      bandpass(frequency, q);
    }

    void setNotch(double frequency, double q = _invsqrt2)
    {
      notch(frequency, q);
    }

    void setLowShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      lowShelf(frequency, gain, slope);
    }

    void setHighShelf(double frequency, double gain, double slope = _invsqrt2)
    {
      highShelf(frequency, gain, slope);
    }

    void setPeak(double frequency, double gain, double q = _invsqrt2)
    {
      peak(frequency, gain, q);
    }

    float filter(float x)
    {
      double dx = (double)x;
      double y = (_b0 * dx) + (_b1 * _xn1) + (_b2 * _xn2) + (_a1 * _yn1) + (_a2 * _yn2);
      _xn2 = _xn1;
      _xn1 = dx;
      _yn2 = _yn1;
      _yn1 = y;

      return (float)y;
    }

    void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      if (!_active)
      {
        if (pSrc != pDst) arm_copy_f32(pSrc, pDst, len);
        return;
      }

      double b0 = _b0;
      double b1 = _b1;
      double b2 = _b2;
      double a1 = _a1;
      double a2 = _a2;
      double xn1 = _xn1;
      double xn2 = _xn2;
      double yn1 = _yn1;
      double yn2 = _yn2;
      double x, y;
      float *pSrcEnd = pSrc + len;

      uint32_t batches = len / 4U;
      while (batches-- > 0U)
      {
        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = (float)y;

        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = (float)y;

        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = (float)y;

        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = (float)y;
      }

      while (pSrc < pSrcEnd)
      {
        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (b2 * xn2) + (a1 * yn1) + (a2 * yn2);
        xn2 = xn1;
        xn1 = x;
        yn2 = yn1;
        yn1 = y;
        *pDst++ = (float)y;
      }
      
      _xn1 = xn1;
      _xn2 = xn2;
      _yn1 = yn1;
      _yn2 = yn2;
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

  protected:
    bool coefficients(double b0, double b1, double b2, double a1, double a2) override
    {
      __disable_irq();
      _b0 = b0;
      _b1 = b1;
      _b2 = b2;
      _a1 = a1;
      _a2 = a2;
      __enable_irq();

      return _active = _b0 != 1.0 || _b1 != 0.0 || _b2 != 0.0 || _a1 != 0.0 || _a2 != 0.0;
    }

  private:
    bool _active = false;
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


class HQFirstOrder: public BiquadBase
{
  public:
    HQFirstOrder(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT):
      BiquadBase(sample_rate_Hz)
    {
    }

    virtual ~HQFirstOrder() {}
    HQFirstOrder(HQFirstOrder const&) = delete;
    HQFirstOrder& operator=(HQFirstOrder const&) = delete;

    void reset()
    {
      _xn1 = 0.0;
      _yn1 = 0.0;
    }

    void setBypass()
    {
      coefficients(1.0, 0.0, 0.0, 0.0, 0.0);
    }

    void setLowpassFirstOrder(double frequency)
    {
      lowpassFirstOrder(frequency);
    }

    void setHighpassFirstOrder(double frequency)
    {
      highpassFirstOrder(frequency);
    }

    void setLowShelfFirstOrder(double frequency, double gain)
    {
      lowShelfFirstOrder(frequency, gain);
    }

    void setHighShelfFirstOrder(double frequency, double gain)
    {
      highShelfFirstOrder(frequency, gain);
    }

    float filter(float x)
    {
      double dx = (double)x;
      double y = (_b0 * dx) + (_b1 * _xn1) + (_a1 * _yn1);
      _xn1 = dx;
      _yn1 = y;
      return (float)y;
    }

    void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      if (!_active)
      {
        if (pSrc != pDst) arm_copy_f32(pSrc, pDst, len);
        return;
      }

      double b0 = _b0;
      double b1 = _b1;
      double a1 = _a1;
      double xn1 = _xn1;
      double yn1 = _yn1;
      double x, y;
      float *pSrcEnd = pSrc + len;

      uint32_t batches = len / 4U;
      while (batches-- > 0U)
      {
        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = (float)y;
        
        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = (float)y;
        
        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = (float)y;
        
        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = (float)y;
      }

      while (pSrc < pSrcEnd)
      {
        x = (double)*pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = (float)y;
      }
      
      _xn1 = xn1;
      _yn1 = yn1;
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

  protected:
    bool coefficients(double b0, double b1, double b2, double a1, double a2) override
    {
      __disable_irq();
      _b0 = b0;
      _b1 = b1;
      _a1 = a1;
      __enable_irq();
      
      return _active = _b0 != 1.0 || _b1 != 0.0 || _a1 != 0.0;
    }

  private:
    bool _active = false;
    double _b0 = 1.0;
    double _b1 = 0.0;
    double _a1 = 0.0;
    double _xn1 = 0.0;
    double _yn1 = 0.0;
};


class FirstOrder: public BiquadBase
{
  public:
    FirstOrder(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT):
      BiquadBase(sample_rate_Hz)
    {
    }

    virtual ~FirstOrder() {}
    FirstOrder(FirstOrder const&) = delete;
    FirstOrder& operator=(FirstOrder const&) = delete;

    void reset()
    {
      _xn1 = 0.0f;
      _yn1 = 0.0f;
    }

    void setBypass()
    {
      coefficients(1.0, 0.0, 0.0, 0.0, 0.0);
    }

    void setLowpassFirstOrder(double frequency)
    {
      lowpassFirstOrder(frequency);
    }

    void setHighpassFirstOrder(double frequency)
    {
      highpassFirstOrder(frequency);
    }

    void setLowShelfFirstOrder(double frequency, double gain)
    {
      lowShelfFirstOrder(frequency, gain);
    }

    void setHighShelfFirstOrder(double frequency, double gain)
    {
      highShelfFirstOrder(frequency, gain);
    }

    float filter(float x)
    {
      float y = (_b0 * x) + (_b1 * _xn1) + (_a1 * _yn1);
      _xn1 = x;      
      _yn1 = y;
      return y;
    }

    void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      if (!_active)
      {
        if (pSrc != pDst) arm_copy_f32(pSrc, pDst, len);
        return;
      }

      float b0 = _b0;
      float b1 = _b1;
      float a1 = _a1;
      float xn1 = _xn1;
      float yn1 = _yn1;
      float x, y;
      float *pSrcEnd = pSrc + len;
      uint32_t batches = len / 4U;
      while (batches-- > 0U)
      {
        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = y;
        
        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = y;
        
        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = y;
        
        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = y;
      }

      while (pSrc < pSrcEnd)
      {
        x = *pSrc++;
        y = (b0 * x) + (b1 * xn1) + (a1 * yn1);
        xn1 = x;
        yn1 = y;
        *pDst++ = y;
      }

      _xn1 = xn1;
      _yn1 = yn1;
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

  protected:
    bool coefficients(double b0, double b1, double b2, double a1, double a2) override
    {
      (void)b2;
      (void)a2;

      __disable_irq();
      _b0 = (float)b0;
      _b1 = (float)b1;
      _a1 = (float)a1;
      __enable_irq();
      
      return _active = _b0 != 1.0f || _b1 != 0.0f || _a1 != 0.0f;
    }

  private:
    bool _active = false;
    float _b0 = 1.0f;
    float _b1 = 0.0f;
    float _a1 = 0.0f;
    float _xn1 = 0.0f;
    float _yn1 = 0.0f;
};


class LPFirstOrder
{
  public:
    LPFirstOrder(double sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT):
      _sample_rate_Hz(sample_rate_Hz)
    {
    }

    virtual ~LPFirstOrder() {}
    LPFirstOrder(LPFirstOrder const&) = delete;
    LPFirstOrder& operator=(LPFirstOrder const&) = delete;

    void begin()
    {
    }

    void coefficient(float b)
    {
      _b = b < 0.0f ? 0.0f : b > 1.0f ? 1.0f : b;
    }

    void frequency(double freq)
    {
      double hw0 = freq * PI / (double)_sample_rate_Hz;
      double k = tan(hw0);
      coefficient((float)((2.0 * k) / (k + 1.0)));
    }

    LPFirstOrder& reset()
    {
      _yn1 = 0.0f;
      coefficient(1.0f);
      return *this;
    }

    LPFirstOrder& setLowpass(double freq)
    {
      frequency(freq);
      return *this;
    }   

    LPFirstOrder& setLowpassFirstOrder(double freq)
    {
      frequency(freq);
      return *this;
    }   

    float filter(float x)
    {
      return _yn1 += (x - _yn1) * _b;
    }

    void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      float b = _b;
      float yn1 = _yn1;
      float *pSrcEnd = pSrc + len;
      uint32_t batches = len / 4U;
      while (batches-- > 0U)
      {
        *pDst++ = yn1 += (*pSrc++ - yn1) * b;
        *pDst++ = yn1 += (*pSrc++ - yn1) * b;
        *pDst++ = yn1 += (*pSrc++ - yn1) * b;
        *pDst++ = yn1 += (*pSrc++ - yn1) * b;
      }
      while (pSrc < pSrcEnd)
      {
        *pDst++ = yn1 += (*pSrc++ - yn1) * b;
      }
      
      _yn1 = yn1;
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (!pSrc) return;

      if (!pDst) pDst = pSrc;

      filterArray(pSrc->data, pDst->data, pSrc->length);
    }

  private:
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    float _b = 1.0f;
    float _yn1 = 0.0f;
};

template <uint32_t N>
class StateVariableFilter
{
  public:
    StateVariableFilter(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT):
      _sample_rate_Hz(sample_rate_Hz)
    {
    }

    virtual ~StateVariableFilter() {}
    StateVariableFilter(StateVariableFilter const&) = delete;
    StateVariableFilter& operator=(StateVariableFilter const&) = delete;

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
      _q = 1.0f / (q < 0.2f ? 0.2f : q > 10.0f ? 10.0f : q);
    }

    void mix(float dry, float low, float band, float high)
    {
      _mix_dry = dry;
      _mix_low = low;
      _mix_band = band;
      _mix_high = high;
    }

    void split(float input)
    {
      float f = _center * _octave;
      f = f < 0.0005f ? 0.0005f : f > 0.9995f ? 0.9995f : f;

      float q = _q;
      float low = _low;
      float high = _high;
      float band = _band;

      for (uint32_t i = 0; i < N; i++)
      {
        low += f * band;
        high = input - low - q * band;
        band += f * high;
      }

      _low = low;
      _high = high;
      _band = band;
    }

    void splitArray(float *pSrc, float *pDstLow, float *pDstBand, float *pDstHigh, uint32_t len)
    {
      if (!pSrc || len == 0) return;

      float *pSrcEnd = pSrc + len;
      while (pSrc < pSrcEnd)
      {
        split(*pSrc++);
        if (pDstLow) *pDstLow++ = _low;
        if (pDstBand) *pDstBand++ = _band;
        if (pDstHigh) *pDstHigh++ = _high;
      }
    }

    void splitBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDstLow = nullptr, audio_block_f32_t *pDstBand = nullptr, audio_block_f32_t *pDstHigh = nullptr)
    {
      if (!pSrc) return;

      splitArray(pSrc->data, pDstLow->data, pDstBand->data, pDstHigh->data, pSrc->length);
    }

    inline float low()
    {
      return _low;
    }

    inline float band()
    {
      return _band;
    }

    inline float high()
    {
      return _high;
    }

    inline float filter(float input)
    {
      split(input);
      float out = input * _mix_dry;
      out += _low * _mix_low;
      out += _band * _mix_band;
      out += _high * _mix_high;
      return out;
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
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
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
    DcBlock() {}
    virtual ~DcBlock() {}
    DcBlock(DcBlock const&) = delete;
    DcBlock& operator=(DcBlock const&) = delete;

    float filter(float x)
    {
      _yn1 = (_a * _yn1) + x - _xn1;
      _xn1 = x;
      return _yn1;
    }

    void filterArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;

      float a = _a;
      float yn1 = _yn1;
      float xn1 = _xn1;
      float x;
      float *pSrcEnd = pSrc + len;
      uint32_t batches = len / 4U;
      while (batches-- > 0U)
      {
        x = *pSrc++;
        *pDst++ = yn1 = (a * yn1) + x - xn1;
        xn1 = x;

        x = *pSrc++;
        *pDst++ = yn1 = (a * yn1) + x - xn1;
        xn1 = x;

        x = *pSrc++;
        *pDst++ = yn1 = (a * yn1) + x - xn1;
        xn1 = x;

        x = *pSrc++;
        *pDst++ = yn1 = (a * yn1) + x - xn1;
        xn1 = x;
      }
      while (pSrc < pSrcEnd)
      {
        x = *pSrc++;
        *pDst++ = yn1 = (a * yn1) + x - xn1;
        xn1 = x;
      }

      _xn1 = xn1;
      _yn1 = yn1;
    }

    void filterBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
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
    Detector() {}
    virtual ~Detector() {}
    Detector(Detector const&) = delete;
    Detector& operator=(Detector const&) = delete;

    inline float detect(float x)
    {
      _yn1 = (_a * _yn1) + x - _xn1;
      _xn1 = x;

      float level = fabsf(_yn1);
      return _level += (level - _level) * (level > _level ? _attack : _decay);
    }

    inline float level() const
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


class LfoBase
{
  public:
    LfoBase(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT)
    {
      _sample_rate_Hz = sample_rate_Hz;
      _nyquist = _sample_rate_Hz * 0.5f;
    }

    virtual ~LfoBase() {}
    LfoBase(LfoBase const&) = delete;
    LfoBase& operator=(LfoBase const&) = delete;

    void freq(float freq)
    {
      freq = freq < 0.0f ? 0.0f : freq > _nyquist ? _nyquist : freq;
      _phaseIncrement = FullRotation * freq / _sample_rate_Hz;
    }

    inline float peek(float phase = 0.0f) const
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
    virtual inline float value(float phase) const
    {
      return phase;
    }
};


class TriangleLfo: public LfoBase
{
  public:
    TriangleLfo(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT) : LfoBase(sample_rate_Hz) {}

    virtual ~TriangleLfo() {}
    TriangleLfo(TriangleLfo const&) = delete;
    TriangleLfo& operator=(TriangleLfo const&) = delete;

  protected:
    static constexpr float _oneOverQuarter = 1.0f / Quarter;
    virtual inline float value(float phase) const
    {
      return (fabsf(phase) - Quarter) * _oneOverQuarter;
    }
};


class SineLfo: public LfoBase
{
  public:
    SineLfo(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT) : LfoBase(sample_rate_Hz) {}

    virtual ~SineLfo() {}
    SineLfo(SineLfo const&) = delete;
    SineLfo& operator=(SineLfo const&) = delete;

  protected:
    static constexpr float _piOverHalf = (float)PI / Half;
    virtual inline float value(float phase) const
    {
      return -cosf(phase * _piOverHalf);
    }
};


class ModifiedSineLfo: public LfoBase
{
  public:
    ModifiedSineLfo(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT) : LfoBase(sample_rate_Hz) {}

    virtual ~ModifiedSineLfo() {}
    ModifiedSineLfo(ModifiedSineLfo const&) = delete;
    ModifiedSineLfo& operator=(ModifiedSineLfo const&) = delete;

    void harmonic(int order, float level)
    {
      _order = (float)(order < 1 ? 1 : order);
      _level = powf(level, 2);
      _comp = -1.0f / (1.0f + _level);
    }

  protected:
    static constexpr float _piOverHalf = (float)PI / Half;
    float _order = 0.0f;
    float _level = 0.0f;
    float _comp = 1.0f;

    virtual inline float value(float phase) const
    {
      float phi = phase * _piOverHalf;
      float amplitude = (cosf(phi * _order) * _level) + cosf(phi);
      return amplitude * _comp;
    }
};


class DelayBase
{
  public:
    DelayBase() {}
    virtual ~DelayBase() {}
    DelayBase(DelayBase const&) = delete;
    DelayBase& operator=(DelayBase const&) = delete;

    void time(float sec)
    {
      float samples = AUDIO_SAMPLE_RATE_EXACT * sec;
      _dts = (int)(samples < 0.0f ? 0.0f : samples > _max_delay ? _max_delay : samples);
    }

    void feedback(float feedback)
    {
      _feedback = feedback < -1.0f ? -1.0f : feedback > 1.0f ? 1.0f : feedback;
      _kcabdeef = -_feedback;
    }

    float read(float delay_samples) const
    {
      if (!_delay) return 0.0f;

      delay_samples = delay_samples < 1.0f ? 1.0f : delay_samples > _max_delay ? _max_delay : delay_samples;

      float i;
      float d = 1.0f - modff(delay_samples, &i);

      int i0 = _index - (int)i - 1;
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

      return (x0 * d0) + (x1 * d1) + (x2 * d2);
    }

    float readSample(int delay_samples = -1) const
    {
      if (delay_samples < 0) return _delay[_index];

      int index = _index - delay_samples - 1;
      while (index < 0) index += _max_index;

      return _delay[index];
    }

    void write(float sample)
    {
      _delay[_index] = sample;
      if (++_index == _max_index) _index = 0;
    }

    float delay(float input)
    {
      write(input);
      return readSample(_dts);
    }

    float allpass(float input)
    {
      float bufout = readSample(_dts);
      float bufin = (bufout * _kcabdeef) + input;
      write(bufin);
      return (bufin * _feedback) + bufout;
    }

    float allpass(float input, float delay_samples)
    {
      float bufout = read(delay_samples);
      float bufin = (bufout * _kcabdeef) + input;
      write(bufin);
      return (bufin * _feedback) + bufout;
    }

    inline void delayArray(float *pSrc, float *pDst, uint32_t len)
    {
      if (!pSrc || !pDst || len == 0) return;
      
      float *pSrcEnd = pSrc + len;
      while (pSrc < pSrcEnd)
      {
        write(*pSrc++);
        *pDst++ = readSample(_dts);
      }
    }

    void delayBlock(audio_block_f32_t *pSrc, audio_block_f32_t *pDst = nullptr)
    {
      if (pSrc == nullptr) return;
      if (pDst == nullptr) pDst = pSrc;

      delayArray(pSrc->data, pDst->data, pSrc->length);
    }

    float maxTime() const
    {
      return _max_delay / AUDIO_SAMPLE_RATE_EXACT;
    }

    static constexpr float msToSamples(float ms)
    {
      ms = ms < 0.0f ? 0.0f : ms;
      return AUDIO_SAMPLE_RATE_EXACT * 0.001f * ms;
    }

    static constexpr uint16_t bufferSizeMs(float ms)
    {
      ms = ms < 0.0f ? 0.0f : ms;
      return (uint16_t)(AUDIO_SAMPLE_RATE_EXACT * 0.001f * ms);
    }

  protected:
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    int _max_index = 0;
    float _max_delay = 0.0f;
    float *_delay = nullptr;
    int _index = 0;
    int _dts = 0;
    float _feedback = 0.0f;
    float _kcabdeef = 0.0f;
};


template <uint32_t N>
class RAM1Delay: public DelayBase
{
  public:
    RAM1Delay()
    {
      _delay = _stack_delay;
      _max_index = N + 4;
      _max_delay = (float)N;
      _dts = (int)N;
    }

    virtual ~RAM1Delay() {}
    RAM1Delay(RAM1Delay const&) = delete;
    RAM1Delay& operator=(RAM1Delay const&) = delete;

  private:
    __attribute__((aligned(8))) float _stack_delay[N + 4];
};


template <uint32_t N>
class RAM2Delay: public DelayBase
{
  public:
    RAM2Delay()
    {
      size_t max_index = N + 4;
      size_t size = max_index * sizeof(float);
      _delay = (float*)malloc(size);
      if (_delay)
      {
        memset(_delay, 0, size);
        _max_index = max_index;
        _max_delay = (float)N;
        _dts = (int)N;
      }
    }

    ~RAM2Delay()
    {
      free(_delay);
    }

    RAM2Delay(RAM2Delay const&) = delete;
    RAM2Delay& operator=(RAM2Delay const&) = delete;
};


template <uint32_t N>
class EXTMEMDelay: public DelayBase
{
  public:
    EXTMEMDelay()
    {
      size_t max_index = N + 4;
      size_t size = max_index * sizeof(float);
      _delay = (float*)extmem_malloc(size);
      if (_delay)
      {
        memset(_delay, 0, size);
        _max_index = max_index;
        _max_delay = (float)N;
        _dts = (int)N;
      }
    }

    ~EXTMEMDelay()
    {
      extmem_free(_delay);
    }

    EXTMEMDelay(EXTMEMDelay const&) = delete;
    EXTMEMDelay& operator=(EXTMEMDelay const&) = delete;
};


template <uint16_t N>
class DelayFilter
{
  public:
    DelayFilter()
    {
      uint32_t size = N * sizeof(float);
      _delay = (float*)malloc(size);
      if (_delay)
      {
        memset(_delay, 0, size);
      }
    }

    ~DelayFilter()
    {
      free(_delay);
    }

    DelayFilter(DelayFilter const&) = delete;
    DelayFilter& operator=(DelayFilter const&) = delete;

    void damping(float damping)
    {
      _damping = damping < 0.0f ? 0.0f : damping > 1.0f ? 1.0f : damping;
    }

    void feedback(float feedback)
    {
      _feedback = feedback < -1.0f ? -1.0f : feedback > 1.0f ? 1.0f : feedback;
      _kcabdeef = -_feedback;
    }

    // Dattorro
    float allpass(float input)
    {
      float bufout = _delay[_index];
      float bufin = _delay[_index] = (bufout * _kcabdeef) + input;
      if (++_index == N) _index = 0;
      return (bufin * _feedback) + bufout;
    }

    // Freeverb
    float comb(float input)
    {
      float output = _delay[_index];
      _state += (output - _state) * _damping;
      _delay[_index] = (_state * _feedback) + input;
      if (++_index == N) _index = 0;
      return output;
    }

    float delay(float input)
    {
      float output = _delay[_index];
      _delay[_index] = input;
      if (++_index == N) _index = 0;
      return output;
    }

    float readSample(int t)
    {
      t = t < 0 ? 0 : t > (N - 1) ? (N - 1) : t;
      int i = _index - t - 1;
      if (i < 0) i += N;
      return _delay[i];
    }

    float operator [](int t)
    {
      t = t < 0 ? 0 : t > (N - 1) ? (N - 1) : t;
      int i = _index - t - 1;
      if (i < 0) i += N;
      return _delay[i];
    }

  protected:
    float *_delay;
    float _feedback = 0.0f;
    float _kcabdeef = 0.0f;
    float _damping = 0.5f;
    float _state = 0.0f;
    int _index = 0;
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

    virtual ~Freeverb() {}
    Freeverb(Freeverb const&) = delete;
    Freeverb& operator=(Freeverb const&) = delete;

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

      float input, output;
      float *pSrcEnd = pSrc + len;
      while (pSrc < pSrcEnd)
      {
        input = *pSrc++;
        output = _cf1.comb(input);
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
