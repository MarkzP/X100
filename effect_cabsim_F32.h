#ifndef _effect_cabsim_F32_h_
#define _effect_cabsim_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectCabSim_F32 :
  public AudioStream_F32
{
    //GUI: inputs:2, outputs:2  //this line used for automatic generation of GUI node
    //GUI: shortName:delay
  public:
    AudioEffectCabSim_F32(void):
      AudioStream_F32(2, inputQueueArray)
    {
    }

    AudioEffectCabSim_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(2, inputQueueArray),
      _fpreL(settings.sample_rate_Hz),
      _fpreR(settings.sample_rate_Hz),
      _frevL(settings.sample_rate_Hz),
      _frevR(settings.sample_rate_Hz),
      _fpostL(settings.sample_rate_Hz),
      _fpostR(settings.sample_rate_Hz)
    {
    }

    void begin()
    {
      _fpreL.reset()
      .setHighpass(280.0f)
      .setPeak(1300.0f, -14.0f)
      .setLowpass(3500.0f, 1.4f)
      .setLowpass(14000.0f)
      .begin();

      _fpreR.reset()
      .setHighpass(280.0f)
      .setPeak(1300.0f, -14.0f)
      .setLowpass(3500.0f, 1.4f)
      .setLowpass(14000.0f)
      .begin();

      _frevL.reset().setHighpass1p1z(400.0f).begin();
      _frevR.reset().setHighpass1p1z(400.0f).begin();

      _fpostL.reset().setLowpass(10000.0f).setLowpass(10000.0f).begin();
      _fpostR.reset().setLowpass(10000.0f).setLowpass(10000.0f).begin();

      size();
      direct();
      room();
      enable();
    }

    void size(float size = 0.5f)
    {
      size *= 0.3f;
      size += 0.1f;
      _revL.size(size); _revR.size(size);
      _revL.damping(0.8f); _revR.damping(0.8f);
    }

    void direct(float direct = 1.0f)
    {
      _direct = (direct < 0.0f ? 0.0f : direct > 1.0f ? 1.0f : direct) * 2.0f;
    }

    void room(float room = 0.5f)
    {
      _room = (room < 0.0f ? 0.0f : room > 1.0f ? 1.0f : room) * 0.25f;
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }

    virtual void update(void)
    {
      audio_block_f32_t* blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t* blockR = AudioStream_F32::receiveWritable_f32(1);
      audio_block_f32_t* roomL = AudioStream_F32::allocate_f32();
      audio_block_f32_t* roomR = AudioStream_F32::allocate_f32();
      if (!blockL || !blockR || !roomL || !roomR)
      {
        if (roomR) AudioStream_F32::release(roomR);
        if (roomL) AudioStream_F32::release(roomL);
        if (blockR) AudioStream_F32::release(blockR);
        if (blockL) AudioStream_F32::release(blockL);
        return;
      }

      if (_enable)
      {
        _fpreL.filterBlock(blockL);
        _fpreR.filterBlock(blockR);

        _frevL.filterBlock(blockL, roomL);
        _frevR.filterBlock(blockR, roomR);

        BlockOperations::scale(blockL, _direct);
        BlockOperations::scale(blockR, _direct);

        BlockOperations::scale(roomL, _room);
        BlockOperations::scale(roomR, _room);

        _delayL.delayBlock(roomL);
        _delayR.delayBlock(roomR);

        _revL.processBlock(roomL);
        _revR.processBlock(roomR);

        BlockOperations::add(roomL, blockL);
        BlockOperations::add(roomR, blockR);

        _fpostL.filterBlock(blockL);
        _fpostR.filterBlock(blockR);
      }

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(roomR);
      AudioStream_F32::release(roomL);
      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
    }

  private:
    audio_block_f32_t *inputQueueArray[2];
    CascadeBiquad<4> _fpreL;
    CascadeBiquad<4> _fpreR;
    CascadeBiquad<1> _frevL;
    CascadeBiquad<1> _frevR;
    CascadeBiquad<2> _fpostL;
    CascadeBiquad<2> _fpostR;
    FreeverbL _revL;
    FreeverbR _revR;
    StaticDelay<346> _delayL;
    StaticDelay<291> _delayR;

    bool _enable = false;
    float _direct = 1.0f;
    float _room = 1.0f;
};

#endif
