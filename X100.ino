
#define UX1
//#define TGA

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

#include "effect_dynamics_F32.h"              // https://github.com/MarkzP/AudioEffectDynamics_F32

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
AudioInputI2S_F32        audioIn;        //xy=63,138
AudioSynthWaveformSine_F32 testTone;       //xy=184,177
AudioEffectHDR_F32       hdr;            //xy=185,138
AudioMixer4_F32          inputMixer;     //xy=318,157
AudioEffectPreamp_F32    preamp;         //xy=464,156
AudioAnalyzePeak_F32     levelIn;        //xy=474,222
AudioConvert_F32toI16    toTuner;        //xy=478,91
AudioEffectDynamics_F32  dynamics;       //xy=592,163
AudioEffectAutoWah_F32   wah;            //xy=720,156
AudioEffectDistortion_F32 distortion;     //xy=846,156
AudioEffectEqualizer_F32 equalizer;      //xy=983,156
AudioFilterToneStack_F32 toneStack;      //xy=1117,156
AudioEffectPhaser_F32    phaser;         //xy=1243,156
AudioEffectTremolo_F32   tremolo;        //xy=1364,156
AudioEffectMultiChorus_F32 chorus;         //xy=1486,156
AudioEffectStereoDelay_F32 stereodelay;    //xy=1626,156
AudioEffectMyVerb_F32    reverb;         //xy=1763,156
AudioEffectMultiband_F32 sonic;          //xy=1888,156
AudioEffectCabSim_F32    cabsim;         //xy=2012,156
AudioAnalyzePeak_F32     levelOutR;      //xy=2143,220
AudioAnalyzePeak_F32     levelOutL;      //xy=2154,90
AudioOutputI2S_F32       audioOut;       //xy=2157,156
AudioConnection_F32          patchCord1(audioIn, 0, hdr, 0);
AudioConnection_F32          patchCord2(audioIn, 1, hdr, 1);
AudioConnection_F32          patchCord3(testTone, 0, inputMixer, 2);
AudioConnection_F32          patchCord4(hdr, 0, inputMixer, 0);
AudioConnection_F32          patchCord5(inputMixer, levelIn);
AudioConnection_F32          patchCord6(inputMixer, preamp);
AudioConnection_F32          patchCord7(inputMixer, toTuner);
AudioConnection_F32          patchCord8(preamp, 0, dynamics, 0);
AudioConnection_F32          patchCord9(dynamics, 0, wah, 0);
AudioConnection_F32          patchCord10(wah, distortion);
AudioConnection_F32          patchCord11(distortion, equalizer);
AudioConnection_F32          patchCord12(equalizer, toneStack);
AudioConnection_F32          patchCord13(toneStack, phaser);
AudioConnection_F32          patchCord14(phaser, tremolo);
AudioConnection_F32          patchCord15(tremolo, chorus);
AudioConnection_F32          patchCord16(chorus, 0, stereodelay, 0);
AudioConnection_F32          patchCord17(chorus, 1, stereodelay, 1);
AudioConnection_F32          patchCord18(stereodelay, 0, reverb, 0);
AudioConnection_F32          patchCord19(stereodelay, 1, reverb, 1);
AudioConnection_F32          patchCord20(reverb, 0, sonic, 0);
AudioConnection_F32          patchCord21(reverb, 1, sonic, 1);
AudioConnection_F32          patchCord22(sonic, 0, cabsim, 0);
AudioConnection_F32          patchCord23(sonic, 1, cabsim, 1);
AudioConnection_F32          patchCord24(cabsim, 0, audioOut, 0);
AudioConnection_F32          patchCord25(cabsim, 0, levelOutL, 0);
AudioConnection_F32          patchCord26(cabsim, 1, audioOut, 1);
AudioConnection_F32          patchCord27(cabsim, 1, levelOutR, 0);
// GUItool: end automatically generated code


/**********************************************************************************************************************/

AudioAnalyzeNoteFrequency tuner;
AudioConnection          patchCordTuner1(toTuner, tuner);

/**********************************************************************************************************************/

#ifdef AUDIO_INTERFACE

AudioConvert_F32toI16    toUSBL;
AudioConvert_F32toI16    toUSBR;

AudioConnection_F32      patchCordAI1(cabsim, 0, toUSBL, 0);
AudioConnection_F32      patchCordAI2(cabsim, 1, toUSBR, 0);

AudioOutputUSB           usbOut;
AudioConnection          patchCordUSB_OL(toUSBL, 0, usbOut, 0);
AudioConnection          patchCordUSB_OR(toUSBR, 0, usbOut, 1);

#endif
/**********************************************************************************************************************/

const float delayMs = 1000.0f;
const int delaySamples = LQModDelay::bufferSize(delayMs);
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

void tone(float freq = 1000.0f, float amp = 0.0f)
{
  AudioNoInterrupts();
  inputMixer.gain(2, 1.0f);
  testTone.frequency(freq);
  testTone.amplitude(amp);
  AudioInterrupts();
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

void readLevels()
{
  if (tuner.available())
  {
    float freq = tuner.read();
    if (printTuner) Serial.printf("%.6f\n", freq);

    if (levelInAvg > 0.018f)
    {
      float semitones = 12.0f * log2f(freq * (1.0f / 440.0f));
      bool valid = false;

      int roundSemitones = lroundf(semitones);
      float cents = (semitones - (float)roundSemitones) * 100.0f;
      int wholeNote = ((roundSemitones % 12) + 12) % 12;
      switch (wholeNote)
      {
        case 0:  // A
        case 2:  // B
        case 5:  // D
        case 7:  // E
        case 10: // G
          valid = true;
          break;
      }

      if (valid)
      {
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

    if (printLevels && lastPrintLevels > 25)
    {
      lastPrintLevels = 0;
      //Serial.printf("%.6f\t%.6f\n", unitToDb(levelInAvg), dynamics.effectiveGain());
      //Serial.printf("0\t10\t%.6f\n", chorus._mod1 * 10.0f);
      //Serial.printf("%.6f\t%.6f\n", hdr._gain, hdr._error);
      //Serial.printf("%.6f\t%.6f\n", wah._control, wah._smooth);
    }
  }
  
  if (levelOutFor > 0 && levelOutSince > levelOutFor && levelOutL.available() && levelOutR.available())
  {
    float l = levelOutL.read(); // levelOutL.readPeakToPeak() * 0.5f;
    float r = levelOutR.read(); // levelOutR.readPeakToPeak() * 0.5f;

    levelOutFor = 0;
    testTone.amplitude(0.0f);

    Serial.printf("%.7f,%.7f\n", l, r);
  }
}


FLASHMEM void setVolume(float volume = 1.0f)
{
  audioOut.setGain(volume);
}

/*******************************************************************************************************************/


FLASHMEM void setup()
{
  AudioMemory(30);
  AudioMemory_F32(10);

  distortion.begin();
  chorus.begin();
  stereodelay.begin(delayLine, delaySamples);

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
  HW_Loop();
  UI_Loop();
  handleSercom();
}
