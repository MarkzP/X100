#ifndef _effect_myverb_F32_h_
#define _effect_myverb_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

constexpr float src(int samples) { return (float)samples * AUDIO_SAMPLE_RATE_EXACT / 29761.0f; }
constexpr float ratio = 0.7f;

class AudioEffectMyVerb_F32 : public AudioStream_F32
{
  //GUI: inputs:2, outputs:2  //this line used for automatic generation of GUI node
  //GUI: shortName:reverb
public:
  AudioEffectMyVerb_F32(void)
    : AudioStream_F32(2, inputQueueArray)
  {
  }

  AudioEffectMyVerb_F32(const AudioSettings_F32& settings)
    : AudioStream_F32(2, inputQueueArray)
  {
  }

  FLASHMEM void begin()
  {
    _lfoL.freq(1.161f);
    _lfoR.freq(0.813f);

    _node13_14.feedback(0.750f * ratio);
    _node19_20.feedback(0.750f * ratio);
    _node15_16.feedback(0.625f * ratio);
    _node21_22.feedback(0.625f * ratio);

    delay();
    highpass();
    lowpass();
    density();
    decay();
    damping();
    wet();
    dry();
    enable();
  }

  FLASHMEM void density(float density1 = 0.7f)
  {
    density1 = density1 < 0.0f ? 0.0f : density1 > 1.0f ? 1.0f : density1;
    density1 = density1 * 0.7995f + 0.005f;

    _node23_24.feedback(-density1);
    _node46_48.feedback(-density1);
  }

  FLASHMEM void delay(float delay = 0.5f)
  {
    delay = delay < 0.0f ? 0.0f : delay > 1.0f ? 1.0f : delay;
    
    _predelay.time(_predelay.maxTime() * delay);
  }

  FLASHMEM void decay(float decay = 0.3f)
  {
    decay = decay < 0.0f ? 0.0f : decay > 1.0f ? 1.0f : decay;
    //decay = powf(decay, 2.0f);

    float density2 = decay + 0.15f;
    density2 = density2 < 0.25f ? 0.25f : density2 > 0.5f ? 0.5f : density2;
    
    _decay = decay;
    _decay2 = powf(decay, 0.25f);
    _node31_33.feedback(density2);
    _node55_59.feedback(density2);
  }

  FLASHMEM void highpass(float freq = 100.0f)
  {
    _preFilterHp.setHighpassFirstOrder(freq);
  }

  FLASHMEM void lowpass(float freq = 5600.0f)
  {
    _preFilterLp.setLowpassFirstOrder(freq);
  }

  FLASHMEM void damping(float damping = 0.3f)
  {
    damping = damping < 0.0f ? 0.0f : damping > 1.0f ? 1.0f : damping;
    //damping = powf(damping, 0.25f);
    damping = (1.0f - damping) * 0.984f + 0.015f;
    
    _node30.coefficient(damping);
    _node54.coefficient(damping);
  }

  FLASHMEM void wet(float wet = 0.2f)
  {
    _wet = wet < 0.0f ? 0.0f : wet > 1.0f ? 1.0f : wet;
  }

  FLASHMEM void dry(float dry = 1.0f)
  {
    _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
  }

  FLASHMEM void enable(bool enable = false)
  {
    _enable = enable;
  }

  FLASHMEM void bypass(bool bypass = false)
  {
    _bypass = bypass;
  }

