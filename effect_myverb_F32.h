#ifndef _effect_myverb_F32_h_
#define _effect_myverb_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectMyVerb_F32 :
  public AudioStream_F32
{
    //GUI: inputs:2, outputs:2  //this line used for automatic generation of GUI node
    //GUI: shortName:reverb
  public:
    AudioEffectMyVerb_F32(void):
      AudioStream_F32(2, inputQueueArray)
    {
      begin();
    }

    AudioEffectMyVerb_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(2, inputQueueArray),
      _fpreL(settings.sample_rate_Hz),
      _fpreR(settings.sample_rate_Hz)
    {
      begin();
    }

    void begin()
    {
      _fpreL.reset().setHighpass(300.0f).begin();
      _fpreR.reset().setHighpass(300.0f).begin();
      roomsize();
      damping();
    }

    void roomsize(float n = 0.5f)
    {
      _revL.roomsize(n);
      _revR.roomsize(n);
    }

    void damping(float n = 0.5f)
    {
      _revL.damping(n);
      _revR.damping(n);
    }

    void wet(float wet = 0.15f)
    {
      _wet = wet < -1.0f ? -1.0f : wet > 1.0f ? 1.0f : wet;
    }

    void dry(float dry = 1.0f)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }

    virtual void update(void)
    {
      audio_block_f32_t* blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t* blockR = AudioStream_F32::receiveWritable_f32(1);
      audio_block_f32_t* blockWetL = AudioStream_F32::allocate_f32();
      audio_block_f32_t* blockWetR = AudioStream_F32::allocate_f32();
      if (!blockL || !blockR || !blockWetL || !blockWetR)
      {
        if (blockWetR) AudioStream_F32::release(blockWetR);
        if (blockWetL) AudioStream_F32::release(blockWetL);
        if (blockR) AudioStream_F32::release(blockR);
        if (blockL) AudioStream_F32::release(blockL);
        return;
      }

      _fpreL.filterBlock(blockL, blockWetL);
      _fpreR.filterBlock(blockR, blockWetR);

      BlockOperations::scale(blockWetL, _wet);
      BlockOperations::scale(blockWetR, _wet);

      _revL.processBlock(blockWetL);
      _revR.processBlock(blockWetR);

      if (_enable)
      {
        BlockOperations::scale(blockL, _dry);
        BlockOperations::scale(blockR, _dry);

        BlockOperations::add(blockWetL, blockL);
        BlockOperations::add(blockWetR, blockR);
      }

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockWetR);
      AudioStream_F32::release(blockWetL);
      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
    }

  private:
    audio_block_f32_t *inputQueueArray[2];

    CascadeBiquad<1> _fpreL;
    CascadeBiquad<1> _fpreR;
    FreeverbL _revL;
    FreeverbR _revR;

    float _wet = 0.15f;
    float _dry = 1.0f;
    bool _enable = false;
};

#endif
