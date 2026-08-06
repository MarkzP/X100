#ifndef _effect_equalizer_F32_h_
#define _effect_equalizer_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectEqualizer_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:hdr
  public:
    AudioEffectEqualizer_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioEffectEqualizer_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray),
      _f1(settings.sample_rate_Hz),
      _f2(settings.sample_rate_Hz),
      _f3(settings.sample_rate_Hz),
      _f4(settings.sample_rate_Hz),
      _f5(settings.sample_rate_Hz),
      _f6(settings.sample_rate_Hz),
      _f7(settings.sample_rate_Hz),
      _f8(settings.sample_rate_Hz)
    {
    }

    void begin()
    {
      _f8.setLowpassFirstOrder(15000.0);
    }

    FLASHMEM void enable(bool enable = false)
    {
      _enable = enable;
    }

    void eq(int band, float db)
    {
      db = db < -15.0f ? -15.0f : db > 15.0f ? 15.0f : db;

      switch (band)
      {
        case 1: _f1.setPeak(100.0, db, _q); break;
        case 2: _f2.setPeak(200.0, db, _q); break;
        case 3: _f3.setPeak(400.0, db, _q); break;
        case 4: _f4.setPeak(800.0, db, _q); break;
        case 5: _f5.setPeak(1600.0, db, _q); break;
        case 6: _f6.setPeak(3200.0, db, _q); break;
        case 7: _f7.setHighShelfFirstOrder(6400.0, db); break;
      }
    }

    FLASHMEM void level(float level)
    {
      level = level < -9.0f ? -9.0f : level > 9.0f ? 9.0f : level;
      _level = powf(10.0f, level / 20.0f);
    }

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

      _f1.filterBlock(block);
      _f2.filterBlock(block);
      _f3.filterBlock(block);
      _f4.filterBlock(block);
      _f5.filterBlock(block);
      _f6.filterBlock(block);
      _f7.filterBlock(block);
      _f8.filterBlock(block);
      BlockOperations::scale(block, _level);

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    bool _enable = false;

    static constexpr double _q = 0.85f;

    Biquad _f1;
    Biquad _f2;
    Biquad _f3;
    Biquad _f4;
    Biquad _f5;
    Biquad _f6;
    FirstOrder _f7;
    FirstOrder _f8;

    float _level = 1.0f;
};

#endif
