#ifndef effect_saturation_F32_h_
#define effect_saturation_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include <arm_math.h>

class AudioEffectSaturation_F32 :
  public AudioStream_F32
{
//GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
//GUI: shortName:delay  
  public:
    AudioEffectSaturation_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioEffectSaturation_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    void saturation(float saturation = 0.0f)
    {
      saturation = saturation < 0.0f ? 0.0f : saturation > 1.0f ? 1.0f : saturation;
      
      _pre = (saturation * saturation * 19.5f) + 0.5f;
      _post = ((2.0f / 3.0f) / _pre);
    }

    virtual void update(void)
    {
      float sample;
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      
      if (!block) return;

      if (_pre > 0.0f)
      {
        for (uint16_t i = 0; i < block->length; i++)
        {
          sample = block->data[i];

          sample *= _pre;
          sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
          sample = arm_sin_f32(sample * _half_pi) * _post;

          block->data[i] = sample;
        }
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    static constexpr float _half_pi = 1.5707963267948f;
    float _pre;
    float _post;     
};

#endif
