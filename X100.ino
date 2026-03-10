
#define UX1
#define USB_INPUT
#define USB_OUTPUT

#include "analyze_level_F32.h"
#include "effect_autowah_F32.h"
#include "effect_cabsim_F32.h"
#include "effect_distortion_F32.h"
#include "effect_equalizer_F32.h"
#include "effect_hdr_F32.h"
#include "effect_multiband_F32.h"
#include "effect_myverb_F32.h"
#include "effect_preamp_F32.h"
#include "effect_stereo_delay_F32.h"
#include "effect_multichorus_F32.h"
#include "effect_tremolo_F32.h"
#include "effect_phaser_F32.h"
#include "filter_tonestack_F32.h"
#include "effect_otacomp_F32.h"
#include "effect_envelope_F32.h"

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
AudioInputI2S_F32        audioIn;        //xy=63,188
AudioSynthWaveformSine_F32 testTone;       //xy=133,353
AudioEffectHDR_F32       hdr;            //xy=196,189
AudioMixer4_F32          inputMixer;     //xy=330,208
AudioAnalyzeLevel_F32    levelIn;        //xy=474,272
AudioConvert_F32toI16    toTuner;        //xy=478,141
AudioEffectPreamp_F32    preamp;         //xy=481,207
AudioEffectEnvelope_F32  envelope;       //xy=613,206
AudioEffectAutoWah_F32   wah;            //xy=747,206
AudioEffectOtaComp_F32   ota;            //xy=875,207
AudioEffectDistortion_F32 distortion;     //xy=1005,207
AudioFilterToneStack_F32 toneStack;      //xy=1142,207
AudioEffectEqualizer_F32 equalizer;      //xy=1280,207
AudioEffectPhaser_F32    phaser;         //xy=1409,206
AudioEffectTremolo_F32   tremolo;        //xy=1530,206
AudioEffectMultiChorus_F32 chorus;         //xy=1652,206
AudioEffectStereoDelay_F32 stereodelay;    //xy=1804.8787460327148,206.30298137664795
AudioEffectMyVerb_F32    reverb;         //xy=1955.13627243042,205.92427825927734
AudioEffectCabSim_F32    cabsim;         //xy=2100.2728385925293,205.99999809265137
AudioEffectMultiband_F32 sonic;          //xy=2229.151523590088,205.83334159851074
AudioAnalyzeLevel_F32    levelOutL;      //xy=2393.818183898926,110
AudioMixer4_F32          outputMixerR;         //xy=2397.818183898926,255
AudioAnalyzeLevel_F32    levelOutR;      //xy=2397.818183898926,329
AudioMixer4_F32          outputMixerL;         //xy=2398.818183898926,183
AudioOutputI2S_F32       audioOut;       //xy=2581.818183898926,218
AudioConnection_F32          patchCord1(audioIn, 0, hdr, 0);
AudioConnection_F32          patchCord2(audioIn, 1, hdr, 1);
AudioConnection_F32          patchCord3(testTone, 0, inputMixer, 2);
AudioConnection_F32          patchCord4(testTone, 0, outputMixerL, 2);
AudioConnection_F32          patchCord5(testTone, 0, outputMixerR, 2);
AudioConnection_F32          patchCord6(hdr, 0, inputMixer, 0);
AudioConnection_F32          patchCord7(inputMixer, levelIn);
AudioConnection_F32          patchCord8(inputMixer, toTuner);
AudioConnection_F32          patchCord9(inputMixer, preamp);
AudioConnection_F32          patchCord10(preamp, envelope);
AudioConnection_F32          patchCord11(envelope, wah);
AudioConnection_F32          patchCord12(wah, ota);
AudioConnection_F32          patchCord13(ota, distortion);
AudioConnection_F32          patchCord14(distortion, toneStack);
AudioConnection_F32          patchCord15(toneStack, equalizer);
AudioConnection_F32          patchCord16(equalizer, phaser);
AudioConnection_F32          patchCord17(phaser, tremolo);
AudioConnection_F32          patchCord18(tremolo, chorus);
AudioConnection_F32          patchCord19(chorus, 0, stereodelay, 0);
AudioConnection_F32          patchCord20(chorus, 1, stereodelay, 1);
AudioConnection_F32          patchCord21(stereodelay, 0, reverb, 0);
AudioConnection_F32          patchCord22(stereodelay, 1, reverb, 1);
AudioConnection_F32          patchCord23(reverb, 0, cabsim, 0);
AudioConnection_F32          patchCord24(reverb, 1, cabsim, 1);
AudioConnection_F32          patchCord25(cabsim, 0, sonic, 0);
AudioConnection_F32          patchCord26(cabsim, 1, sonic, 1);
AudioConnection_F32          patchCord27(sonic, 0, outputMixerL, 0);
AudioConnection_F32          patchCord28(sonic, 0, levelOutL, 0);
AudioConnection_F32          patchCord29(sonic, 1, outputMixerR, 0);
AudioConnection_F32          patchCord30(sonic, 1, levelOutR, 0);
AudioConnection_F32          patchCord31(outputMixerR, 0, audioOut, 1);
AudioConnection_F32          patchCord32(outputMixerL, 0, audioOut, 0);
// GUItool: end automatically generated code

