
#define UX1
//#define TGA

#include "effect_hdr_F32.h"
#include "effect_multiband_F32.h"
#include "effect_simple_delay_F32.h"
#include "effect_trichorus_F32.h"
#include "effect_tremolo_F32.h"
#include "effect_phaser_F32.h"
#include "filter_state_variable_F32.h"
#include "filter_tonestack_F32.h"
#include "synth_lfo_F32.h"

#include "effect_dynamics_F32.h"              // https://github.com/MarkzP/AudioEffectDynamics_F32
#include "effect_modulated_delay_F32.h"       // https://github.com/MarkzP/ModulatedDelay_F32
#include "effect_freeverb_F32.h"              // https://github.com/MarkzP/Freeverb_F32
#include "effect_nonlinear_F32.h"             // https://github.com/MarkzP/NonLinear_F32

/**********************************************************************************************************************/

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioSettings_F32 audio_settings(AUDIO_SAMPLE_RATE_EXACT, AUDIO_BLOCK_SAMPLES);

// GUItool: begin automatically generated code
AudioInputI2S_F32        audioIn;        //xy=56.5,137
AudioSynthWaveformSine_F32 testTone;       //xy=56.5,175
AudioEffectHDR_F32       hdr;           //xy=169,220
AudioMixer4_F32          inputMixer;     //xy=288.5,149
AudioAnalyzePeak_F32     levelIn;        //xy=416.50000762939453,235.00000190734863
AudioFilterBiquad_F32    tunerFilter;    //xy=433.50000762939453,81.00000190734863
AudioFilterBiquad_F32    preampEq;       //xy=433.50000762939453,129.00000190734863
AudioFilterBiquad_F32    detectorFilter;        //xy=436.00000762939453,184.00000190734863
AudioConvert_F32toI16    toTuner;        //xy=570.5000076293945,81.00000190734863
AudioEffectDynamics_F32  dynamics;       //xy=580.5000076293945,155.00000190734863
AudioSynthLfo_F32        filterLfo;      //xy=583.5000076293945,201.00000190734863
AudioMixer4_F32          svModMixer;     //xy=718.5000076293945,208.00000190734863
AudioFilterStateVariable_F32 svFilter;       //xy=855.5000076293945,181.00000190734863
AudioMixer4_F32          filterMixer;    //xy=986.5000076293945,171.00000190734863
AudioEffectNonLinear_F32 distortion;     //xy=1118.5000076293945,171.00000190734863
AudioFilterToneStack_F32 toneStack;      //xy=1251.5000076293945,171.00000190734863
AudioSynthLfo_F32        tremoloLfo;     //xy=1362.5000076293945,211.00000190734863
AudioEffectPhaser_F32    phaser;         //xy=1377.5000076293945,171.00000190734863
AudioEffectTremolo_F32   tremoloVca;     //xy=1508.5000076293945,177.00000190734863
AudioMixer4_F32          chorusInputL;   //xy=1681.5000076293945,89.00000190734863
AudioSynthLfo_F32        chorusLfoL;     //xy=1686.5000076293945,142.00000190734863
AudioMixer4_F32          chorusInputC;   //xy=1691.5000076293945,192.00000190734863
AudioSynthLfo_F32        chorusLfoC;     //xy=1693.5000076293945,246.00000190734863
AudioMixer4_F32          chorusInputR;   //xy=1697.5000076293945,303.00000190734863
AudioSynthLfo_F32        chorusLfoR;     //xy=1704.5000076293945,357.00000190734863
AudioEffectModulatedDelay_F32 chorusModDelayL; //xy=1858.5000076293945,96.00000190734863
AudioEffectModulatedDelay_F32 chorusModDelayC; //xy=1869.5000076293945,199.00000190734863
AudioEffectModulatedDelay_F32 chorusModDelayR; //xy=1881.5000076293945,310.00000190734863
AudioFilterBiquad_F32    chorusPostFilterL; //xy=2055.5000076293945,140.00000190734863
AudioFilterBiquad_F32    chorusPostFilterR; //xy=2056.5000076293945,346.00000190734863
AudioFilterBiquad_F32    chorusPostFilterC; //xy=2062.5000076293945,234.00000190734863
AudioMixer4_F32          chorusMixerL;   //xy=2241.5000076293945,171.00000190734863
AudioMixer4_F32          chorusMixerR;   //xy=2255.5000076293945,319.00000190734863
AudioMixer4_F32          delayFeedbackL; //xy=2431.5000076293945,209.00000190734863
AudioMixer4_F32          delayFeedbackR; //xy=2432.5000076293945,282.00000190734863
AudioEffectSimpleDelay_F32 delayL;         //xy=2577.499973297119,208.99999523162842
AudioEffectSimpleDelay_F32 delayR;         //xy=2579.6819038391113,281.9999942779541
AudioFilterBiquad_F32    delayFilterL;   //xy=2708.499931335449,209.00000381469727
AudioFilterBiquad_F32    delayFilterR;   //xy=2711.5909309387207,281.99999237060547
AudioMixer4_F32          delayMixerL;    //xy=2867.2273864746094,192.00001335144043
AudioMixer4_F32          delayMixerR;    //xy=2869.2273864746094,339.00001335144043
AudioEffectSimpleDelay_F32 reverbPreDelay; //xy=3040.357109069824,266.99999809265137
AudioFilterBiquad_F32    reverbPreFilter; //xy=3209.357109069824,267.99999809265137
AudioEffectFreeverbStereo_F32 reverb;         //xy=3349.357109069824,267.99999809265137
AudioMixer4_F32          reverbMixerL;   //xy=3503.607162475586,211.75000190734863
AudioMixer4_F32          reverbMixerR;   //xy=3506.607162475586,357.75000190734863
AudioEffectMultiband_F32 multiband;      //xy=3650.607162475586,283.75000190734863
AudioFilterBiquad_F32    cabSimL;        //xy=3785.607162475586,209.75000190734863
AudioFilterBiquad_F32    cabSimR;        //xy=3785.607162475586,355.75000190734863
AudioFilterBiquad_F32    roomFilter;     //xy=3916.607162475586,292.75000190734863
AudioEffectFreeverbStereo_F32 roomReverb;     //xy=4060.607162475586,292.75000190734863
AudioEffectSimpleDelay_F32 roomDelayR;     //xy=4207.607162475586,330.75000190734863
AudioEffectSimpleDelay_F32 roomDelayL;     //xy=4208.607162475586,257.75000190734863
AudioMixer4_F32          cabSimMixerL;   //xy=4372.607162475586,232.75000190734863
AudioMixer4_F32          cabSimMixerR;   //xy=4377.607162475586,375.75000190734863
AudioAnalyzePeak_F32     levelOutR;      //xy=4533.607173919678,395.0000057220459
AudioAnalyzePeak_F32     levelOutL;      //xy=4534.857177734375,204.00000095367432
AudioMixer4_F32          outMixerR;      //xy=4534.857177734375,333.7500057220459
AudioMixer4_F32          outMixerL;      //xy=4535.857177734375,260.00000190734863
AudioOutputI2S_F32       audioOut;       //xy=4696.357177734375,296.25000381469727
AudioConnection_F32          patchCord1(audioIn, 0, inputMixer, 0);
AudioConnection_F32          patchCord2(audioIn, 0, hdr, 0);
AudioConnection_F32          patchCord3(audioIn, 1, inputMixer, 1);
AudioConnection_F32          patchCord4(audioIn, 1, hdr, 1);
AudioConnection_F32          patchCord5(testTone, 0, inputMixer, 2);
AudioConnection_F32          patchCord6(hdr, 0, inputMixer, 3);
AudioConnection_F32          patchCord7(inputMixer, levelIn);
AudioConnection_F32          patchCord8(inputMixer, preampEq);
AudioConnection_F32          patchCord9(inputMixer, tunerFilter);
AudioConnection_F32          patchCord10(inputMixer, detectorFilter);
AudioConnection_F32          patchCord11(tunerFilter, toTuner);
AudioConnection_F32          patchCord12(preampEq, 0, dynamics, 0);
AudioConnection_F32          patchCord13(detectorFilter, 0, dynamics, 1);
AudioConnection_F32          patchCord14(dynamics, 0, svFilter, 0);
AudioConnection_F32          patchCord15(dynamics, 0, filterMixer, 0);
AudioConnection_F32          patchCord16(dynamics, 1, svModMixer, 0);
AudioConnection_F32          patchCord17(filterLfo, 0, svModMixer, 1);
AudioConnection_F32          patchCord18(svModMixer, 0, svFilter, 1);
AudioConnection_F32          patchCord19(svFilter, 0, filterMixer, 1);
AudioConnection_F32          patchCord20(svFilter, 1, filterMixer, 2);
AudioConnection_F32          patchCord21(svFilter, 2, filterMixer, 3);
AudioConnection_F32          patchCord22(filterMixer, distortion);
AudioConnection_F32          patchCord23(distortion, toneStack);
AudioConnection_F32          patchCord24(toneStack, phaser);
AudioConnection_F32          patchCord25(tremoloLfo, 0, tremoloVca, 1);
AudioConnection_F32          patchCord26(phaser, 0, tremoloVca, 0);
AudioConnection_F32          patchCord27(tremoloVca, 0, chorusInputL, 0);
AudioConnection_F32          patchCord28(tremoloVca, 0, chorusInputC, 0);
AudioConnection_F32          patchCord29(tremoloVca, 0, chorusInputR, 0);
AudioConnection_F32          patchCord30(tremoloVca, 0, chorusMixerL, 0);
AudioConnection_F32          patchCord31(tremoloVca, 0, chorusMixerR, 0);
AudioConnection_F32          patchCord32(chorusInputL, 0, chorusModDelayL, 0);
AudioConnection_F32          patchCord33(chorusLfoL, 0, chorusModDelayL, 1);
AudioConnection_F32          patchCord34(chorusInputC, 0, chorusModDelayC, 0);
AudioConnection_F32          patchCord35(chorusLfoC, 0, chorusModDelayC, 1);
AudioConnection_F32          patchCord36(chorusInputR, 0, chorusModDelayR, 0);
AudioConnection_F32          patchCord37(chorusLfoR, 0, chorusModDelayR, 1);
AudioConnection_F32          patchCord38(chorusModDelayL, chorusPostFilterL);
AudioConnection_F32          patchCord39(chorusModDelayC, chorusPostFilterC);
AudioConnection_F32          patchCord40(chorusModDelayR, chorusPostFilterR);
AudioConnection_F32          patchCord41(chorusPostFilterL, 0, chorusMixerL, 1);
AudioConnection_F32          patchCord42(chorusPostFilterL, 0, chorusMixerR, 1);
AudioConnection_F32          patchCord43(chorusPostFilterL, 0, chorusInputL, 1);
AudioConnection_F32          patchCord44(chorusPostFilterR, 0, chorusMixerL, 3);
AudioConnection_F32          patchCord45(chorusPostFilterR, 0, chorusMixerR, 3);
AudioConnection_F32          patchCord46(chorusPostFilterR, 0, chorusInputR, 1);
AudioConnection_F32          patchCord47(chorusPostFilterC, 0, chorusMixerL, 2);
AudioConnection_F32          patchCord48(chorusPostFilterC, 0, chorusMixerR, 2);
AudioConnection_F32          patchCord49(chorusPostFilterC, 0, chorusInputC, 1);
AudioConnection_F32          patchCord50(chorusPostFilterC, 0, chorusInputL, 2);
AudioConnection_F32          patchCord51(chorusPostFilterC, 0, chorusInputR, 2);
AudioConnection_F32          patchCord52(chorusMixerL, 0, delayMixerL, 0);
AudioConnection_F32          patchCord53(chorusMixerL, 0, delayFeedbackL, 0);
AudioConnection_F32          patchCord54(chorusMixerR, 0, delayMixerR, 0);
AudioConnection_F32          patchCord55(chorusMixerR, 0, delayFeedbackR, 0);
AudioConnection_F32          patchCord56(delayFeedbackL, delayL);
AudioConnection_F32          patchCord57(delayFeedbackR, delayR);
AudioConnection_F32          patchCord58(delayL, delayFilterL);
AudioConnection_F32          patchCord59(delayR, delayFilterR);
AudioConnection_F32          patchCord60(delayFilterL, 0, delayMixerL, 1);
AudioConnection_F32          patchCord61(delayFilterL, 0, delayFeedbackL, 1);
AudioConnection_F32          patchCord62(delayFilterL, 0, delayFeedbackR, 2);
AudioConnection_F32          patchCord63(delayFilterR, 0, delayMixerR, 1);
AudioConnection_F32          patchCord64(delayFilterR, 0, delayFeedbackR, 1);
AudioConnection_F32          patchCord65(delayFilterR, 0, delayFeedbackL, 2);
AudioConnection_F32          patchCord66(delayMixerL, 0, reverbMixerL, 0);
AudioConnection_F32          patchCord67(delayMixerR, 0, reverbMixerR, 0);
AudioConnection_F32          patchCord68(delayMixerR, reverbPreDelay);
AudioConnection_F32          patchCord69(reverbPreDelay, reverbPreFilter);
AudioConnection_F32          patchCord70(reverbPreFilter, reverb);
AudioConnection_F32          patchCord71(reverb, 0, reverbMixerL, 1);
AudioConnection_F32          patchCord72(reverb, 1, reverbMixerR, 1);
AudioConnection_F32          patchCord73(reverbMixerL, 0, multiband, 0);
AudioConnection_F32          patchCord74(reverbMixerR, 0, multiband, 1);
AudioConnection_F32          patchCord75(multiband, 0, cabSimL, 0);
AudioConnection_F32          patchCord76(multiband, 1, cabSimR, 0);
AudioConnection_F32          patchCord77(cabSimL, 0, cabSimMixerL, 0);
AudioConnection_F32          patchCord78(cabSimL, roomFilter);
AudioConnection_F32          patchCord79(cabSimR, 0, cabSimMixerR, 0);
AudioConnection_F32          patchCord80(roomFilter, roomReverb);
AudioConnection_F32          patchCord81(roomReverb, 0, roomDelayL, 0);
AudioConnection_F32          patchCord82(roomReverb, 1, roomDelayR, 0);
AudioConnection_F32          patchCord83(roomDelayR, 0, cabSimMixerR, 1);
AudioConnection_F32          patchCord84(roomDelayL, 0, cabSimMixerL, 1);
AudioConnection_F32          patchCord85(cabSimMixerL, levelOutL);
AudioConnection_F32          patchCord86(cabSimMixerL, 0, outMixerL, 0);
AudioConnection_F32          patchCord87(cabSimMixerR, levelOutR);
AudioConnection_F32          patchCord88(cabSimMixerR, 0, outMixerR, 0);
AudioConnection_F32          patchCord89(outMixerR, 0, audioOut, 1);
AudioConnection_F32          patchCord90(outMixerL, 0, audioOut, 0);
// GUItool: end automatically generated code


