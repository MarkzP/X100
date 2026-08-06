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
      _roomL(settings),
      _roomR(settings),
      _fpre1L(settings.sample_rate_Hz),
      _fpre2L(settings.sample_rate_Hz),
      _fpre3L(settings.sample_rate_Hz),
      _fpre4L(settings.sample_rate_Hz),
      _fpre5L(settings.sample_rate_Hz),
      _fpre6L(settings.sample_rate_Hz),
      _fpre1R(settings.sample_rate_Hz),
      _fpre2R(settings.sample_rate_Hz),
      _fpre3R(settings.sample_rate_Hz),
      _fpre4R(settings.sample_rate_Hz),
      _fpre5R(settings.sample_rate_Hz),
      _fpre6R(settings.sample_rate_Hz),
      _frevL(settings.sample_rate_Hz),
      _frevR(settings.sample_rate_Hz),
      _fpost1L(settings.sample_rate_Hz),
      _fpost2L(settings.sample_rate_Hz),
      _fpost1R(settings.sample_rate_Hz),
      _fpost2R(settings.sample_rate_Hz)
    {
    }

    void begin()
    {
      _fpre1L.setHighpassFirstOrder(280.0);
      _fpre2L.setPeak(1300.0, -14.0);
      _fpre3L.setLowpass(3500.0, 1.4);
      _fpre4L.setLowpass(14000.0);

      _fpre1R.setHighpassFirstOrder(280.0);
      _fpre2R.setPeak(1300.0, -14.0);
      _fpre3R.setLowpass(3500.0, 1.4);
      _fpre4R.setLowpass(14000.0);

      _frevL.setHighpassFirstOrder(400.0);
      _frevR.setHighpassFirstOrder(400.0);

      _fpost1L.setLowpass(10000.0);
      _fpost2L.setLowpass(10000.0);
      _fpost1R.setLowpass(10000.0);
      _fpost2R.setLowpass(10000.0);

      size();
      direct();
      room();
      notch();
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

    void notch(float q = 0.0f, float low = 0.0f, float high = 0.0f)
    {
      q = q < 0.5f ? 0.5f : q > 5.0f ? 5.0f : q;

      if (low < 550.0f || low > 2500.0f)
      {
        _fpre5L.setBypass();
        _fpre5R.setBypass();
      }
      else
      {
        _fpre5L.setNotch(low, q);
        _fpre5R.setNotch(low, q);
      }

      if (high < 900.0f || high > 7000.0f)
      {
        _fpre6L.setBypass();
        _fpre6R.setBypass();
      }
      else
      {
        _fpre6L.setNotch(high, q);
        _fpre6R.setNotch(high, q);
      }
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }

    virtual void update(void)
    {
      audio_block_f32_t *roomL = &_roomL;
      audio_block_f32_t *roomR = &_roomR;

      audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t *blockR = AudioStream_F32::receiveWritable_f32(1);
      if (!blockL || !blockR)
      {
        if (blockR) AudioStream_F32::release(blockR);
        if (blockL) AudioStream_F32::release(blockL);
        return;
      }

      if (_enable)
      {
        _fpre1L.filterBlock(blockL);
        _fpre2L.filterBlock(blockL);
        _fpre3L.filterBlock(blockL);
        _fpre4L.filterBlock(blockL);
        _fpre5L.filterBlock(blockL);
        _fpre6L.filterBlock(blockL);
        _frevL.filterBlock(blockL, roomL);
        BlockOperations::scale(blockL, _direct);
        BlockOperations::scale(roomL, _room);
        _delayL.delayBlock(roomL);
        _revL.processBlock(roomL);
        BlockOperations::add(roomL, blockL);
        _fpost1L.filterBlock(blockL);
        _fpost2L.filterBlock(blockL);

        _fpre1R.filterBlock(blockR);
        _fpre2R.filterBlock(blockR);
        _fpre3R.filterBlock(blockR);
        _fpre4R.filterBlock(blockR);
        _fpre5R.filterBlock(blockR);
        _fpre6R.filterBlock(blockR);
        _frevR.filterBlock(blockR, roomR);
        BlockOperations::scale(blockR, _direct);
        BlockOperations::scale(roomR, _room);
        _delayR.delayBlock(roomR);
        _revR.processBlock(roomR);
        BlockOperations::add(roomR, blockR);
        _fpost1R.filterBlock(blockR);
        _fpost2R.filterBlock(blockR);
      }
      else
      {
        BlockOperations::scale(blockL, 0.5f);
        BlockOperations::scale(blockR, 0.5f);
      }

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
    }

  private:
    audio_block_f32_t *inputQueueArray[2];
    audio_block_f32_t _roomL;
    audio_block_f32_t _roomR;
    FirstOrder _fpre1L;
    Biquad _fpre2L;
    Biquad _fpre3L;
    Biquad _fpre4L;
    Biquad _fpre5L;
    Biquad _fpre6L;
    FirstOrder _fpre1R;
    Biquad _fpre2R;
    Biquad _fpre3R;
    Biquad _fpre4R;
    Biquad _fpre5R;
    Biquad _fpre6R;
    FirstOrder _frevL;
    FirstOrder _frevR;
    Biquad _fpost1L;
    Biquad _fpost2L;
    Biquad _fpost1R;
    Biquad _fpost2R;
    FreeverbL _revL;
    FreeverbR _revR;
    RAM1Delay<346> _delayL;
    RAM1Delay<291> _delayR;

    bool _enable = false;
    float _direct = 1.0f;
    float _room = 1.0f;
};

#endif
