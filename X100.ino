

#include "BALibrary.h"
using namespace BALibrary;

#include "effect_dynamics_F32.h"
#include "effect_modulated_delay_F32.h"
#include "effect_freeverb_F32.h"
#include "effect_nonlinear_f32.h"

// AudioEffectDynamics_F32
// AudioEffectFreeverbStereo_F32
// AudioEffectModulatedDelay_F32
// AudioEffectNonLinear_F32
// AudioEffectDelay_OA_F32

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S_F32        audioIn;    //xy=81,49
AudioFilterBiquad_F32    preEq;          //xy=85,173
AudioEffectDynamics_F32 compressor;     //xy=223,173
AudioEffectNonLinear_F32      distortion;          //xy=363,125
AudioFilterBiquad_F32    EQ2;        //xy=492,174
AudioFilterEqualizer_F32 EQ1; //xy=493,125
AudioMixer4_F32          mixerMode;      //xy=646,155
AudioSynthWaveform_F32   chorusMod;      //xy=805,105
AudioFilterBiquad_F32    chorusPreFilter; //xy=809,50
AudioFilterBiquad_F32    reverbPreFilter;        //xy=839,293
AudioEffectDelay_OA_F32     chorusPostDelay;         //xy=982,240
AudioEffectFreeverbStereo_F32      reverb;    //xy=989,293
AudioEffectModulatedDelay_F32    chorusModDelay; //xy=993,56
AudioFilterBiquad_F32    chorusPostFilter; //xy=1172,56
AudioMixer4_F32          mixerL;         //xy=1175,172
AudioMixer4_F32          mixerR;         //xy=1175,247
AudioOutputI2S_F32       audioOut;   //xy=1312,207
AudioConnection_F32          patchCord1(audioIn, 0, preEq, 0);
AudioConnection_F32          patchCord2(preEq, compressor);
AudioConnection_F32          patchCord3(compressor, distortion);
AudioConnection_F32          patchCord4(compressor, EQ2);
AudioConnection_F32          patchCord5(distortion, EQ1);
AudioConnection_F32          patchCord6(EQ2, 0, mixerMode, 2);
AudioConnection_F32          patchCord7(EQ1, 0, mixerMode, 1);
AudioConnection_F32          patchCord8(mixerMode, 0, mixerL, 0);
AudioConnection_F32          patchCord9(mixerMode, 0, mixerR, 0);
AudioConnection_F32          patchCord10(mixerMode, chorusPreFilter);
AudioConnection_F32          patchCord11(mixerMode, reverbPreFilter);
AudioConnection_F32          patchCord12(chorusMod, 0, chorusModDelay, 1);
AudioConnection_F32          patchCord13(chorusPreFilter, 0, chorusModDelay, 0);
AudioConnection_F32          patchCord14(reverbPreFilter, reverb);
AudioConnection_F32          patchCord15(chorusPostDelay, 0, mixerR, 1);
AudioConnection_F32          patchCord16(reverb, 0, mixerL, 2);
AudioConnection_F32          patchCord17(reverb, 1, mixerR, 2);
AudioConnection_F32          patchCord18(chorusModDelay, chorusPostFilter);
AudioConnection_F32          patchCord19(chorusPostFilter, 0, mixerL, 1);
AudioConnection_F32          patchCord20(chorusPostFilter, chorusPostDelay);
AudioConnection_F32          patchCord21(mixerL, 0, audioOut, 0);
AudioConnection_F32          patchCord22(mixerR, 0, audioOut, 1);
// GUItool: end automatically generated code


typedef enum
{
  X100_mute   = 0,
  X100_Dist   = 1,
  X100_Edge   = 2,
  X100_Cln1 = 3,
  X100_Cln2 = 4,
  
} X100_Channels;

const int eq1n = 8;
float eq1f[] = { 100.0f, 200.0f, 350.0f, 800.0f, 1500.0f, 3000.0f, 6000.0f, 12000.0f };
float eq1g[] = {  -6.0f,  -3.0f,  -1.5f,  -9.0f,  -12.0f,    1.5f,  -9.0f,   -36.0f };
float eq1c[200];

const int chorusDelaySamples = 1024;
float chorusDelayLine[chorusDelaySamples];

