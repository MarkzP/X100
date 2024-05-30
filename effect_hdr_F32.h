#ifndef _effect_hdr_F32_h_
#define _effect_hdr_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectHDR_F32 :
  public AudioStream_F32
{
    //GUI: inputs:2, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:hdr
  public:
    AudioEffectHDR_F32(void):
      AudioStream_F32(2, inputQueueArray)
    {
    }

    AudioEffectHDR_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(2, inputQueueArray)
    {
    }

    virtual void update(void)
    {
      audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
      if (!blockL) return;

      audio_block_f32_t *blockH = AudioStream_F32::receiveReadOnly_f32(1);
      if (!blockH)
      {
        AudioStream_F32::release(blockL);
        return;
      }

      float gain = _gain;

      for (uint16_t i = 0; i < blockL->length; i++)
      {
        float ls = blockL->data[i] * gain;
        float hs = blockH->data[i];
        float lx = 0.0f;
        float hx = 1.0f;

        float low = _ld.detect(ls);
        float high = _lh.detect(hs);
        float r = 0.0f;

        if (high < _min)
        {
          lx = 0.0f;
        }
        else if (high < _max)
        {
          lx = high * (1.0f / _max);
          hx = 1.0f - lx;
          r = (high / low) - 1.0f;
          r = lx < 0.5f ? r * lx : r * hx;
          r *= 0.00025f;
        }
        else
        {
          lx = 1.0f;
          hx = 0.0f;
        }

        blockL->data[i] = (hs * hx) + (ls * lx);

        gain += r;
        _error = r;
      }

      _gain = gain;

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::release(blockL);
      AudioStream_F32::release(blockH);
    }

  private:
    audio_block_f32_t *inputQueueArray[2];
    Detector _ld;
    Detector _lh;
    static constexpr float _min = 0.001f; // ~ -60.0db
    static constexpr float _max = 0.5f;    // ~ -6.0db
    float _gain = 4.0f;
    float _error = 0.0f;
};

#endif