  virtual void update(void)
  {
    if (_bypass)
    {
      audio_block_f32_t *bpL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t *bpR = AudioStream_F32::receiveWritable_f32(1);

      if (!bpL || !bpR)
      {
        if (bpR) AudioStream_F32::release(bpR);
        if (bpL) AudioStream_F32::release(bpL);
        return;
      }

      AudioStream_F32::transmit(bpL, 0);
      AudioStream_F32::transmit(bpR, 1);
      AudioStream_F32::release(bpR);
      AudioStream_F32::release(bpL);
      return;
    }


    audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
    audio_block_f32_t *blockR = AudioStream_F32::receiveWritable_f32(1);

    if (!blockL || !blockR)
    {
      if (blockR) AudioStream_F32::release(blockR);
      if (blockL) AudioStream_F32::release(blockL);
      return;
    }

    bool enable = _enable;
    float input = enable ? 0.5f : 0.0f;
    float dry = enable ? _dry : 1.0f;
    float decay = enable ? _decay : _decay * 0.5f;
    float decay2 = _decay2;
    float wet = _wet * (1.0f - (_dry * 0.4f));
    float tap1 = _tap1;
    float tap2 = _tap2;
    float tap3 = _tap3;
    float tap4 = _tap4;
    float tap5 = _tap5;
    float tap6 = _tap6;
    float tap7 = _tap7;
    float tankL = _tankL;
    float tankR = _tankR;

    float dryL, dryR, signal, modL, modR, yL, yR;

    float *pl = blockL->data;
    float *pr = blockR->data;
    float *endl = pl + blockL->length;
    do
    {
      dryL = *pl;
      dryR = *pr;

      signal = (dryL + dryR) * input;
      signal = _preFilterHp.filter(signal);
      signal = _preFilterLp.filter(signal);
      signal = _predelay.delay(signal);

      signal = _node13_14.allpass(signal);
      signal = _node19_20.allpass(signal);
      signal = _node15_16.allpass(signal);
      signal = _node21_22.allpass(signal);

      modL = (_lfoL.next() * _excursionL) + _node23_24_s;
      tankL = (tankR * decay) + signal;
      tankL = _node23_24.allpass(tankL, modL);
      tankL = _node24_30.delay(tankL);
      tankL = _node30.filter(tankL);
      tankL *= decay2;
      tankL = _node31_33.allpass(tankL);
      tankL = _node33_39.delay(tankL);

      modR = (_lfoR.next() * _excursionR) + _node46_48_s;
      tankR = (tankL * decay) + signal;
      tankR = _node46_48.allpass(tankR, modR);
      tankR = _node48_54.delay(tankR);
      tankR = _node54.filter(tankR);
      tankR *= decay2;
      tankR = _node55_59.allpass(tankR);
      tankR = _node59_63.delay(tankR);

      yL =  _node48_54.readSample(_yl_t1) * tap1;
      yL += _node48_54.readSample(_yl_t2) * tap2;
      yL += _node55_59.readSample(_yl_t3) * tap3;
      yL += _node59_63.readSample(_yl_t4) * tap4;
      yL += _node24_30.readSample(_yl_t5) * tap5;
      yL += _node31_33.readSample(_yl_t6) * tap6;
      yL += _node33_39.readSample(_yl_t7) * tap7;

      yR =  _node24_30.readSample(_yr_t1) * tap1;
      yR += _node24_30.readSample(_yr_t2) * tap2;
      yR += _node31_33.readSample(_yr_t3) * tap3;
      yR += _node33_39.readSample(_yr_t4) * tap4;
      yR += _node48_54.readSample(_yr_t5) * tap5;
      yR += _node55_59.readSample(_yr_t6) * tap6;
      yR += _node59_63.readSample(_yr_t7) * tap7;

      *pl++ = (dryL * dry) + (yL * wet);
      *pr++ = (dryR * dry) + (yR * wet);
    }
    while (pl < endl);

    _tankL = tankL;
    _tankR = tankR;

    AudioStream_F32::transmit(blockL, 0);
    AudioStream_F32::transmit(blockR, 1);
    AudioStream_F32::release(blockR);
    AudioStream_F32::release(blockL);
  }

private:
  static constexpr float _node13_14_s = src(142);
  static constexpr float _node19_20_s = src(107);
  static constexpr float _node15_16_s = src(379);
  static constexpr float _node21_22_s = src(277);