BAAudioControlWM8731 codec;
// regArray[WM8731_REG_INTERFACE] = 0b00001010;
// write(WM8731_REG_INTERFACE, regArray[WM8731_REG_INTERFACE]); // I2S, 24 bit, MCLK slave


BAPhysicalControls controls(0, 3, 0, 0);

float chorusGain = 0.750f;
float reverbGain = 0.016f;
float mainGain =   1.000f;
X100_Channels channel = X100_mute;


void setChannel(X100_Channels ch)
{
  if (ch == channel) return;
  
  AudioNoInterrupts(); 
  distortion.gain(ch == X100_Dist ? 25.0f
                : ch == X100_Edge ? 2.5f
                : 1.0f);
  distortion.curve(ch == X100_Dist ? 3.5f
                :  ch == X100_Edge ? 3.8f
                :  ch == X100_Cln1 ? 1.0f
                :  0.1f);
  mixerMode.gain(1, ch == X100_Dist ? 0.18f
                  : ch == X100_Edge ? 0.32f
                  : ch == X100_Cln1 ? 1.00f
                  : 0.0f);
  mixerMode.gain(2, ch == X100_Cln2 ? 1.0f : 0.0f);
  AudioInterrupts();

  Serial.print("Channel "); Serial.println(ch);
  channel = ch;
}

void setup()
{
  TGA_PRO_MKII_REV1();

  AudioMemory(5);
  AudioMemory_F32(50);
  
  preEq.setHighpass(0, 200.0f, 0.7071f);
  preEq.setLowShelf(1, 1000.0f, -30.0f, 0.2f);
  preEq.setLowpass(2, 2000.0f, 1.7f);
  preEq.begin();

  compressor.gate(-75.0f, 0.0f, 5.0f, 6.0f, -12.0f);
  compressor.compression(-55.0f, 0.0015f, 2.5f, 4.5f);

  EQ1.equalizerNew(eq1n, &eq1f[0], &eq1g[0], 30, &eq1c[0], 60.0f);

  EQ2.setLowShelf(0, 300.0f, -9.0f, 0.5f);
  EQ2.setHighShelf(1, 3500.0f, 6.0f, 0.5f);
  EQ2.setLowpass(2, 11000.0f, 0.5f);
  EQ2.begin();

  mixerMode.gain(0, 0.0f);
  mixerMode.gain(1, 0.0f);
  mixerMode.gain(2, 0.0f);
  mixerMode.gain(3, 0.0f);

  chorusPreFilter.setNotch(0, 6500.0f, 0.7f);
  chorusPreFilter.setLowpass(1, 8000.0f, 0.7071f);
  chorusPreFilter.begin();
  chorusModDelay.begin(chorusDelayLine, chorusDelaySamples);
  chorusMod.begin(0.032f, 1.7f, WAVEFORM_TRIANGLE);
  chorusPostFilter.setLowpass(0, 8000.0f, 0.5f);
  chorusPostFilter.begin();
  chorusPostDelay.delay(0, 17);
  mixerR.gain(1, -chorusGain);
  mixerL.gain(1, chorusGain);

  reverbPreFilter.setLowpass(1, 11000.0f, 0.3f);
  reverbPreFilter.begin();
  reverb.roomsize(0.9f);
  reverb.damping(0.001f);
  mixerR.gain(2, reverbGain);
  mixerL.gain(2, reverbGain);
 
  mixerR.gain(3, 0.0f);
  mixerL.gain(3, 0.0f);
  
  delay(500); // allow the WM7831 extra time to power up
  codec.enable();

  delay(500);

  codec.setHeadphoneVolume(0.8f);

  // Create the controls using the calib values
  controls.addPot(BA_EXPAND_POT1_PIN, 0, 1023, false);
  controls.addPot(BA_EXPAND_POT2_PIN, 0, 1023, false);
  controls.addPot(BA_EXPAND_POT3_PIN, 0, 1023, false);

  setChannel(X100_Cln1);
}

void loop()
{
  float value;
    
  if (controls.checkPotValue(0, value))
  {
    if (value < 0.20) setChannel(X100_Cln2);
    if (value > 0.25 && value < 0.45) setChannel(X100_Cln1);
    if (value > 0.50 && value < 0.70) setChannel(X100_Edge);
    if (value > 0.75) { setChannel(X100_Dist); }
  }

  delay(20);
}
