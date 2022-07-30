/*
 * Non-linear waveshaper for the Teensy OpenAudio F32 library 
 * https://github.com/chipaudette/OpenAudio_ArduinoLibrary
 * crude approximation of the typical twin diode clipping stage used in distortion pedals
 * curves: 
 *  ~0.0 : identity - completely linear (but useless for distortion!)
 *  ~0.5 : slight "tube" warmth
 *  ~1.0 : soft klipping
 *  ~3.5 : nice overdrive
 *  >5.0 : metal 
 *  
 *  by Marc Paquette
 */

#ifndef effect_nonlinear_f32_h_
#define effect_nonlinear_f32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

class AudioEffectNonLinear_F32 : public AudioStream_F32
{
  public:
    AudioEffectNonLinear_F32(void): AudioStream_F32(1, inputQueueArray) {}
    
    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveWritable_f32(0);
      if (!block) return;
      
      float curve = aCurve;
      float pre = aPre;
    
      for (int i = 0; i < block->length; i++)
      {
        float x = block->data[i] * pre;
        block->data[i] = (curve + 1.0f) * x / (1.0f + fabsf(curve * x));
      }
    
      AudioStream_F32::transmit(block);
      AudioStream_F32::release(block);
    }

    void curve(float curve) { aCurve = curve; }
    void gain(float u) { aPre = u; }
  private:
    audio_block_f32_t *inputQueueArray[1];
    float aCurve = 3.3f;
    float aPre = 1.0f;
};

#endif
