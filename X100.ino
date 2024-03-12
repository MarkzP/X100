#include "effect_simple_delay_F32.h"
#include "effect_tremolo_F32.h"
#include "filter_state_variable_F32.h"
#include "filter_tonestack_F32.h"
#include "effect_phaser_F32.h"
#include "hexefx_audio_F32.h"
#include "effect_dynamics_F32.h"              // https://github.com/MarkzP/AudioEffectDynamics_F32
#include "effect_modulated_delay_F32.h"       // https://github.com/MarkzP/ModulatedDelay_F32
#include "effect_freeverb_F32.h"              // https://github.com/MarkzP/Freeverb_F32
#include "effect_nonlinear_F32.h"             // https://github.com/MarkzP/NonLinear_F32
#include "synth_lfo_F32.h"

/**********************************************************************************************************************/

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S_F32        audioIn;        //xy=62,166
AudioSynthWaveformSine_F32 testTone;       //xy=62,204
AudioMixer4_F32          inputMixer;     //xy=228,179
AudioAnalyzePeak_F32     levelIn;        //xy=367,224
AudioFilterBiquad_F32    preampEq;       //xy=377,178
AudioEffectDynamics_F32  dynamics;       //xy=520,185
AudioSynthLfo_F32        filterLfo;      //xy=524,230
AudioMixer4_F32          svModMixer;     //xy=659,237
AudioFilterStateVariable_F32 svFilter;       //xy=796,210
AudioMixer4_F32          filterMixer;    //xy=927,200
AudioEffectNonLinear_F32 distortion;     //xy=1066.0000610351562,200
AudioFilterToneStack_F32 toneStack; //xy=1213.0000305175781,200.00000190734863
AudioSynthLfo_F32        tremoloLfo;     //xy=1348,245
AudioEffectPhaser_F32    phaser;         //xy=1352.0000305175781,200.00000190734863
AudioEffectTremolo_F32   tremoloVca;     //xy=1491.0000305175781,206.00000190734863
AudioMixer4_F32          chorusInputL;   //xy=1775.0000305175781,122.00000190734863
AudioSynthLfo_F32        chorusLfoL;     //xy=1780.0000305175781,175.00000190734863
AudioMixer4_F32          chorusInputC;   //xy=1785.0000305175781,225.00000190734863
AudioSynthLfo_F32        chorusLfoC;     //xy=1787.0000305175781,279.00000190734863
AudioMixer4_F32          chorusInputR;   //xy=1791.0000305175781,336.00000190734863
AudioSynthLfo_F32        chorusLfoR;     //xy=1798.0000305175781,390.00000190734863
AudioEffectModulatedDelay_F32 chorusModDelayL; //xy=1952.0000305175781,129.00000190734863
AudioEffectModulatedDelay_F32 chorusModDelayC; //xy=1963.0000305175781,232.00000190734863
AudioEffectModulatedDelay_F32 chorusModDelayR; //xy=1975.0000305175781,343.00000190734863
AudioFilterBiquad_F32    chorusPostFilterL; //xy=2149.000030517578,173.00000190734863
AudioFilterBiquad_F32    chorusPostFilterR; //xy=2150.000030517578,379.00000190734863
AudioFilterBiquad_F32    chorusPostFilterC; //xy=2156.000030517578,267.00000190734863
AudioMixer4_F32          chorusMixerL;   //xy=2468.000030517578,205.00000190734863
AudioMixer4_F32          chorusMixerR;   //xy=2482.000030517578,353.00000190734863
AudioMixer4_F32          delayFeedbackL; //xy=2687.000030517578,242.00000190734863
AudioMixer4_F32          delayFeedbackR; //xy=2688.000030517578,315.00000190734863
AudioEffectSimpleDelay_F32 delayL;         //xy=2843,242
AudioEffectSimpleDelay_F32 delayR;         //xy=2847,315
AudioFilterBiquad_F32    delayFilterL;   //xy=2984,242
AudioFilterBiquad_F32    delayFilterR;   //xy=2988,315
AudioMixer4_F32          delayMixerL;    //xy=3156,224
AudioMixer4_F32          delayMixerR;    //xy=3158,371
//AudioEffectPlateReverb_F32 plateReverb;    //xy=3325,304
AudioMixer4_F32          reverbMixerL;   //xy=3487,243
AudioMixer4_F32          reverbMixerR;   //xy=3501,389
AudioFilterBiquad_F32    cabSimL;        //xy=3633,243
AudioFilterBiquad_F32    cabSimR;        //xy=3644.000030517578,389.00000190734863
AudioMixer4_F32          roomMixer;      //xy=3787,323
AudioFilterBiquad_F32    roomFilter;     //xy=3927,323
AudioEffectFreeverbStereo_F32 roomReverb;     //xy=4071,323
AudioEffectSimpleDelay_F32 roomDelayR;     //xy=4218,361
AudioEffectSimpleDelay_F32 roomDelayL;     //xy=4219,288
AudioMixer4_F32          cabSimMixerL;   //xy=4383,263
AudioMixer4_F32          cabSimMixerR;   //xy=4388,406
AudioConvert_F32toI16    toUSBR;         //xy=4553,363
AudioConvert_F32toI16    toUSBL;         //xy=4554,316
AudioAnalyzePeak_F32     levelOutR;      //xy=4554,429
AudioConvert_I16toF32    fromUSBL;       //xy=4555,267
AudioAnalyzePeak_F32     levelOutL;      //xy=4559,203
AudioConvert_I16toF32    fromUSBR;       //xy=4560,483
AudioMixer4_F32          outMixerL;      //xy=4700,269
AudioMixer4_F32          outMixerR;      //xy=4709,464
AudioOutputI2S_F32       audioOut;       //xy=4858.999969482422,364.99999809265137
AudioConnection_F32          patchCord1(audioIn, 0, inputMixer, 0);
AudioConnection_F32          patchCord2(audioIn, 1, inputMixer, 1);
AudioConnection_F32          patchCord3(testTone, 0, inputMixer, 2);
AudioConnection_F32          patchCord4(inputMixer, levelIn);
AudioConnection_F32          patchCord5(inputMixer, preampEq);
AudioConnection_F32          patchCord6(preampEq, 0, dynamics, 0);
AudioConnection_F32          patchCord7(dynamics, 0, svFilter, 0);
AudioConnection_F32          patchCord8(dynamics, 0, filterMixer, 0);
AudioConnection_F32          patchCord9(dynamics, 1, svModMixer, 0);
AudioConnection_F32          patchCord10(filterLfo, 0, svModMixer, 1);
AudioConnection_F32          patchCord11(svModMixer, 0, svFilter, 1);
AudioConnection_F32          patchCord12(svFilter, 0, filterMixer, 1);
AudioConnection_F32          patchCord13(svFilter, 1, filterMixer, 2);
AudioConnection_F32          patchCord14(svFilter, 2, filterMixer, 3);
AudioConnection_F32          patchCord15(filterMixer, distortion);
AudioConnection_F32          patchCord16(distortion, toneStack);
AudioConnection_F32          patchCord17(toneStack, phaser);
AudioConnection_F32          patchCord18(tremoloLfo, 0, tremoloVca, 1);
AudioConnection_F32          patchCord19(phaser, 0, tremoloVca, 0);
AudioConnection_F32          patchCord20(tremoloVca, 0, chorusInputL, 0);
AudioConnection_F32          patchCord21(tremoloVca, 0, chorusInputC, 0);
AudioConnection_F32          patchCord22(tremoloVca, 0, chorusInputR, 0);
AudioConnection_F32          patchCord23(tremoloVca, 0, chorusMixerL, 0);
AudioConnection_F32          patchCord24(tremoloVca, 0, chorusMixerR, 0);
AudioConnection_F32          patchCord25(chorusInputL, 0, chorusModDelayL, 0);
AudioConnection_F32          patchCord26(chorusLfoL, 0, chorusModDelayL, 1);
AudioConnection_F32          patchCord27(chorusInputC, 0, chorusModDelayC, 0);
AudioConnection_F32          patchCord28(chorusLfoC, 0, chorusModDelayC, 1);
AudioConnection_F32          patchCord29(chorusInputR, 0, chorusModDelayR, 0);
AudioConnection_F32          patchCord30(chorusLfoR, 0, chorusModDelayR, 1);
AudioConnection_F32          patchCord31(chorusModDelayL, chorusPostFilterL);
AudioConnection_F32          patchCord32(chorusModDelayC, chorusPostFilterC);
AudioConnection_F32          patchCord33(chorusModDelayR, chorusPostFilterR);
AudioConnection_F32          patchCord34(chorusPostFilterL, 0, chorusMixerL, 1);
AudioConnection_F32          patchCord35(chorusPostFilterL, 0, chorusMixerR, 1);
AudioConnection_F32          patchCord36(chorusPostFilterL, 0, chorusInputL, 1);
AudioConnection_F32          patchCord37(chorusPostFilterR, 0, chorusMixerL, 3);
AudioConnection_F32          patchCord38(chorusPostFilterR, 0, chorusMixerR, 3);
AudioConnection_F32          patchCord39(chorusPostFilterR, 0, chorusInputR, 1);
AudioConnection_F32          patchCord40(chorusPostFilterC, 0, chorusMixerL, 2);
AudioConnection_F32          patchCord41(chorusPostFilterC, 0, chorusMixerR, 2);
AudioConnection_F32          patchCord42(chorusPostFilterC, 0, chorusInputC, 1);
AudioConnection_F32          patchCord43(chorusPostFilterC, 0, chorusInputL, 2);
AudioConnection_F32          patchCord44(chorusPostFilterC, 0, chorusInputR, 2);
AudioConnection_F32          patchCord45(chorusMixerL, 0, delayMixerL, 0);
AudioConnection_F32          patchCord46(chorusMixerL, 0, delayFeedbackL, 0);
AudioConnection_F32          patchCord47(chorusMixerR, 0, delayMixerR, 0);
AudioConnection_F32          patchCord48(chorusMixerR, 0, delayFeedbackR, 0);
AudioConnection_F32          patchCord49(delayFeedbackL, delayL);
AudioConnection_F32          patchCord50(delayFeedbackR, delayR);
AudioConnection_F32          patchCord51(delayL, delayFilterL);
AudioConnection_F32          patchCord52(delayR, delayFilterR);
AudioConnection_F32          patchCord53(delayFilterL, 0, delayMixerL, 1);
AudioConnection_F32          patchCord54(delayFilterL, 0, delayFeedbackL, 1);
AudioConnection_F32          patchCord55(delayFilterL, 0, delayFeedbackR, 2);
AudioConnection_F32          patchCord56(delayFilterR, 0, delayMixerR, 1);
AudioConnection_F32          patchCord57(delayFilterR, 0, delayFeedbackR, 1);
AudioConnection_F32          patchCord58(delayFilterR, 0, delayFeedbackL, 2);
AudioConnection_F32          patchCord59(delayMixerL, 0, reverbMixerL, 0);
//AudioConnection_F32          patchCord60(delayMixerL, 0, plateReverb, 0);
AudioConnection_F32          patchCord61(delayMixerR, 0, reverbMixerR, 0);
//AudioConnection_F32          patchCord62(delayMixerR, 0, plateReverb, 1);
//AudioConnection_F32          patchCord63(plateReverb, 0, reverbMixerL, 1);
//AudioConnection_F32          patchCord64(plateReverb, 1, reverbMixerR, 1);
AudioConnection_F32          patchCord65(reverbMixerL, cabSimL);
AudioConnection_F32          patchCord66(reverbMixerR, cabSimR);
AudioConnection_F32          patchCord67(cabSimL, 0, cabSimMixerL, 0);
AudioConnection_F32          patchCord68(cabSimL, 0, roomMixer, 0);
AudioConnection_F32          patchCord69(cabSimR, 0, cabSimMixerR, 0);
AudioConnection_F32          patchCord70(cabSimR, 0, roomMixer, 1);
AudioConnection_F32          patchCord71(roomMixer, roomFilter);
AudioConnection_F32          patchCord72(roomFilter, roomReverb);
AudioConnection_F32          patchCord73(roomReverb, 0, roomDelayL, 0);
AudioConnection_F32          patchCord74(roomReverb, 1, roomDelayR, 0);
AudioConnection_F32          patchCord75(roomDelayR, 0, cabSimMixerR, 1);
AudioConnection_F32          patchCord76(roomDelayL, 0, cabSimMixerL, 1);
AudioConnection_F32          patchCord77(cabSimMixerL, levelOutL);
AudioConnection_F32          patchCord78(cabSimMixerL, toUSBL);
AudioConnection_F32          patchCord79(cabSimMixerL, 0, outMixerL, 0);
AudioConnection_F32          patchCord80(cabSimMixerR, levelOutR);
AudioConnection_F32          patchCord81(cabSimMixerR, toUSBR);
AudioConnection_F32          patchCord82(cabSimMixerR, 0, outMixerR, 0);
AudioConnection_F32          patchCord83(fromUSBL, 0, outMixerL, 1);
AudioConnection_F32          patchCord84(fromUSBR, 0, outMixerR, 1);
AudioConnection_F32          patchCord85(outMixerL, 0, audioOut, 0);
AudioConnection_F32          patchCord86(outMixerR, 0, audioOut, 1);
// GUItool: end automatically generated code



