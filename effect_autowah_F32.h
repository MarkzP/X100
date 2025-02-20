#ifndef _effect_autowah_F32_h_
#define _effect_autowah_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectAutoWah_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:filter
  public:
    AudioEffectAutoWah_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
      _svf.resonance(3.0f);
      _svf.frequency(250.0f);
    }

    AudioEffectAutoWah_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray), _svf(settings.sample_rate_Hz)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
      _svf.resonance(3.0f);
      _svf.frequency(250.0f);
    }

    FLASHMEM void enable(bool enable = false)
    {
      _enable = enable;
    }

    FLASHMEM void sensitivity(float sensitivity = 0.5f)
    {
      sensitivity = sensitivity < 0.0f ? 0.0f : sensitivity > 1.0f ? 1.0f : sensitivity;
      _sensitivity = ((sensitivity * 15.0f) + 5.0f) * _octave;
    }

    FLASHMEM void speed(float speed = 0.5f)
    {
      speed = speed < 0.0f ? 0.0f : speed > 1.0f ? 1.0f : speed;
      float s = ((1.0f - speed) * 1.8f) + 0.2f;
      _smooth = 1.0f - expf(-3.1699f / (_sample_rate_Hz * s));
    }

    FLASHMEM void wet(float wet)
    {
      _wet = wet < 0.0f ? 0.0f : wet > 1.0f ? 1.0f : wet;
    }

    FLASHMEM void dry(float dry)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    virtual void update(void)
    {
      float control = _control;
      float sensitivity = _sensitivity;
      float smooth = _smooth;

      audio_block_f32_t* block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (_enable)
      {
        _svf.mix(_dry, 0.25f * _wet, 0.55f * _wet, 0.0f);

        for (uint16_t i = 0; i < block->length; i++)
        {
          float sample = block->data[i];
          float level = _d.detect(sample) * sensitivity;
          control += (level - control) * (level > control ? smooth : _decay);
          control = control < -_octave ? -_octave : control > _octave ? _octave : control;
          _svf.control(control);
          sample = _svf.filter(sample);

          block->data[i] = sample;
        }
      }

      _control = control;

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[2];
    float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    static constexpr float _octave = 3.0f;
    static constexpr float _decay = 0.005f;

    Detector _d;
    StateVariableFilter<4> _svf;

    bool _enable = false;
    float _control = 0.0f;
    float _sensitivity = 30.0f;
    float _smooth = 0.00035f;
    float _dry = 0.0f;
    float _wet = 1.0f;
};

#endif