/**********************************************************************************************************************/

AudioAnalyzeNoteFrequency tuner;      //xy=401,113
AudioConnection          patchCordTuner1(toTuner, tuner);

/**********************************************************************************************************************/
#ifdef AUDIO_INTERFACE

AudioConvert_F32toI16    toUSBR;         //xy=4267.500068664551,483.07141876220703
AudioConvert_F32toI16    toUSBL;         //xy=4268.500068664551,436.07141876220703
AudioConvert_I16toF32    fromUSBL;       //xy=4269.500068664551,387.07141876220703
AudioConvert_I16toF32    fromUSBR;       //xy=4274.500068664551,603.071418762207

AudioConnection_F32          patchCordAI1(cabSimMixerL, toUSBL);
AudioConnection_F32          patchCordAI2(cabSimMixerR, toUSBR);
AudioConnection_F32          patchCordAI3(fromUSBL, 0, outMixerL, 1);
AudioConnection_F32          patchCordAI4(fromUSBR, 0, outMixerR, 1);


AudioInputUSB            usbIn;
AudioOutputUSB           usbOut;
AudioConnection          patchCordUSB_IL(usbIn, 0, fromUSBL, 0);
AudioConnection          patchCordUSB_IR(usbIn, 1, fromUSBR, 0);
AudioConnection          patchCordUSB_OL(toUSBL, 0, usbOut, 0);
AudioConnection          patchCordUSB_OR(toUSBR, 0, usbOut, 1);

