#ifndef effect_tremolo_F32_h_
#define effect_tremolo_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

class AudioEffectTremolo_F32 :
  public AudioStream_F32
{
//GUI: inputs:2, outputs:1  //this line used for automatic generation of GUI node
//GUI: shortName:tremolo 
  public:
    AudioEffectTremolo_F32(void):
      AudioStream_F32(2, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    }

    AudioEffectTremolo_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(2, inputQueueArray)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
    }

    void controlSmoothing(float time = 0.02f)
    {
      if (time > 0.0f) {
        if (time > 10.0f) time = 10.0f;
        _smooth = 1.0f - expf(-3.1699f / (_sample_rate_Hz * time));
      }
      else _smooth = 1.0f;
    }

    void offset(float offset)
    {
      if (offset < 0.0f) offset = 0.0f;
      else if (offset > 1.0f) offset = 1.0f;

      _offset = offset;
    }

    virtual void update(void)
    {
      float offset = _offset;
      float smooth = _smooth;
      float control = _control;
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      
      if (!block) return;

      audio_block_f32_t *cb = AudioStream_F32::receiveReadOnly_f32(1);
      if (cb)
      {
        for (uint16_t i = 0; i < block->length; i++)
        { 
          control += ((cb->data[i] + offset) - control) * smooth;
          if (control < 0.0f) control = 0.0f;
          else if (control > 1.0f) control = 1.0f;
          block->data[i] *= control;
        }
        AudioStream_F32::release(cb);
      }

      _control = control;

      AudioStream_F32::transmit(block, 0);
      AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[2];
    float _sample_rate_Hz;
    float _smooth = 1.0f;
    float _control = 0.0f;
    float _offset = 1.0f;
};

#endif