/**********************************************************************************************************************/
#ifdef AUDIO_INTERFACE
AudioInputUSB            usbIn;
AudioOutputUSB           usbOut;
AudioConnection          patchCordUSB_IL(usbIn, 0, fromUSBL, 0);
AudioConnection          patchCordUSB_IR(usbIn, 1, fromUSBR, 0);
AudioConnection          patchCordUSB_OL(toUSBL, 0, usbOut, 0);
AudioConnection          patchCordUSB_OR(toUSBR, 0, usbOut, 1);
#endif

/**********************************************************************************************************************/

const int chorusDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.050f;
DMAMEM float chorusDelayLineL[chorusDelaySamples];
DMAMEM float chorusDelayLineC[chorusDelaySamples];
DMAMEM float chorusDelayLineR[chorusDelaySamples];

const int roomDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.200f;
DMAMEM float roomDelayLineL[roomDelaySamples];
DMAMEM float roomDelayLineR[roomDelaySamples];

const int delaySamples = AUDIO_SAMPLE_RATE_EXACT * 1.000f;
DMAMEM float delayLineL[delaySamples];
DMAMEM float delayLineR[delaySamples];

float lastUSBvolume = 0.0f;

float peak = 0.0f;
float lastLevelIn = 0.0f;
float lastLevelOutL = 0.0f;
float lastLevelOutR = 0.0f;
int levelOutRequests = 0;
int levelOutCurrent = 0;

