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

    void begin()
    {
      _f1.reset().setHighpass1p1z(25.0f).begin();
      _f2.reset().setLowpass1p1z(17500.0f).begin();
    }

    void enable(bool enable = true)
    {
      _enable = enable;
    }

    void level(float level = 0.5f)
    {
      _level = ((level < 0.0f ? 0.0f : level > 1.0f ? 1.0f : level) * 1.5f) + 0.25f;
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (_enable)
      {
        _f1.filterBlock(block);
        _f2.filterBlock(block);
        BlockOperations::scale(block, _level);
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    bool _enable = true;

    HQ1p1zBiquad _f1;
    CascadeBiquad<2> _f2;

    float _level = 1.0f;
};

#endif
