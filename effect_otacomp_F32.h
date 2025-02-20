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
      begin();
    }

    AudioEffectOtaComp_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _sample_rate_Hz(settings.sample_rate_Hz),
      _preFilter(settings.sample_rate_Hz),
      _postFilter(settings.sample_rate_Hz)
    {
      begin();
    }

    void begin()
    {
      _attack = 1.0f - expf(-3.1699f / (_sample_rate_Hz * 0.002f));
      _release = 1.0f - expf(-3.1699f / (_sample_rate_Hz * 1.5f));
      _preFilter.setHighShelf(2000.0, 12.0).setLowpass1p1z(12000.0).begin();
      _postFilter.setHighpass1p1z(50.0).setHighShelf(2000.0, -9.0).setLowpass1p1z(10000.0).begin();
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }
    
    void output(float output)
    {
      _output = output < 0.0f ? 0.0f : output > 1.0f ? 1.0f : output;
      //_output = powf(output, 1.5f);
    }

    void sensitivity(float sensitivity)
    {
      _sensitivity = sensitivity < 0.0f ? 0.0f : sensitivity > 1.0f ? 1.0f : sensitivity;
    }

    void attack(float attack = 0.0f)
    {
      attack = attack < 0.0f ? 0.0f : attack > 1.0f ? 1.0f : attack;
      float t = (attack * 0.098f) + 0.002f;
      _attack = 1.0f - expf(-3.1699f / (_sample_rate_Hz * t));
    }

    float gain() { return _gain; }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (_enable)
      {
        _dcFilter.filterBlock(block);
        _preFilter.filterBlock(block);

        for (uint16_t i = 0; i < block->length; i++)
        {
          // OTA
          float sample = block->data[i] * _gain;
          block->data[i] = sample * _output;

          // Diodes
          sample = fabsf(sample);
          sample -= _vdrop;
          sample = sample < 0.0f ? 0.0f : sample;

          // Transistors
          sample *= _qbeta;
          sample = _maxgain - (sample > _maxgain ? _maxgain : sample);

          // Envelope
          float gain = _mingain + (sample * _sensitivity);
          _gain += (gain - _gain) * (gain < _gain ? _attack : _release);
        }

        _postFilter.filterBlock(block);
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;

    bool _enable = false;
    float _output = 0.5f;
    float _sensitivity = 0.5f;

    DcBlock _dcFilter;
    CascadeBiquad<2> _preFilter;
    CascadeBiquad<3> _postFilter;

    static constexpr float _vdrop = 0.25f;
    static constexpr float _qbeta = 50.0f;

    static constexpr float _maxgain = 30.0f;
    static constexpr float _mingain = 1.0f;

    float _release;
    float _attack;
    
    float _gain = _mingain;
    float _fgain = _mingain;
};

#endif
