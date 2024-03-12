#ifndef effect_warmth_F32_h_
#define effect_warmth_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

class AudioEffectWarmth_F32 :
  public AudioStream_F32
{
//GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
//GUI: shortName:warmth  
  public:
    AudioEffectWarmth_F32(void): AudioStream_F32(1, inputQueueArray) {}
    AudioEffectWarmth_F32(const AudioSettings_F32 &settings): AudioStream_F32(1, inputQueueArray) {}

    void begin()
    {
    }

    void warmth(float warmth)
    {
      warmth = warmth < 0.0f ? 0.0f : warmth > 1.0f ? 1.0f : warmth;
      _warmth = warmth * 3.5f;
      _comp = 1.0f / ((_warmth * 0.75f) + 1.0f);
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      
      if (!block) return;

      float sample;
      for (uint16_t i = 0; i < block->length; i++)
      {
        sample = block->data[i];
        sample = (_warmth + 1.0f) * sample / (1.0f + _warmth * fabsf(sample));
        block->data[i] = sample * _comp;
      }

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    float _warmth = 0.0f;
    float _comp = 1.0f;
};

#endif
