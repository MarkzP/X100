#ifndef _effect_myverb_F32_h_
#define _effect_myverb_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

constexpr float src(int samples) { return (float)samples * AUDIO_SAMPLE_RATE_EXACT / 29761.0f; }

class AudioEffectMyVerb_F32 : public AudioStream_F32 {
  //GUI: inputs:2, outputs:2  //this line used for automatic generation of GUI node
  //GUI: shortName:reverb
public:
  AudioEffectMyVerb_F32(void)
    : AudioStream_F32(2, inputQueueArray) {
  }

  AudioEffectMyVerb_F32(const AudioSettings_F32& settings)
    : AudioStream_F32(2, inputQueueArray) {
  }

  void begin() {
    
    pinMode(0, INPUT_PULLUP);

    _lfoL.freq(1.161f);
    _lfoR.freq(0.813f);

    _node13_14.feedback(0.750f);
    _node19_20.feedback(0.750f);
    _node15_16.feedback(0.625f);
    _node21_22.feedback(0.625f);

    delay();
    bandwidth();
    density();
    decay();
    damping();
    sensitivity();
    wet();
    dry();
    enable();
  }

  void input(float input = 1.0f) {
    _input = (input < 0.0f ? 0.0f : input > 1.0f ? 1.0f : input) * 0.5f;
  }

  void density(float density1 = 0.7f) {
    density1 = density1 < 0.0f ? 0.0f : density1 > 1.0f ? 1.0f : density1;
    density1 = density1 * 0.7995f + 0.005f;

    _node23_24.feedback(-density1);
    _node46_48.feedback(-density1);
  }

  void delay(float delay = 0.5f) {
    delay = delay < 0.0f ? 0.0f : delay > 1.0f ? 1.0f : delay;
    
    _predelay.time(_predelay.maxTime() * delay);
  }

  void decay(float decay = 0.3f) {
    decay = decay < 0.0f ? 0.0f : decay > 1.0f ? 1.0f : decay;

    float density2 = decay + 0.15f;
    density2 = density2 < 0.25f ? 0.25f : density2 > 0.5f ? 0.5f : density2;
    
    _decay = decay;
    _node31_33.feedback(density2);
    _node55_59.feedback(density2);
  }

  void bandwidth(float bandwidth = 0.8f) {
    bandwidth = bandwidth < 0.0f ? 0.0f : bandwidth > 1.0f ? 1.0f : bandwidth;
    bandwidth = bandwidth * 0.994f + 0.005f;
    
    _bandwidth.coefficient(bandwidth);
  }

  void damping(float damping = 0.3f) {
    damping = damping < 0.0f ? 0.0f : damping > 1.0f ? 1.0f : damping;
    damping = (1.0f - damping) * 0.994f + 0.005f;
    
    _node30.coefficient(damping);
    _node54.coefficient(damping);
  }

  void sensitivity(float sensitivity = 0.0f)
  {
    sensitivity = sensitivity < 0.0f ? 0.0f : sensitivity > 1.0f ? 1.0f : sensitivity;
    _sensitivity = sensitivity * 50.0f;
  }

  void wet(float wet = 0.2f) {
    _wet = (wet < -1.0f ? -1.0f : wet > 1.0f ? 1.0f : wet) * 0.6f;
  }

  void dry(float dry = 1.0f) {
    _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
  }

  void enable(bool enable = false) {
    _enable = enable;
  }

