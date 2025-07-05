
#define UX1
#define USB_INPUT
#define USB_OUTPUT

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
AudioInputI2S_F32        audioIn;        //xy=63.33333206176758,177.99999618530273
AudioSynthWaveformSine_F32 testTone;       //xy=192.3333282470703,217
AudioEffectHDR_F32       hdr;            //xy=196.3333282470703,178
AudioMixer4_F32          inputMixer;     //xy=330.3333435058594,197
AudioAnalyzePeak_F32     levelIn;        //xy=474.3333320617676,261.99999618530273
AudioConvert_F32toI16    toTuner;        //xy=478.3333320617676,130.99999618530273
AudioEffectPreamp_F32    preamp;         //xy=481.3333435058594,196
AudioEffectEnvelope_F32  envelope;       //xy=613.3333320617676,195.99999618530273
AudioEffectAutoWah_F32   wah;            //xy=747.3333320617676,195.99999618530273
AudioEffectOtaComp_F32   ota;            //xy=875.3333129882812,196
AudioEffectDistortion_F32 distortion;     //xy=1005.3333129882812,196
AudioFilterToneStack_F32 toneStack;      //xy=1142.3333740234375,196
AudioEffectEqualizer_F32 equalizer;      //xy=1280.3333740234375,196
AudioEffectPhaser_F32    phaser;         //xy=1409.3333320617676,195.99999618530273
AudioEffectTremolo_F32   tremolo;        //xy=1530.3333320617676,195.99999618530273
AudioEffectMultiChorus_F32 chorus;         //xy=1652.3333320617676,195.99999618530273
AudioEffectStereoDelay_F32 stereodelay;    //xy=1792.3333320617676,195.99999618530273
AudioEffectMyVerb_F32    reverb;         //xy=1929.3333320617676,195.99999618530273
AudioEffectMultiband_F32 sonic;          //xy=2054.3333320617676,195.99999618530273
AudioEffectCabSim_F32    cabsim;         //xy=2178.3333320617676,195.99999618530273
AudioAnalyzePeak_F32     levelOutL;      //xy=2342.333251953125,99
AudioMixer4_F32          mixerR;       //xy=2346,244
AudioAnalyzePeak_F32     levelOutR;      //xy=2346.333251953125,318
AudioMixer4_F32          mixerL;       //xy=2347,172
AudioOutputI2S_F32       audioOut;       //xy=2530.333251953125,207
AudioConnection_F32          patchCord1(audioIn, 0, hdr, 0);
AudioConnection_F32          patchCord2(audioIn, 1, hdr, 1);
AudioConnection_F32          patchCord3(testTone, 0, inputMixer, 2);
AudioConnection_F32          patchCord4(hdr, 0, inputMixer, 0);
AudioConnection_F32          patchCord5(inputMixer, levelIn);
AudioConnection_F32          patchCord6(inputMixer, toTuner);
AudioConnection_F32          patchCord7(inputMixer, preamp);
AudioConnection_F32          patchCord8(preamp, envelope);
AudioConnection_F32          patchCord9(envelope, wah);
AudioConnection_F32          patchCord10(wah, ota);
AudioConnection_F32          patchCord11(ota, distortion);
AudioConnection_F32          patchCord12(distortion, toneStack);
AudioConnection_F32          patchCord13(toneStack, equalizer);
AudioConnection_F32          patchCord14(equalizer, phaser);
AudioConnection_F32          patchCord15(phaser, tremolo);
AudioConnection_F32          patchCord16(tremolo, chorus);
AudioConnection_F32          patchCord17(chorus, 0, stereodelay, 0);
AudioConnection_F32          patchCord18(chorus, 1, stereodelay, 1);
AudioConnection_F32          patchCord19(stereodelay, 0, reverb, 0);
AudioConnection_F32          patchCord20(stereodelay, 1, reverb, 1);
AudioConnection_F32          patchCord21(reverb, 0, sonic, 0);
AudioConnection_F32          patchCord22(reverb, 1, sonic, 1);
AudioConnection_F32          patchCord23(sonic, 0, cabsim, 0);
AudioConnection_F32          patchCord24(sonic, 1, cabsim, 1);
AudioConnection_F32          patchCord25(cabsim, 0, levelOutL, 0);
AudioConnection_F32          patchCord26(cabsim, 0, mixerL, 0);
AudioConnection_F32          patchCord27(cabsim, 1, levelOutR, 0);
AudioConnection_F32          patchCord28(cabsim, 1, mixerR, 0);
AudioConnection_F32          patchCord29(mixerR, 0, audioOut, 1);
AudioConnection_F32          patchCord30(mixerL, 0, audioOut, 0);
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
const int delaySamples = LQDelay::bufferSizeMs(delayMs);
DMAMEM int16_t delayLine[delaySamples];

