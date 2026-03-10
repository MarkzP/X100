#ifndef _effect_multiband_F32_h_
#define _effect_multiband_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

class AudioEffectMultiband_F32 :
  public AudioStream_F32
{
//GUI: inputs:2, outputs:2  //this line used for automatic generation of GUI node
//GUI: shortName:multiband  
  public:
    void begin(
        int listen = 3,
        float lowMid = 0.15f,
        float midHigh = 0.6f,
        float lComp = 0.5f,
        float mComp = 0.5f,
        float hComp = 0.5f,
        float lOut = 0.5f,
        float mOut = 0.5f,
        float hOut = 0.5f,
        float attack = 0.5f,
        float release = 0.5f,
        float sWidth = 0.5f     
      )
    {
      //calcs here
      driv1 = powf(10.0f, (2.5f * lComp) - 1.0f);
      trim1 = (0.5f + (4.0f - 2.0f * attack) * (lComp * lComp * lComp)); 
      trim1 = (trim1 * powf(10.0f , 2.0f * lOut - 1.0f));
      att1 = powf(10.0f, -0.05f -(2.5f * attack));
      rel1 = 1.0f - powf(10.0f, -2.0f - (3.5f * release));
      
      driv2 = powf(10.0f, (2.5f * mComp) - 1.0f);
      trim2 = (0.5f + (4.0f - 2.0f * attack) * (mComp * mComp * mComp)); 
      trim2 = (trim2 * powf(10.0f, 2.0f * mOut - 1.0f));
      att2 = powf(10.0f, -0.05f -(2.0f * attack));
      rel2 = 1.0f - powf(10.0f, -2.0f - (3.0f * release));
      
      driv3 = powf(10.0f, (2.5f * hComp) - 1.0f);
      trim3 = (0.5f + (4.0f - 2.0f * attack) * (hComp * hComp * hComp)); 
      trim3 = (trim3 * powf(10.0f, 2.0f * hOut - 1.0f));
      att3 = powf(10.0f, -0.05f - (1.5f * attack));
      rel3 = 1.0f - powf(10.0f, -2.0f - (2.5f * release));
      
      switch (listen)
      {
        case 0: bypass = false; slev = sWidth; break;
        case 1: bypass = false; trim2 = 0.0f; trim3 = 0.0f; slev = 0.0f; break;
        case 2: bypass = false; trim1 = 0.0f; trim3 = 0.0f; slev = 0.0f; break;
        case 3: bypass = false; trim1 = 0.0f; trim2 = 0.0f; slev = 0.0f; break;

        default: bypass = true; break;
      }

      fi1 = powf(10.0f, lowMid - 1.70f); fo1 = (1.0f - fi1);
      fi2 = powf(10.0f, midHigh - 1.05f); fo2 = (1.0f - fi2);
      
      /*
      Serial.printf("low/mid: %.3fhz mid/high: %.3fhz\n",
            _sample_rate_Hz * fi1 * (0.098f + 0.09f * fi1 + 0.5f * powf(fi1, 8.2f)),
            _sample_rate_Hz * fi2 * (0.015f + 0.15f * fi2 + 0.9f * powf(fi2, 8.2f)));
      */
    }

    AudioEffectMultiband_F32(void): AudioStream_F32(2, inputQueueArray)
    {
    }

    AudioEffectMultiband_F32(const AudioSettings_F32 &settings): AudioStream_F32(2, inputQueueArray)
    {
    }  
    
    virtual void update(void)
    {
      audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t *blockR = AudioStream_F32::receiveWritable_f32(1);
      if (!blockL || !blockR)
      {
        if (blockL) AudioStream_F32::release(blockL);
        if (blockR) AudioStream_F32::release(blockR);
        return;
      }

      float *pl = blockL->data;
      float *pr = blockR->data;
      float *endl = pl + blockL->length;
      do
      {
        float a = *pl;
        float b = *pr;        
        float s = (a - b) * slev; //keep stereo component for later
        a += b;

        fb2 = (fi2 * a) + (fo2 * fb2); //crossovers
        fb1 = (fi1 * fb2) + (fo1 * fb1); 
        fb3 = (fi1 * fb1) + (fo1 * fb3);
        float m = fb2 - fb3;
        float h = a - fb2;
        
        float tmp1 = (fb3 > 0.0f) ? fb3 : -fb3;
        gain1 = (tmp1 > gain1) ? gain1 + att1 * (tmp1 - gain1) : gain1 * rel1;
        tmp1 = 1.0f / (1.0f + driv1 * gain1); 
        
        float tmp2 = (m > 0.0f) ? m : -m;
        gain2 = (tmp2 > gain2) ? gain2 + att2 * (tmp2 - gain2) : gain2 * rel2;
        tmp2 = 1.0f / (1.0f + driv2 * gain2); 
        
        float tmp3 = (h > 0.0f)? h : -h;
        gain3 = (tmp3 > gain3) ? gain3 + att3 * (tmp3 - gain3) : gain3 * rel3;
        tmp3 = 1.0f / (1.0f + driv3 * gain3);        

        if (!bypass)
        {
          a = (fb3 * tmp3 * trim1) + (m * tmp2 * trim2) + (h * tmp3 * trim3);

          *pl = a + s;
          *pr = a - s;
        }

        pl++;
        pr++;
      }
      while (pl < endl);

      gain1 = FilterUtils::denormFloat(gain1);
      gain2 = FilterUtils::denormFloat(gain2);
      gain3 = FilterUtils::denormFloat(gain3);
      
      fb1 = FilterUtils::denormFloat(fb1);
      fb2 = FilterUtils::denormFloat(fb2);
      fb3 = FilterUtils::denormFloat(fb3);

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
    }
    
  private:
    audio_block_f32_t *inputQueueArray[2];
    float gain1, driv1, att1, rel1, trim1;
    float gain2, driv2, att2, rel2, trim2;
    float gain3, driv3, att3, rel3, trim3;
    float fi1, fb1, fo1, fi2, fb2, fo2, fb3, slev;
    bool bypass;
};

#endif