float lastUSBvolume = 0.0f;

#endif
/**********************************************************************************************************************/

const int chorusDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.050f;
DMAMEM float chorusDelayLineL[chorusDelaySamples];
DMAMEM float chorusDelayLineC[chorusDelaySamples];
DMAMEM float chorusDelayLineR[chorusDelaySamples];

const int reverbDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.200f;
float reverbDelayLine[reverbDelaySamples];

const int roomDelaySamplesL = AUDIO_SAMPLE_RATE_EXACT * 0.200f;
const int roomDelaySamplesR = AUDIO_SAMPLE_RATE_EXACT * 0.200f * 0.75f;
DMAMEM float roomDelayLineL[roomDelaySamplesL];
DMAMEM float roomDelayLineR[roomDelaySamplesR];

const int delaySamples = AUDIO_SAMPLE_RATE_EXACT * 1.000f;
DMAMEM float delayLineL[delaySamples];
DMAMEM float delayLineR[delaySamples];

float peak = 0.0f;
float levelInAvg = 0.0f;
float lastLevelIn = 0.0f;
elapsedMillis levelOutSince;
unsigned long levelOutFor = 0;


inline float unitToDb(float u)
{
  return u < 5.011872E-07f ? -126.0f : 20.0f * log10f(u);
}

void doTestTone(float freq = 1000.0f, float amp = 0.0f, float t = 0.0f)
{
  if (levelOutFor > 0) return;

  float cycleTime = 1.0f / freq;
  if (t < cycleTime) t = cycleTime;

  float blockTime = (float)AUDIO_BLOCK_SAMPLES / (float)AUDIO_SAMPLE_RATE_EXACT;
  if (t < blockTime) t = blockTime;

  while (!levelOutL.available() || !levelOutR.available()) yield();
  levelOutL.read();
  levelOutR.read();

  AudioNoInterrupts();
  inputMixer.gain(2, 1.0f);  
  testTone.frequency(freq);
  testTone.amplitude(amp);
  levelOutFor = (unsigned long)(t * 1000.0f) + 4;
  levelOutSince = 0;
  AudioInterrupts();
}