  static constexpr float _excursionL = src(7);
  static constexpr float _node23_24_s = src(672);
  static constexpr float _node24_30_s = src(4453);
  static constexpr float _node31_33_s = src(1800);
  static constexpr float _node33_39_s = src(3720);
  static constexpr float _excursionR = src(8);
  static constexpr float _node46_48_s = src(908);
  static constexpr float _node48_54_s = src(4217);
  static constexpr float _node55_59_s = src(2656);
  static constexpr float _node59_63_s = src(3163);

  static constexpr float _yl_t1_m = src(266);
  static constexpr float _yl_t2_m = src(2974);
  static constexpr float _yl_t3_m = src(1913);
  static constexpr float _yl_t4_m = src(1996);
  static constexpr float _yl_t5_m = src(1990);
  static constexpr float _yl_t6_m = src(187);
  static constexpr float _yl_t7_m = src(1066);
  static constexpr float _yr_t1_m = src(353);
  static constexpr float _yr_t2_m = src(3627);
  static constexpr float _yr_t3_m = src(1228);
  static constexpr float _yr_t4_m = src(2673);
  static constexpr float _yr_t5_m = src(2111);
  static constexpr float _yr_t6_m = src(335);
  static constexpr float _yr_t7_m = src(121);

  audio_block_f32_t *inputQueueArray[2];

  SineLfo _lfoL;
  SineLfo _lfoR;

  FirstOrder _preFilterHp;
  FirstOrder _preFilterLp;

  RAM2Delay< DelayBase::bufferSizeMs(100.0f) > _predelay;

  DelayFilter< (uint16_t)_node13_14_s >       _node13_14;
  DelayFilter< (uint16_t)_node19_20_s >       _node19_20;
  DelayFilter< (uint16_t)_node15_16_s >       _node15_16;
  DelayFilter< (uint16_t)_node21_22_s >       _node21_22;

  RAM2Delay< (uint16_t)(_node23_24_s + _excursionL + 1) >  _node23_24;
  DelayFilter< (uint16_t)_node24_30_s >      _node24_30;
  LPFirstOrder _node30;
  DelayFilter< (uint16_t)_node31_33_s >      _node31_33;
  DelayFilter< (uint16_t)_node33_39_s >      _node33_39;
  
  RAM2Delay< (uint16_t)(_node46_48_s + _excursionR + 1) >  _node46_48;
  DelayFilter< (uint16_t)_node48_54_s >      _node48_54;
  LPFirstOrder _node54;
  DelayFilter< (uint16_t)_node55_59_s >      _node55_59;
  DelayFilter< (uint16_t)_node59_63_s >      _node59_63;

  int _yl_t1 = _yl_t1_m;
  int _yl_t2 = _yl_t2_m;
  int _yl_t3 = _yl_t3_m;
  int _yl_t4 = _yl_t4_m;
  int _yl_t5 = _yl_t5_m;
  int _yl_t6 = _yl_t6_m;
  int _yl_t7 = _yl_t7_m;

  int _yr_t1 = _yr_t1_m;
  int _yr_t2 = _yr_t2_m;
  int _yr_t3 = _yr_t3_m;
  int _yr_t4 = _yr_t4_m;
  int _yr_t5 = _yr_t5_m;
  int _yr_t6 = _yr_t6_m;
  int _yr_t7 = _yr_t7_m;

  float _tap1 = 1.0f;
  float _tap2 = 1.0f;
  float _tap3 = -1.0f;
  float _tap4 = 1.0f;
  float _tap5 = -1.0f;
  float _tap6 = -1.0f;
  float _tap7 = -1.0f;

  float _tankL = 0.0f;
  float _tankR = 0.0f;

  float _decay = 0.0f;
  float _decay2 = 0.9f;
  float _wet = 0.0f;
  float _dry = 1.0f;

  bool _enable = false;
  bool _bypass = false;
};


#endif
