#ifndef _effect_ota_comp_F32_h_
#define _effect_ota_comp_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectOtaComp_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:otacomp
  public:
    AudioEffectOtaComp_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioEffectOtaComp_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _sample_rate_Hz(settings.sample_rate_Hz),
      _dcCoupling(settings.sample_rate_Hz),
      _preFilter1(settings.sample_rate_Hz),
      _preFilter2(settings.sample_rate_Hz),
      _postFilter1(settings.sample_rate_Hz),
      _postFilter2(settings.sample_rate_Hz)
    {
    }

    void begin()
    {
      enable();
      output();
      sensitivity();
      attack();
      release();

      _dcCoupling.setHighpassFirstOrder(20);
      _preFilter1.setHighShelf(2000.0, 12.0);
      _preFilter2.setLowpassFirstOrder(12000.0);
      _postFilter1.setHighShelf(2000.0, -9.0);
      _postFilter2.setLowpassFirstOrder(10000.0);
    }

    FLASHMEM void enable(bool enable = false)
    {
      _enable = enable;
    }
    
    FLASHMEM void output(float output = 0.5f)
    {
      _output = output < 0.0f ? 0.0f : output > 1.0f ? 1.0f : output;
    }

    FLASHMEM void sensitivity(float sensitivity = 0.5f)
    {
      _sensitivity = sensitivity < 0.0f ? 0.0f : sensitivity > 1.0f ? 1.0f : sensitivity;
    }

    FLASHMEM void attack(float attack = 0.0f)
    {
      attack = attack < 0.0f ? 0.0f : attack > 1.0f ? 1.0f : attack;
      // 2ms -> 100ms
      float t = (attack * 0.098f) + 0.002f;
      _attack = 1.0f - expf(-3.1699f / (_sample_rate_Hz * t));
    }

    FLASHMEM void release(float release = 0.5f)
    {
      release = release < 0.0f ? 0.0f : release > 1.0f ? 1.0f : release;
      // 1000ms -> 2000ms
      float t = (release * 1.000f) + 1.000f;
      _release = 1.0f - expf(-3.1699f / (_sample_rate_Hz * t));
    }

    FLASHMEM float gain() { return _gain; }

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

      _dcCoupling.filterBlock(block);
      _preFilter1.filterBlock(block);
      _preFilter2.filterBlock(block);

      float output = _output;
      float sensitivity = _sensitivity;
      float attack = _attack;
      float release = _release;
      float gain = _gain;

      float sample, envelope;

      float *p = block->data;
      float *end = p + block->length;
      do
      {
        // OTA
        sample = *p;
        sample *= gain;
        *p++ = sample * output;

        // Diodes
        sample = fabsf(sample);
        sample -= _vdrop;
        sample = sample < 0.0f ? 0.0f : sample;

        // Transistors
        sample *= _qbeta;
        sample = _maxgain - (sample > _maxgain ? _maxgain : sample);

        // Envelope
        envelope = (sample * sensitivity) + _mingain;
        gain += (envelope - gain) * (envelope < gain ? attack : release);
      }
      while (p < end);

      _gain = gain;

      _postFilter1.filterBlock(block);
      _postFilter2.filterBlock(block);

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;

    bool _enable = false;
    float _output = 0.5f;
    float _sensitivity = 0.5f;

    HQFirstOrder _dcCoupling;
    Biquad _preFilter1;
    FirstOrder _preFilter2;
    Biquad _postFilter1;
    FirstOrder _postFilter2;

    static constexpr float _vdrop = 0.25f;
    static constexpr float _qbeta = 50.0f;

    static constexpr float _maxgain = 30.0f;
    static constexpr float _mingain = 1.0f;

    float _release;
    float _attack;
    
    float _gain = _mingain;
};

#endif