/**********************************************************************************************************************/

AudioAnalyzeNoteFrequency tuner;
AudioConnection          patchCordTuner1(toTuner, tuner);

/**********************************************************************************************************************/

#ifdef AUDIO_INTERFACE

#ifdef USB_INPUT
AudioConvert_F32toI16    toUSBL;
AudioConvert_F32toI16    toUSBR;
AudioOutputUSB           usbOut;
AudioConnection_F32      patchCordAI1(audioIn, 0, toUSBL, 0);
AudioConnection_F32      patchCordAI2(audioIn, 1, toUSBR, 0);
AudioConnection          patchCordUSB_OL(toUSBL, 0, usbOut, 0);
AudioConnection          patchCordUSB_OR(toUSBR, 0, usbOut, 1);
#endif

#ifdef USB_OUTPUT
AudioInputUSB            usbIn;
AudioConvert_I16toF32    fromUSBL;
AudioConvert_I16toF32    fromUSBR;
AudioConnection          patchCordUSB_IL_16(usbIn, 0, fromUSBL, 0);
AudioConnection          patchCordUSB_IR_16(usbIn, 1, fromUSBR, 0);
AudioConnection_F32      patchCordUSB_IL_32(fromUSBL, 0, mixerL, 1);
AudioConnection_F32      patchCordUSB_IR_32(fromUSBR, 0, mixerR, 1);
#endif

#endif
/**********************************************************************************************************************/


const float delayMs = 1000.0f;
const int delaySamples = Delay::bufferSizeMs(delayMs);
DMAMEM float delayLineL[delaySamples];
DMAMEM float delayLineR[delaySamples];

float levelInPeak = 0.0f;
float levelInAvg = 0.0f;
float levelOutAvgL = 0.0f;
float levelOutAvgR = 0.0f;
float levelOutPeakL = 0.0f;
float levelOutPeakR = 0.0f;

uint32_t levelOutCount = 0;

float filteredCents = 0.0f;

bool printTuner = false;
bool lastValidNote = -999;
elapsedMillis lastValidTuner;

bool printLevels = false;
elapsedMillis lastPrintLevels;
bool debug = false;


FLASHMEM void inputTone(float freq = 1000.0f, float amp = 0.0f)
{
  inputMixer.gain(2, 2.0f);
  outputMixerL.gain(2, 0.0f);
  outputMixerR.gain(2, 0.0f);
  testTone.frequency(freq);
  testTone.amplitude(amp);
  testTone.begin();
}

FLASHMEM void outputTone(float freq = 1000.0f, float amp = 0.0f)
{
  inputMixer.gain(2, 0.0f);
  outputMixerL.gain(2, 1.0f);
  outputMixerR.gain(2, 1.0f);
  testTone.frequency(freq);
  testTone.amplitude(amp);
  testTone.begin();
}

FLASHMEM void doTestTone(float freq = 1000.0f, float amp = 0.0f, float t = 0.0f)
{
  testTone.end();

  float cycleTime = 1.0f / freq;
  if (t < cycleTime) t = cycleTime;

  float blockTime = (float)AUDIO_BLOCK_SAMPLES / (float)AUDIO_SAMPLE_RATE_EXACT;
  float blockCount = t / blockTime;  

  inputMixer.gain(2, 2.0f);
  outputMixerL.gain(2, 0.0f);
  outputMixerR.gain(2, 0.0f);

  testTone.frequency(freq);
  testTone.amplitude(amp);
  testTone.phase(0.0f);
  testTone.begin();

  levelOutCount = 2 + (int)blockCount;
}

