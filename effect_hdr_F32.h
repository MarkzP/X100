#ifndef effect_hdr_F32_h_
#define effect_hdr_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

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

    void gain(float g)
    {
      _gain = g;
    }

    virtual void update(void)
    {
        audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
        audio_block_f32_t *blockH = AudioStream_F32::receiveReadOnly_f32(1);
        
        if (!blockL) return;

        if (!blockH)
        {
          AudioStream_F32::release(blockL);
          return;
        }

        for (uint16_t i = 0; i < blockL->length; i++)
        {
          float low = blockL->data[i];
          float high = blockH->data[i];
          
          low -= (_offsetLow += (low - _offsetLow) * _hpa);
          high -= (_offsetHigh += (high - _offsetHigh) * _hpa);
          float sample = (low * _gain);

          
          float lowGain = fabsf(low);
          float highGain = fabsf(high);
          
          float x = (highGain - _low) * _range;
          
          if (x < 0.0f)
          {
            // Below - low gain input is too low to reliably track gain error - use high gain only
            sample = high;
          }
          else if (x < 1.0f)
          {
            // Transition
            // TODO track gain error

            // Interpolate between samples
            sample = (sample * x) + (low * (1.0f - x));
          }

          blockL->data[i] = sample;
        }

        AudioStream_F32::transmit(blockL, 0);
        AudioStream_F32::release(blockL);
        AudioStream_F32::release(blockH);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[2];
    static constexpr float _low = 1.0f / 10.0f;
    static constexpr float _high = 1.0f - _low;
    static constexpr float _range = 1.0f / (_high - _low);
    static constexpr float _hpa = 0.00005f;
    
    float _offsetLow = 0.0f;
    float _offsetHigh = 0.0f;

    float _gain = 4.04f;    
};

#endif
