#ifndef _effect_preamp_F32_h_
#define _effect_preamp_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectPreamp_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:preamp
  public:
    AudioEffectPreamp_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioEffectPreamp_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _f1(settings.sample_rate_Hz),
      _f2(settings.sample_rate_Hz)
    {
    }

    FLASHMEM void begin()
    {
      _f1.setHighpassFirstOrder(20.0);
      _f2.setLowpassFirstOrder(17500.0);
    }

    FLASHMEM void enable(bool enable = true)
    {
      _enable = enable;
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (_enable)
      {
        _f1.filterBlock(block);
        _f2.filterBlock(block);
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    bool _enable = true;

    HQFirstOrder _f1;
    HQFirstOrder _f2;
};

#endif