void readLevels()
{
  if (tuner.available())
  {
    float freq = tuner.read();
    if (printTuner) Serial.printf("%.6f\n", freq);

    if (levelInAvg > 0.015f)
    {
      float semitones = 12.0f * log2f(freq * (1.0f / 440.0f));
      int roundSemitones = lroundf(semitones);
      float cents = (semitones - (float)roundSemitones) * 100.0f;
      int wholeNote = ((roundSemitones % 12) + 12) % 12;
      
      lastValidTuner = 0;
      if (abs(roundSemitones - lastValidNote) > 2)
      {
        filteredCents = cents;
        lastValidNote = roundSemitones;
      }
      else filteredCents += (cents - filteredCents) * 0.00005f;

      displayTuner(freq, wholeNote, roundSemitones, filteredCents);
    }
  }

  if (lastValidTuner > 500)
  {
    filteredCents = 0.0f;
    lastValidNote = -999;
  }

  if (levelIn.available())
  {
    float thisLevelIn = levelIn.read();
    levelInAvg += (thisLevelIn - levelInAvg) * 0.05f;
    if (thisLevelIn > levelInPeak) levelInPeak = thisLevelIn;
    else levelInPeak *= 0.9f;
  }

  if (levelOutCount > 0)
  {
    uint32_t countL = levelOutL.getCount();
    uint32_t countR = levelOutR.getCount();
    if (countL >= levelOutCount && countR >= levelOutCount)
    {
      testTone.end();
      levelOutCount = 0;
      Serial.printf("%.7f,%.7f,%u,%u\n", levelOutL.read(), levelOutR.read(), countL, countR);
    }
  }
  else
  {
    if (levelOutL.available())
    {
      float thisLevelOutL = levelOutL.read();
      levelOutAvgL += (thisLevelOutL - levelOutAvgL) * 0.05f;
      if (thisLevelOutL > levelOutPeakL) levelOutPeakL = thisLevelOutL;
      else levelOutPeakL *= 0.9f;
    }
    if (levelOutR.available())
    {
      float thisLevelOutR = levelOutR.read();
      levelOutAvgR += (thisLevelOutR - levelOutAvgR) * 0.05f;
      if (thisLevelOutR > levelOutPeakR) levelOutPeakR = thisLevelOutR;
      else levelOutPeakR *= 0.9f;
    }
  }

  if (printLevels && lastPrintLevels > 25)
  {
    lastPrintLevels = 0;
    Serial.printf("%.6f\t%.6f\t%.6f\n", FilterUtils::u2dB(levelInAvg), FilterUtils::u2dB(levelOutAvgL), FilterUtils::u2dB(levelOutAvgR));
    //Serial.printf("0\t10\t%.6f\n", chorus._mod1 * 10.0f);
    //Serial.printf("min:-0\tmax:1\ts:%.6f\tf:%.6f\tg:%.6f\n", envelope.surge(), envelope.level(), envelope.gate());
    //Serial.printf("min:-16\tmax:2\ts:%.6f\n", envelope.surge());
    //Serial.printf("%.6f\t%.6f\t%.6f\n", hdr.db_low(), hdr.db_high(), hdr.gain());
    //Serial.printf("%.6f\t%.6f\n", wah._control, wah._smooth);
  }
}

void usbVolume()
{
#ifdef AUDIO_INTERFACE
#ifdef USB_OUTPUT
  float volume = usbIn.volume();
  outputMixerL.gain(1, volume);
  outputMixerR.gain(1, volume);
#endif
#endif
}

void setVolume(float volume = 1.0f)
{
  outputMixerL.gain(0, volume);
  outputMixerR.gain(0, volume);
}

/*******************************************************************************************************************/


FLASHMEM void setup()
{
  AudioMemory(32);
  AudioMemory_F32(16);

  preamp.begin();
  distortion.begin();
  chorus.begin();
  stereodelay.begin(delayLineL, delayLineR, delaySamples);
  reverb.begin();
  cabsim.begin();

  tuner.begin(0.2f);

  SD.begin(BUILTIN_SDCARD);

  HW_Setup();
  UI_Setup();
  initSercom();
  setVolume();
}

void loop() {
  readLevels();
  usbVolume();
  HW_Loop();
  UI_Loop();
  handleSercom();
}
