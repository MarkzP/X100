#include "effect_dynamics_F32.h"              // https://github.com/MarkzP/AudioEffectDynamics_F32
#include "effect_modulated_delay_F32.h"       // https://github.com/MarkzP/ModulatedDelay_F32
#include "effect_freeverb_F32.h"              // https://github.com/MarkzP/Freeverb_F32
#include "effect_nonlinear_F32.h"             // https://github.com/MarkzP/NonLinear_F32
#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S_F32        audioIn;        //xy=60,40
AudioFilterBiquad_F32    preEq;          //xy=161,138
AudioMixer4_F32          inputMixer;     //xy=195,53
AudioEffectDynamics_F32  dynamics;       //xy=293,144
AudioAnalyzePeak_F32     inputLevel;     //xy=383,55
AudioEffectNonLinear_F32 distortion;     //xy=429,144
AudioSynthWaveform_F32   chorusLfo;      //xy=556,58
AudioFilterEqualizer_F32 postEq;         //xy=557,144
AudioMixer4_F32          delayMixer;     //xy=727,283
AudioEffectModulatedDelay_F32 chorusModDelay; //xy=738,106
AudioFilterBiquad_F32    chorusPostFilter; //xy=917,106
AudioEffectDelay_OA_F32  reverbPreDelay; //xy=923,239
AudioFilterBiquad_F32    delayFilter;    //xy=929,326
AudioEffectFreeverbStereo_F32 reverb;         //xy=1096,240
AudioEffectDelay_OA_F32  delayLine;      //xy=1097,327
AudioEffectDelay_OA_F32  chorusPostDelay; //xy=1105,106
AudioMixer4_F32          mixerL;         //xy=1297,194
AudioMixer4_F32          mixerR;         //xy=1299,334
AudioOutputI2S_F32       audioOut;       //xy=1439,231
AudioConnection_F32          patchCord1(audioIn, 0, inputMixer, 0);
AudioConnection_F32          patchCord2(audioIn, 1, inputMixer, 1);
AudioConnection_F32          patchCord3(preEq, 0, dynamics, 0);
AudioConnection_F32          patchCord4(inputMixer, inputLevel);
AudioConnection_F32          patchCord5(inputMixer, preEq);
AudioConnection_F32          patchCord6(dynamics, distortion);
AudioConnection_F32          patchCord7(distortion, postEq);
AudioConnection_F32          patchCord8(chorusLfo, 0, chorusModDelay, 1);
AudioConnection_F32          patchCord10(postEq, 0, mixerL, 0);
AudioConnection_F32          patchCord11(postEq, 0, mixerR, 0);
AudioConnection_F32          patchCord12(postEq, 0, delayMixer, 0);
AudioConnection_F32          patchCord13(postEq, reverbPreDelay);
AudioConnection_F32          patchCord14(postEq, 0, chorusModDelay, 0);
AudioConnection_F32          patchCord15(delayMixer, delayFilter);
AudioConnection_F32          patchCord16(chorusModDelay, chorusPostFilter);
AudioConnection_F32          patchCord17(chorusPostFilter, chorusPostDelay);
AudioConnection_F32          patchCord18(chorusPostFilter, 0, mixerL, 1);
AudioConnection_F32          patchCord19(reverbPreDelay, reverb);
AudioConnection_F32          patchCord20(delayFilter, delayLine);
AudioConnection_F32          patchCord21(reverb, 0, mixerL, 2);
AudioConnection_F32          patchCord22(reverb, 1, mixerR, 2);
AudioConnection_F32          patchCord23(delayLine, 0, mixerR, 3);
AudioConnection_F32          patchCord24(delayLine, 0, mixerL, 3);
AudioConnection_F32          patchCord25(delayLine, 0, delayMixer, 1);
AudioConnection_F32          patchCord26(chorusPostDelay, 0, mixerR, 1);
AudioConnection_F32          patchCord27(mixerL, 0, audioOut, 0);
AudioConnection_F32          patchCord28(mixerR, 0, audioOut, 1);
// GUItool: end automatically generated code

typedef enum
{
  X100_mute   = 0,
  X100_Dist   = 1,
  X100_Edge   = 2,
  X100_Cln1 = 3,
  X100_Cln2 = 4,
  
} X100_Channels;

const int posteqnf = 7;
float posteqf[]  = {  100.0f,      200.0f,      400.0f,      800.0f,      1600.0f,      3200.0f,      6400.0f };
float eq1g[]     = {   -6.0f,       -3.0f,       -1.5f,       -9.0f,       -12.0f,         1.5f,        -9.0f };
float eq2g[]     = {   -9.0f,       -6.0f,        0.5f,        0.0f,         0.0f,         4.5f,         6.0f };
float posteqg[]  = {    0.0f,        0.0f,        0.0f,        0.0f,         0.0f,         0.0f,         0.0f };
const int posteqnc = 33;
float posteqc[200];
float posteqsl = 60.0f;