bool printTuner = false;
elapsedMillis lastPrintTuner;


bool printLevels = false;
elapsedMillis lastPrintLevels;
bool debug = false;

void readLevels()
{
  if (tuner.available())
  {
    float freq = tuner.read();
    
    if (levelInAvg > 0.018f && lastPrintTuner > 50)
    {
      lastPrintTuner = 0;

      float note = 12.0f * log2f(freq * (1.0f / 440.0f));
      bool valid = false;
      float cents = 0.0f;

      // -29.0f
      if (note > -30.0f && note < -28.0f)
      {
        valid = true;
        cents = 100.0f * (note + 29.0f);
      }
      // -24.0f
      else if (note > -25.0f && note < -23.0f)
      {
        valid = true;
        cents = 100.0f * (note + 24.0f);      
      }
      // -19.0f
      else if (note > -20.0f && note < -18.0f)
      {
        valid = true;
        cents = 100.0f * (note + 19.0f);      
      }
      // -14.0f
      else if (note > -15.0f && note < -13.0f)
      {
        valid = true;
        cents = 100.0f * (note + 14.0f);      
      }
      // -10.0f
      else if (note > -11.0f && note < -9.0f)
      {
        valid = true;
        cents = 100.0f * (note + 10.0f);      
      }
      //  -5.0f
      else if (note > -6.0f && note < -4.0f)
      {
        valid = true;
        cents = 100.0f * (note + 5.0f);      
      }

      if (valid && printTuner) Serial.printf("%.6f\n", cents);
    }
  }
  
  if (levelIn.available())
  {    
    lastLevelIn = levelIn.readPeakToPeak() * 0.5f;
    levelInAvg += (lastLevelIn - levelInAvg) * 0.05f;
    if (lastLevelIn > peak) peak = lastLevelIn;
    else peak *= 0.9f;

    if (printLevels && lastPrintLevels > 50)
    {
      lastPrintLevels = 0;
      Serial.printf("%.6f\t%.6f\n", unitToDb(levelInAvg), dynamics.effectiveGain());
    }

#ifdef AUDIO_INTERFACE
    float vol = usbIn.volume();
    if (vol != lastUSBvolume)
    {
      outMixerL.gain(1, vol);
      outMixerR.gain(1, vol);  
      lastUSBvolume = vol;
    }
#endif
  }
  if (levelOutFor > 0 && levelOutSince > levelOutFor && levelOutL.available() && levelOutR.available())
  {
    float l = levelOutL.read(); // levelOutL.readPeakToPeak() * 0.5f;
    float r = levelOutR.read(); // levelOutR.readPeakToPeak() * 0.5f;
    
    levelOutFor = 0;
    testTone.amplitude(0.0f);
    inputMixer.gain(2, 0.0f);  

    Serial.printf("%.7f,%.7f\r\n", l, r);
  }
}

