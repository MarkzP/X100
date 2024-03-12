#ifndef effect_simple_delay_F32_h_
#define effect_simple_delay_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

class AudioEffectSimpleDelay_F32 :
  public AudioStream_F32
{
//GUI: inputs:1, outputs:1  //this line used for automatic generation of GUI node
//GUI: shortName:delay  
  public:
    AudioEffectSimpleDelay_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    }

    AudioEffectSimpleDelay_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
    }    

    boolean begin(float *delayline, int d_length)
    {
      _delayline = NULL;
      _delay_length = 0;
      _cb_index = 0;
    
      if (delayline == NULL)
        return (false);
      if (d_length < 10)
        return (false);
    
      _delayline = delayline;
      _delay_length = d_length;
      _delay_samp = 0;
      memset(_delayline, 0, _delay_length * sizeof(float));
    
      return (true);
    }

    void delay(float ms)
    {
      if (!_delayline) return;
      
      if (ms < 0.0f) ms = 0.0f;
      int numsamp = (int)(ms * 0.001f * _sample_rate_Hz);
      if (numsamp > _delay_length - 1) numsamp = _delay_length - 1;

      _delay_samp = numsamp;
    }
    
    virtual void update(void)
    {
        int _cb_delay_index;

        if (!_delayline) return;

        audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
        
        if (!block) return;

        for (uint16_t i = 0; i < block->length; i++)
        {
          if (_cb_index >= _delay_length) _cb_index = 0;
          _cb_delay_index = _cb_index - _delay_samp;
          if (_cb_delay_index < 0) _cb_delay_index += _delay_length;

          _delayline[_cb_index] = block->data[i];
          block->data[i] = _delayline[_cb_delay_index];

          _cb_index++;
        }

        AudioStream_F32::transmit(block, 0);
        AudioStream_F32::release(block);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    float _sample_rate_Hz;
    float *_delayline;  // pointer for the circular buffer
    int _cb_index;   // current write pointer of the circular buffer
    int _delay_length; // calculated number of samples of the delay
    int _delay_samp;
};

#endif
