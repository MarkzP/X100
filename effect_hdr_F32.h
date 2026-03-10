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

    float db_low() { return FilterUtils::u2dB(_ld.level()); }
    float db_high() { return FilterUtils::u2dB(_hd.level()); }
    float gain() { return _gain; }

    virtual void update(void)
    {
      audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t *blockH = AudioStream_F32::receiveReadOnly_f32(1);
      if (!blockL || !blockH)
      {
        if (blockH) AudioStream_F32::release(blockH);
        if (blockL) AudioStream_F32::release(blockL);
        return;
      }

      float *pl = blockL->data;
      float *ph = blockH->data;
      float *endl = pl + blockL->length;
      do
      {
        float rs = *pl;
        float hs = *ph;
        float ls = rs * _gain;

        float low = _ld.detect(ls);
        float high = _hd.detect(hs);

        if (high < _minLevel)
        {
          // high below min - use high gain input only
          *pl = hs;
        }
        else if (high < _maxLevel)
        {
          // high between min & max
          float lx = (high - _minLevel) * (1.0f / (_maxLevel - _minLevel));
          float hx = 1.0f - lx;
          *pl = (hs * hx) + (ls * lx);

          float gainError = (high / low) - 1.0f;
          gainError = lx < 0.5f ? gainError * lx : gainError * hx;
          gainError *= 0.00025f;
          gainError = FilterUtils::denormFloat(gainError);
          _gain += gainError;
          _gain = _gain < _minGain ? _minGain : _gain > _maxGain ? _maxGain : _gain;
        }
        else
        {
          // high above max - use low gain input only
          *pl = ls;
        }
        
        pl++;
        ph++;
      }
      while (pl < endl);

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::release(blockL);
      AudioStream_F32::release(blockH);
    }

  private:
    audio_block_f32_t *inputQueueArray[2];
    Detector _ld;
    Detector _hd;
    static constexpr float _minLevel = 0.001f; // ~ -60.0db
    static constexpr float _maxLevel = 0.5f;    // ~ -6.0db
    static constexpr float _maxGain = 6.0f;
    static constexpr float _minGain = 3.0f;
    float _gain = 4.0f;
};

#endif