void setPeak(AudioFilterBiquad_F32 &filter, uint32_t stage, float frequency, float gain, float q)
{
  double coeff[5];
  double a = pow(10.0, (double)gain / 40.0);
  double w0 = (double)frequency * (2.0 * 3.141592654 / (double)AUDIO_SAMPLE_RATE_EXACT);
  double sinW0 = sin(w0);
  double alpha = sinW0 / ((double)q * 2.0);
  double cosW0 = cos(w0);

  double a0 = 1.0 + (alpha / a);
  double scale = 1.0 / a0;
  
  /* b0 */ coeff[0] = 1.0 + (alpha * a);
  /* b1 */ coeff[1] = -2.0 * cosW0;
  /* b2 */ coeff[2] = 1.0 - (alpha * a);
  /* a1 */ coeff[3] = -2.0 * cosW0;
  /* a2 */ coeff[4] = 1 - (alpha / a);

  coeff[0] *= scale;
  coeff[1] *= scale;
  coeff[2] *= scale;
  coeff[3] *= -scale;
  coeff[4] *= -scale;

  filter.setCoefficients(stage, coeff); 
}

void muteMixer(AudioMixer4_F32 &mixer)
{
  mixer.gain(0, 0.0f);
  mixer.gain(1, 0.0f);
  mixer.gain(2, 0.0f);
  mixer.gain(3, 0.0f);
}