const int chorusDelaySamples = 1024;
float chorusDelayLine[chorusDelaySamples];

float instrumentGain =   1.000f;
float mikeGain = 0.000f;
float chorusGain = 0.750f;
float reverbGain = 0.018f;
float delayGain = 0.175f;

X100_Channels channel = X100_mute;

void setInstrument(bool enable)
{
  mixerR.gain(0, enable ? instrumentGain : 0.0f);
  mixerL.gain(0, enable ? instrumentGain : 0.0f);
}

void setChorus(bool enable)
{
  if (enable) {
    chorusLfo.begin(0.032f, 1.7f, WAVEFORM_TRIANGLE);
    chorusModDelay.begin(chorusDelayLine, chorusDelaySamples);
    chorusPostFilter.setNotch(0, 6500.0f, 0.7f);
    chorusPostFilter.setLowpass(1, 8000.0f, 0.5f);
    chorusPostFilter.begin();
    chorusPostDelay.delay(0, 17.0f);
  }

  mixerR.gain(1, enable ? -chorusGain : 0.0f);
  mixerL.gain(1, enable ? chorusGain : 0.0f);
}

void setReverb(bool enable)
{
  if (enable) {
    reverb.roomsize(0.9f);
    reverb.damping(0.001f);
    reverbPreDelay.delay(0, 15.0f);
  }
  
  mixerR.gain(2, enable ? reverbGain : 0.0f);
  mixerL.gain(2, enable ? reverbGain : 0.0f);
}

void setDelay(bool enable)
{
  if (enable) {
    delayMixer.gain(0, 0.7f);
    delayMixer.gain(1, 0.3f);
    delayLine.delay(0, 300);
    delayFilter.setHighpass(0, 500.0f, 0.7071f);
    delayFilter.setLowpass(1, 3000.0f, 0.7071f);
    delayFilter.begin();
  }
  
  mixerR.gain(3, enable ? delayGain : 0.0f);
  mixerL.gain(3, enable ? -delayGain : 0.0f);
}

void setChannel(X100_Channels ch)
{
  if (ch == channel) return;

  inputMixer.gain(0, 0.0f);
  inputMixer.gain(1, 0.0f);
  delay(20);

  preEq.setHighpass(0, 200.0f, 0.7071f);
  preEq.setLowShelf(1, 1000.0f, -18.0f, 0.2f);
  preEq.setLowpass(2, 2400.0f, 1.7f);
  preEq.begin();

  dynamics.detector(AudioEffectDynamics_F32::DetectorType_DiodeBridge, 0.002f, 0.0f);
  dynamics.gate(-75.0f, 0.0f, 3.0f, 6.0f, -14.0f);
  dynamics.compression(-55.0f, 0.0015f, 2.5f, 4.5f);
  
  distortion.gain(ch == X100_Dist ?  25.0f
                  : ch == X100_Edge ? 2.5f
								                    : 1.0f,
				          ch == X100_Dist ? 0.18f
                  : ch == X100_Edge ? 0.32f
                                  : 1.0f);

  distortion.curve(ch == X100_Dist ? 3.5f
                :  ch == X100_Edge ? 3.8f
                :  ch == X100_Cln1 ? 1.0f
                :  0.0f);
				
  postEq.equalizerNew(posteqnf, &posteqf[0], ch == X100_Cln2 ? &eq2g[0] : &eq1g[0], posteqnc, &posteqc[0], posteqsl);			
  
  delay(20);
  inputMixer.gain(0, ch == X100_mute ? 0.0f : -1.0f);
  inputMixer.gain(1, ch == X100_mute ? 0.0f : 1.0f);
  channel = ch;
}

float inputLevelDb = 0.0f;
float inputLevelAvg = 0.0f;
elapsedMillis reportInput;


inline float unit2db(float u)
{
  if (u < 5.001554864521944e-7f) return -126.0f;
  union { float f; uint32_t i; } vx = { u };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;
  y -= 126.94269504f;

  return 6.02f * y;
}

void setup()
{
  AudioMemory(5);
  AudioMemory_F32(192);
  
  setChorus(true);
  setReverb(true);
  setDelay(false);
  setInstrument(true);
  HW_Setup();
  initSercom();
  setChannel(X100_Cln1);
}


void loop() {
  if (inputLevel.available())
  {
    float inputUnit = fabsf(inputLevel.read());
    inputLevelDb = unit2db(inputUnit);    
    inputLevelAvg += (inputLevelDb - inputLevelAvg) * 0.01f;
  }

  if (reportInput > 50) {
    reportInput = 0;
    Serial.print("-126,0,");
    Serial.println(inputLevelAvg, 5);
  }
  
  HW_Loop();
  handleSercom();
}
