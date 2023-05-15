#ifndef filter_state_variable_F32_h_
#define filter_state_variable_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

class AudioFilterStateVariable_F32 :
  public AudioStream_F32
{
    //GUI: inputs:2, outputs:1  //this line used for automatic generation of GUI node
    //GUI: shortName:filter
  public:
    AudioFilterStateVariable_F32(void):
      AudioStream_F32(2, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
      _low = 0.0f;
      _band = 0.0f;
      frequency();
      resonance();
      octaveControl();
      controlSmoothing();
    }

    AudioFilterStateVariable_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(2, inputQueueArray)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
      _low = 0.0f;
      _band = 0.0f;
      frequency();
      resonance();
      octaveControl();
      controlSmoothing();
    }

    void frequency(float freq = 200.0f)
    {
      float center = PI * freq / _sample_rate_Hz;

      if (center < 0.0001f) center = 0.0001f;
      else if (center > 0.9999f) center = 0.9999f;

      _center = center;
    }

    void resonance(float q = 4.0f)
    {
      if (q < 0.5f) q = 0.5f;
      else if (q > 5.0f) q = 5.0f;

      _q = 1.0f / q;
    }

    void octaveControl(float n = 3.0f)
    {
      if (n < 0.0f) n = 0.0f;
      else if (n > 6.9999f) n = 6.9999f;
      
      _octave = n;
    }

    void controlSmoothing(float time = 0.1f)
    {
      if (time > 0.0f) {
        if (time > 10.0f) time = 10.0f;
        _smooth = 1.0f - expf(-3.1699f / (_sample_rate_Hz * time));
      }
      else _smooth = 1.0f;
    }

    virtual void update(void)
    {
      float input, high;
      float octave = _octave;
      float center = _center;
      float f = _center;
      float q = _q;      
      float low = _low;
      float band = _band;
      float control = _control;
      float smooth = _smooth;

      audio_block_f32_t* input_block = AudioStream_F32::receiveReadOnly_f32(0);
      if (!input_block) return;
      
      audio_block_f32_t* control_block = AudioStream_F32::receiveReadOnly_f32(1); 

      audio_block_f32_t* lowpass_block = AudioStream_F32::allocate_f32();
      if (!lowpass_block) {
        AudioStream_F32::release(input_block);
        if (control_block) AudioStream_F32::release(control_block);
        return;
      }
      audio_block_f32_t* bandpass_block = AudioStream_F32::allocate_f32();
      if (!bandpass_block) {
        AudioStream_F32::release(input_block);
        AudioStream_F32::release(lowpass_block);
        if (control_block) AudioStream_F32::release(control_block);
        return;
      }
      audio_block_f32_t* highpass_block = AudioStream_F32::allocate_f32();
      if (!highpass_block) {
        AudioStream_F32::release(input_block);
        AudioStream_F32::release(lowpass_block);
        AudioStream_F32::release(bandpass_block);
        if (control_block) AudioStream_F32::release(control_block);
        return;
      }

      float* cb = (control_block ? control_block->data : nullptr);
      float* ib = input_block->data;
      float* lp = lowpass_block->data;
      float* bp = bandpass_block->data;
      float* hp = highpass_block->data;

      for (uint16_t i = 0; i < input_block->length; i++)
      {
        input = *ib++;
        if (control_block)
        {
          control += ((*cb++) - control) * smooth;
          if (control < -1.0f) control = -1.0f;
          else if (control > 1.0f) control = 1.0f;
          f = center * fasterexp2f(control * octave);
          if (f < 0.0001f) f = 0.0001f;
          else if (f > 0.9999f) f = 0.9999f;
        }

        low += f * band + 1e-21f;
        high = input - low - q * band;
        band += f * high;
        
        low += f * band + 1e-21f;
        high = input - low - q * band;
        band += f * high;
        
        *lp++ = low;
        *bp++ = band;
        *hp++ = high;        
      }

      _low = low;
      _band = band;
      _control = control;

      if (control_block) AudioStream_F32::release(control_block);
      AudioStream_F32::release(input_block);
      AudioStream_F32::transmit(lowpass_block, 0);
      AudioStream_F32::release(lowpass_block);
      AudioStream_F32::transmit(bandpass_block, 1);
      AudioStream_F32::release(bandpass_block);
      AudioStream_F32::transmit(highpass_block, 2);
      AudioStream_F32::release(highpass_block);
    }

  private:
    inline float fasterexp2f(float p)
    {
      union { uint32_t i; float f; } v = { uint32_t((1 << 23) * (p + 126.94269504f)) };
      return v.f;
    }
  
    audio_block_f32_t *inputQueueArray[2];
    float _sample_rate_Hz;
    float _low;
    float _band;
    float _control;
    float _smooth;
    float _q;
    float _center;
    float _octave;
};

#endif