void setInput(float level = 1.0f)
{
  /*
  level = level < 0.0f ? 0.0f : level > 2.0f ? 2.0f : level;
#ifdef UX1
  inputMixer.gain(0, 0.0f);
  inputMixer.gain(1, level);
#else
  inputMixer.gain(0, level);
  inputMixer.gain(1, 0.0f);
#endif
*/
}

void setVolume(float volume = 1.0f)
{
  audioOut.setGain(volume);
  outMixerL.gain(0, 1.0f);
  outMixerR.gain(0, 1.0f);
}

/*******************************************************************************************************************/


void setup()
{
  AudioMemory(30);
  AudioMemory_F32(30);

  muteMixer(inputMixer);
  muteMixer(svModMixer);
  muteMixer(filterMixer);
  muteMixer(chorusInputL);
  muteMixer(chorusInputC);
  muteMixer(chorusInputR);
  muteMixer(chorusMixerL);
  muteMixer(chorusMixerR);
  muteMixer(delayFeedbackL);
  muteMixer(delayFeedbackR);
  muteMixer(delayMixerL);
  muteMixer(delayMixerR);
  muteMixer(reverbMixerL);
  muteMixer(reverbMixerR);
  muteMixer(cabSimMixerL);
  muteMixer(cabSimMixerL);
  muteMixer(outMixerL);
  muteMixer(outMixerR);

  detectorFilter.setHighpass(0, 80.0f, 0.7071f);
  detectorFilter.setLowShelf(1, 1000.0f, -4.5f, 0.3f);
  detectorFilter.setLowpass(2, 5000.0f, 0.5f);
  detectorFilter.begin();
  //dynamics.detector(AudioEffectDynamics_F32::DetectorType_RMS, 0.04f, 0.0f);

  distortion.begin();

  delayL.begin(delayLineL, delaySamples);
  delayR.begin(delayLineR, delaySamples);
  chorusModDelayL.begin(chorusDelayLineL, chorusDelaySamples);
  chorusModDelayC.begin(chorusDelayLineC, chorusDelaySamples);
  chorusModDelayR.begin(chorusDelayLineR, chorusDelaySamples);
  reverbPreDelay.begin(reverbDelayLine, reverbDelaySamples);
  roomDelayL.begin(roomDelayLineL, roomDelaySamplesL);
  roomDelayR.begin(roomDelayLineR, roomDelaySamplesR);

  tunerFilter.setHighpass(0, 50.0f, 0.7071f);
  tunerFilter.setHighpass(1, 50.0f, 0.7071f);
  tunerFilter.setLowpass(2, 500.0f, 0.7071f);
  tunerFilter.setLowpass(3, 500.0f, 0.7071f);
  tunerFilter.begin();
  tuner.begin(0.2f);

  testTone.begin();
  testTone.amplitude(0.0f);
  testTone.frequency(1000.0f);

  SD.begin(BUILTIN_SDCARD);

  HW_Setup();
  UI_Setup();
  initSercom();

  setInput();
  setVolume();
}

void loop() {
  readLevels();
  HW_Loop();
  UI_Loop();
  handleSercom();
}
