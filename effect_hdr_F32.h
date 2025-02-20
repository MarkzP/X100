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

    void setOutput(int output)
    {
      _out = output;
    }

    void setGain(float gain)
    {
      _auto = false;
      _gain = gain;
    }

    float db_low() { return _rd.db(); }
    float db_high() { return _hd.db(); }
    float lx() { return _lx; }
    float gain() { return _gain; }

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
        float rs = blockL->data[i];
        float ls = rs * gain;
        float hs = blockH->data[i];

        float rx = 0.0f;
        float lx = 0.0f;
        float hx = 1.0f;

        _rd.detect(rs);
        float low = _ld.detect(ls);
        float high = _hd.detect(hs);
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

        if (_out == 1)
        {
          rx = 1.0f;
          lx = 0.0f;
          hx = 0.0f;          
        }
        else if (_out == 2)
        {
          rx = 0.0f;
          lx = 0.0f;
          hx = 1.0f;           
        }

        blockL->data[i] = (hs * hx) + (ls * lx) + (rs * rx);

        if (_auto) 
        {
          gain += r;
          gain = gain < _ming ? _ming : gain > _maxg ? _maxg : gain;
        }        
        
        _lx = lx;
      }

      _gain = gain;

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::release(blockL);
      AudioStream_F32::release(blockH);
    }

  private:
    audio_block_f32_t *inputQueueArray[2];
    Detector _rd;
    Detector _ld;
    Detector _hd;
    static constexpr float _min = 0.001f; // ~ -60.0db
    static constexpr float _max = 0.5f;    // ~ -6.0db
    bool _auto = true;
    float _gain = 4.0f;
    float _maxg = 6.0f;
    float _ming = 3.0f;
    float _lx = 0.0f;
    int _out = 0;
};

#endif