float peak = 0.0f;
float levelInAvg = 0.0f;
float lastLevelIn = 0.0f;
elapsedMillis levelOutSince;
unsigned long levelOutFor = 0;

float filteredCents = 0.0f;

bool printTuner = false;
bool lastValidNote = -999;
elapsedMillis lastValidTuner;

bool printLevels = false;
elapsedMillis lastPrintLevels;
bool debug = false;

inline float unitToDb(float u)
{
  return u < 5.011872E-07f ? -126.0f : 20.0f * log10f(u);
}

FLASHMEM void tone(float freq = 1000.0f, float amp = 0.0f)
{
  AudioNoInterrupts();
  inputMixer.gain(2, 1.0f);
  testTone.frequency(freq);
  testTone.amplitude(amp);
  AudioInterrupts();
}

FLASHMEM void doTestTone(float freq = 1000.0f, float amp = 0.0f, float t = 0.0f)
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

FLASHMEM void readLevels()
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
    lastLevelIn = levelIn.readPeakToPeak() * 0.5f;
    levelInAvg += (lastLevelIn - levelInAvg) * 0.05f;
    if (lastLevelIn > peak) peak = lastLevelIn;
    else peak *= 0.9f;
  }
  
  if (levelOutFor > 0 && levelOutSince > levelOutFor && levelOutL.available() && levelOutR.available())
  {
    float l = levelOutL.read(); // levelOutL.readPeakToPeak() * 0.5f;
    float r = levelOutR.read(); // levelOutR.readPeakToPeak() * 0.5f;

    levelOutFor = 0;
    testTone.amplitude(0.0f);

    Serial.printf("%.7f,%.7f\n", l, r);
  }

  if (printLevels && lastPrintLevels > 25)
  {
    lastPrintLevels = 0;
    //Serial.printf("%.6f\t%.6f\n", unitToDb(levelInAvg), dynamics.effectiveGain());
    //Serial.printf("0\t10\t%.6f\n", chorus._mod1 * 10.0f);
    Serial.printf("min:-0\tmax:1\ts:%.6f\tf:%.6f\tg:%.6f\n", envelope.surge(), envelope.level(), envelope.gate());
    //Serial.printf("min:-16\tmax:2\ts:%.6f\n", envelope.surge());
    //Serial.printf("%.6f\t%.6f\t%.6f\n", hdr.db_low(), hdr.db_high(), hdr.gain());
    //Serial.printf("%.6f\t%.6f\n", wah._control, wah._smooth);
  }
}

FLASHMEM void usbVolume()
{
#ifdef AUDIO_INTERFACE
#ifdef USB_OUTPUT
  float volume = usbIn.volume();
  mixerL.gain(1, volume);
  mixerR.gain(1, volume);
#endif
#endif
}

FLASHMEM void setVolume(float volume = 1.0f)
{
  mixerL.gain(0, volume);
  mixerR.gain(0, volume);
}

/*******************************************************************************************************************/


FLASHMEM void setup()
{
  AudioMemory(30);
  AudioMemory_F32(10);

  preamp.begin();
  distortion.begin();
  chorus.begin();
  stereodelay.begin(delayLine, delaySamples);
  reverb.begin();
  cabsim.begin();

  tuner.begin(0.2f);

  testTone.begin();
  testTone.amplitude(0.0f);
  testTone.frequency(1000.0f);

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
