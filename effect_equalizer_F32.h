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
      _f3(settings.sample_rate_Hz)
    {
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }

    void eq(int band, float db)
    {
      db = db < -15.0f ? -15.0f : db > 15.0f ? 15.0f : db;

      switch (band)
      {
        case 1: _g1 = db; break;
        case 2: _g2 = db; break;
        case 3: _g3 = db; break;
        case 4: _g4 = db; break;
        case 5: _g5 = db; break;
        case 6: _g6 = db; break;
        case 7: _g7 = db; break;
      }

      _f1.reset().setHighpass1p1z(50.0).begin();
      _f2.reset().setPeak(100.0, _g1, 1.4).begin();
      _f3.reset()
        .setPeak(200.0, _g2, 1.4)
        .setPeak(400.0, _g3, 1.4)
        .setPeak(800.0, _g4, 1.4)
        .setPeak(1600.0, _g5, 1.4)
        .setPeak(3200.0, _g6, 1.4)
        .setPeak(6400.0, _g7, 1.4)
        .setLowpass(12800.0)
        .begin();
    }

    void level(float level)
    {
      level = level < -15.0f ? -15.0f : level > 15.0f ? 15.0f : level;
      _level = powf(10.0f, level / 20.0f);
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;

      if (_enable)
      {
        _f1.filterBlock(block);
        _f2.filterBlock(block);
        _f3.filterBlock(block);
        BlockOperations::scale(block, _level);
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    bool _enable = false;

    HQ1p1zBiquad _f1;
    HQBiquad _f2;
    CascadeBiquad<7> _f3;

    float _g1 = 0.0f;
    float _g2 = 0.0f;
    float _g3 = 0.0f;
    float _g4 = 0.0f;
    float _g5 = 0.0f;
    float _g6 = 0.0f;
    float _g7 = 0.0f;
    float _level = 1.0f;
};

#endif