  virtual void update(void) {

    audio_block_f32_t* blockL = AudioStream_F32::receiveWritable_f32(0);
    audio_block_f32_t* blockR = AudioStream_F32::receiveWritable_f32(1);

    if (!blockL || !blockR) {
      if (blockR) AudioStream_F32::release(blockR);
      if (blockL) AudioStream_F32::release(blockL);
      return;
    }

    float tankL = _tankL;
    float tankR = _tankR;

    for (uint16_t i = 0; i < blockL->length; i++) {

      float signal = (blockL->data[i] + blockR->data[i]) * (digitalReadFast(0) ? _input : 0.5f);

      float level = _d.detect(signal) * _sensitivity;
      level = level < 0.0f ? 0.0f : level > 1.0f ? 1.0f : level;
      _fin += (level - _fin) * 0.01f;
      float decay = _decay * (1.0f - _fin);

      signal = _bandwidth.filter(signal);
      signal = _predelay.delay(signal);

      signal = _node13_14.allpass(signal);
      signal = _node19_20.allpass(signal);
      signal = _node15_16.allpass(signal);
      signal = _node21_22.allpass(signal);

      float modL = _lfoL.next() * _excursionL;
      tankL *= decay;
      tankL = _node23_24.allpass(signal + tankR, modL);
      tankL = _node24_30.delay(tankL);
      tankL = _node30.filter(tankL);
      tankL *= decay;
      tankL = _node31_33.allpass(tankL);
      tankL = _node33_39.delay(tankL);

      float modR = _lfoR.next() * _excursionR;
      tankR *= decay;
      tankR = _node46_48.allpass(signal + tankL, modR);
      tankR = _node48_54.delay(tankR);
      tankR = _node54.filter(tankR);
      tankR *= decay;
      tankR = _node55_59.allpass(tankR);
      tankR = _node59_63.delay(tankR);

      if (_enable) {

        float yL = ((_tap1l * _node48_54.readSample(_yl_t1))
                  + (_tap2l * _node48_54.readSample(_yl_t2))
                  - (_tap3l * _node55_59.readSample(_yl_t3))
                  + (_tap4l * _node59_63.readSample(_yl_t4))
                  - (_tap5l * _node24_30.readSample(_yl_t5))
                  - (_tap6l * _node31_33.readSample(_yl_t6))
                  - (_tap7l * _node33_39.readSample(_yl_t7)));

        float yR = ((_tap1r * _node24_30.readSample(_yr_t1))
                  + (_tap2r * _node24_30.readSample(_yr_t2))
                  - (_tap3r * _node31_33.readSample(_yr_t3))
                  + (_tap4r * _node33_39.readSample(_yr_t4))
                  - (_tap5r * _node48_54.readSample(_yr_t5))
                  - (_tap6r * _node55_59.readSample(_yr_t6))
                  - (_tap7r * _node59_63.readSample(_yr_t7)));                  

        blockL->data[i] *= _dry;
        blockR->data[i] *= _dry;

        blockL->data[i] += yL * _wet;
        blockR->data[i] += yR * _wet;
      }
    }

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

  static constexpr float _excursionL = src(17);
  static constexpr float _node23_24_s = src(672);
  static constexpr float _node24_30_s = src(4453);
  static constexpr float _node31_33_s = src(1800);
  static constexpr float _node33_39_s = src(3720);
  static constexpr float _excursionR = src(11);
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

  audio_block_f32_t* inputQueueArray[2];

  TriangleLfo _lfoL;
  TriangleLfo _lfoR;

  LPFirstOrder _bandwidth;

  StaticDelay< Delay::bufferSize(0.100f) > _predelay;

  DelayFilter< (uint16_t)_node13_14_s >       _node13_14;
  DelayFilter< (uint16_t)_node19_20_s >       _node19_20;
  DelayFilter< (uint16_t)_node15_16_s >       _node15_16;
  DelayFilter< (uint16_t)_node21_22_s >       _node21_22;

  StaticDelay< (uint16_t)(_node23_24_s + (_excursionL * 2)) >  _node23_24;
  DelayFilter< (uint16_t)_node24_30_s >      _node24_30;
  LPFirstOrder _node30;
  DelayFilter< (uint16_t)_node31_33_s >      _node31_33;
  DelayFilter< (uint16_t)_node33_39_s >      _node33_39;
  
  StaticDelay< (uint16_t)(_node46_48_s + (_excursionR * 2)) >  _node46_48;
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

  float _tap1l = 1.0f;
  float _tap2l = 1.0f;
  float _tap3l = 1.0f;
  float _tap4l = 1.0f;
  float _tap5l = 1.0f;
  float _tap6l = 1.0f;
  float _tap7l = 1.0f;

  float _tap1r = 1.0f;
  float _tap2r = 1.0f;
  float _tap3r = 1.0f;
  float _tap4r = 1.0f;
  float _tap5r = 1.0f;
  float _tap6r = 1.0f;
  float _tap7r = 1.0f;

  float _tankL = 0.0f;
  float _tankR = 0.0f;

  float _decay = 0.0f;
  float _wet = 0.0f;
  float _dry = 1.0f;

  Detector _d;
  float _input = 0.5f;
  float _sensitivity = 20.0f;
  float _fin = 0.0f;

  bool _enable = false;
};


#endif
