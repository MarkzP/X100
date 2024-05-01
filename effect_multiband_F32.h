#ifndef effect_multiband_F32_h_
#define effect_multiband_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

class AudioEffectMultiband_F32 :
  public AudioStream_F32
{
//GUI: inputs:2, outputs:2  //this line used for automatic generation of GUI node
//GUI: shortName:multiband  
  public:
    void parameters(
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
        float sWidth = 0.5f,
        bool swap = false        
      )
    {
      //calcs here
      driv1 = powf(10.0f, (2.5f * lComp) - 1.0f);
      trim1 = (0.5f + (4.0f - 2.0f * attack) * (lComp * lComp * lComp)); 
      trim1 = (trim1 * powf(10.0f , 2.0f * lOut - 1.0f));
      att1 = powf(10.0f, -0.05f -(2.5f * attack));
      rel1 = powf(10.0f, -2.0f - (3.5f * release));
      
      driv2 = powf(10.0f, (2.5f * mComp) - 1.0f);
      trim2 = (0.5f + (4.0f - 2.0f * attack) * (mComp * mComp * mComp)); 
      trim2 = (trim2 * powf(10.0f, 2.0f * mOut - 1.0f));
      att2 = powf(10.0f, -0.05f -(2.0f * attack));
      rel2 = powf(10.0f, -2.0f - (3.0f * release));
      
      driv3 = powf(10.0f, (2.5f * hComp) - 1.0f);
      trim3 = (0.5f + (4.0f - 2.0f * attack) * (hComp * hComp * hComp)); 
      trim3 = (trim3 * powf(10.0f, 2.0f * hOut - 1.0f));
      att3 = powf(10.0f, -0.05f - (1.5f * attack));
      rel3 = powf(10.0f, -2.0f - (2.5f * release));
      
      switch (listen)
      {
        case 0: bypass = false; trim2 = 0.0f; trim3 = 0.0f; slev = 0.0f; break;
        case 1: bypass = false; trim1 = 0.0f; trim3 = 0.0f; slev = 0.0f; break;
        case 2: bypass = false; trim1 = 0.0f; trim2 = 0.0f; slev = 0.0f; break;
        case 3: bypass = false; slev = sWidth; break;
        default: bypass = true; break;
      }
      fi1 = powf(10.0f, lowMid - 1.70f); fo1 = (1.0f - fi1);
      fi2 = powf(10.0f, midHigh - 1.05f); fo2 = (1.0f - fi2);

      /*
      Serial.printf("low/mid: %.3fhz mid/high: %.3fhz\n",
            _sample_rate_Hz * fi1 * (0.098f + 0.09f * fi1 + 0.5f * powf(fi1, 8.2f)),
            _sample_rate_Hz * fi2 * (0.015f + 0.15f * fi2 + 0.9f * powf(fi2, 8.2f)));
      */

      mswap = swap;
    }

    AudioEffectMultiband_F32(void): AudioStream_F32(2, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
      parameters();
    }

    AudioEffectMultiband_F32(const AudioSettings_F32 &settings): AudioStream_F32(2, inputQueueArray)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
      parameters();
    }  
    
    virtual void update(void)
    {
      float a, b, c, d, l = fb3, m, h, s, sl = slev, tmp1, tmp2, tmp3;  
      float f1i = fi1, f1o = fo1, f2i = fi2, f2o = fo2, b1 = fb1, b2 = fb2;
      float g1 = gain1, d1 = driv1, t1 = trim1, a1 = att1, r1 = 1.0f - rel1;
      float g2 = gain2, d2 = driv2, t2 = trim2, a2 = att2, r2 = 1.0f - rel2;
      float g3 = gain3, d3 = driv3, t3 = trim3, a3 = att3, r3 = 1.0f - rel3;
      bool ms = mswap;
      bool bp = bypass;
    
      audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t *blockR = AudioStream_F32::receiveWritable_f32(1);
      
      if (!blockL) return;
      if (!blockR) blockR = blockL;

      for (uint16_t i = 0; i < blockL->length; i++)
      {
        a = blockL->data[i];
        b = blockR->data[i];
        
        b = (ms) ? -b : b;
        
        s = (a - b) * sl; //keep stereo component for later
        a += b;
        b2 = (f2i * a) + (f2o * b2); //crossovers
        b1 = (f1i * b2) + (f1o * b1); 
        l = (f1i * b1) + (f1o * l);
        m = b2 - l;
        h = a - b2;
        
        tmp1 = (l > 0.0f) ? l : -l;  //l
        g1 = (tmp1 > g1) ? g1+a1 * (tmp1 - g1) : g1*r1;
        tmp1 = 1.0f / (1.0f + d1 * g1); 
        
        tmp2 = (m > 0.0f) ? m : -m;
        g2 = (tmp2 > g2) ? g2+a2 * (tmp2-g2) : g2*r2;
        tmp2 = 1.0f / (1.0f + d2 * g2); 
        
        tmp3 = (h > 0.0f)? h : -h;
        g3 = (tmp3 > g3) ? g3+a3 * (tmp3-g3) : g3*r3;
        tmp3 = 1.0f / (1.0f + d3 * g3); 
        
        a = (l*tmp3*t1) + (m*tmp2*t2) + (h*tmp3*t3);
        c = a + s; // output
        d = (ms) ? s - a : a - s;

        if (!bp)
        {
          blockL->data[i] = c;
          blockR->data[i] = d;
        }
      }

      gain1 = (g1 < 1.0e-8f) ? 0.0f : g1;
      gain2 = (g2 < 1.0e-8f) ? 0.0f : g2;
      gain3 = (g3 < 1.0e-8f) ? 0.0f : g3;
      if (fabsf(b1) < 1.0e-8f) { fb1 = 0.0f; fb2 = 0.0f; fb3 = 0.0f; }
      else { fb1 = b1; fb2 = b2; fb3 = l; }

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::release(blockL);
      if (blockR != blockL)
      {
        AudioStream_F32::transmit(blockR, 1);
        AudioStream_F32::release(blockR);
      }        
    }
    
  private:
    audio_block_f32_t *inputQueueArray[2];
    float _sample_rate_Hz;

    float gain1, driv1, att1, rel1, trim1;
    float gain2, driv2, att2, rel2, trim2;
    float gain3, driv3, att3, rel3, trim3;
    float fi1, fb1, fo1, fi2, fb2, fo2, fb3, slev;
    bool mswap;
    bool bypass;
};

#endif