void doTestTone(float freq = 1000.0f, float amp = 0.0f)
{
  if (levelOutRequests > 0) return;

  AudioNoInterrupts();
  if (levelOutL.available()) levelOutL.read();
  if (levelOutR.available()) levelOutR.read();
  lastLevelOutL = 0.0f;
  lastLevelOutR = 0.0f;
  testTone.frequency(freq);
  testTone.amplitude(amp);
  levelOutCurrent = 0;
  levelOutRequests = 2 + (int)((1.0f/freq)*(AUDIO_SAMPLE_RATE_EXACT/(float)128));
  AudioInterrupts();
}

bool printCompGain = false;

void readLevels()
{
  if (levelIn.available())
  {
    if (printCompGain) Serial.println(dynamics.effectiveGain(), 4);
    
    lastLevelIn = levelIn.read();
    if (lastLevelIn > peak) peak = lastLevelIn;
    else peak *= 0.9f;

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
  if (levelOutRequests > 0 && levelOutL.available() && levelOutR.available())
  {
    if (levelOutCurrent++ == 0)
    {
      levelOutL.read();
      levelOutR.read();
    }
    else
    {
      lastLevelOutL = max(lastLevelOutL, levelOutL.read());
      lastLevelOutR = max(lastLevelOutR, levelOutR.read());
      if (--levelOutRequests == 0)
      {
        levelOutRequests = 0;
        testTone.amplitude(0.0f);
        Serial.printf("%.7f,%.7f\r\n", lastLevelOutL, lastLevelOutR);
      }
    }
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

/*******************************************************************************************************************/




void setVolume(float volume = 1.0f)
{
  audioOut.setGain(volume);
}

void setup()
{
  AudioMemory(10);
  AudioMemory_F32(50);

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
  muteMixer(roomMixer);  
  muteMixer(cabSimMixerL);
  muteMixer(cabSimMixerL);
  muteMixer(outMixerL);
  muteMixer(outMixerR);

  distortion.begin();
  dynamics.detector(AudioEffectDynamics_F32::DetectorType_RMS, 0.04f, 0.0f);

  delayL.begin(delayLineL, delaySamples);
  delayR.begin(delayLineR, delaySamples);
  chorusModDelayL.begin(chorusDelayLineL, chorusDelaySamples);
  chorusModDelayC.begin(chorusDelayLineC, chorusDelaySamples);
  chorusModDelayR.begin(chorusDelayLineR, chorusDelaySamples);
  roomDelayL.begin(roomDelayLineL, roomDelaySamples);
  roomDelayR.begin(roomDelayLineR, roomDelaySamples);

  testTone.begin();
  testTone.amplitude(0.0f);
  testTone.frequency(1000.0f);

  SD.begin(BUILTIN_SDCARD);

  HW_Setup();
  UI_Setup();
  initSercom();

  inputMixer.gain(0, 0.0f);
  inputMixer.gain(1, 1.0f);
  inputMixer.gain(1, 1.0f);
  inputMixer.gain(2, 1.0f);

  outMixerL.gain(0, 1.0f);
  outMixerR.gain(0, 1.0f);
}

void loop() {
  readLevels();
  HW_Loop();
  UI_Loop();
  handleSercom();
}
