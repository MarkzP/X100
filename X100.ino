#include <WS2812Serial.h>
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
AudioInputI2S_F32        audioIn;        //xy=80,105
AudioFilterBiquad_F32    preEqInstrument; //xy=108,164
AudioFilterBiquad_F32    preMike;        //xy=134,32
AudioMixer4_F32          mixerLevel;       //xy=232,103
AudioEffectDynamics_F32 compMike;       //xy=277,32
AudioEffectDynamics_F32 compInstrument; //xy=281,164
AudioAnalyzePeak_F32     levelIn;          //xy=391,75
AudioEffectNonLinear_F32      distortion;     //xy=407,116
AudioFilterEqualizer_F32 EQ1;            //xy=532,116
AudioFilterBiquad_F32    EQ2;            //xy=532,165
AudioMixer4_F32          mixerMode;      //xy=682,146
AudioMixer4_F32          reverbMixer;    //xy=701,309
AudioMixer4_F32          delayMixer;     //xy=706,389
AudioSynthWaveform_F32   chorusMod;      //xy=825,96
AudioFilterBiquad_F32    chorusPreFilter; //xy=829,41
AudioFilterBiquad_F32    reverbPreFilter; //xy=863,309
AudioFilterBiquad_F32    delayFilter;    //xy=889,389
AudioEffectDelay_OA_F32     chorusPostDelay; //xy=996,271
AudioRecordQueue_F32     lfoLevel;   //xy=1010,95
AudioEffectModulatedDelay_F32    chorusModDelay; //xy=1013,47
AudioEffectFreeverbStereo_F32      reverb;         //xy=1012,309
AudioEffectDelay_OA_F32     delayLine;      //xy=1029,389
AudioMixer8_F32          mixerR;         //xy=1191,289
AudioFilterBiquad_F32    chorusPostFilter; //xy=1193,47
AudioMixer8_F32          mixerL;         //xy=1195,163
AudioOutputI2S_F32       audioOut;       //xy=1332,198
AudioConnection_F32          patchCord1(audioIn, 0, preMike, 0);
AudioConnection_F32          patchCord2(audioIn, 0, mixerLevel, 0);
AudioConnection_F32          patchCord3(audioIn, 1, preEqInstrument, 0);
AudioConnection_F32          patchCord4(audioIn, 1, mixerLevel, 1);
AudioConnection_F32          patchCord5(preEqInstrument, compInstrument);
AudioConnection_F32          patchCord6(preMike, compMike);
AudioConnection_F32          patchCord7(mixerLevel, levelIn);
AudioConnection_F32          patchCord8(compMike, 0, mixerL, 1);
AudioConnection_F32          patchCord9(compMike, 0, mixerR, 1);
AudioConnection_F32          patchCord10(compMike, 0, reverbMixer, 1);
AudioConnection_F32          patchCord11(compMike, 0, delayMixer, 1);
AudioConnection_F32          patchCord12(compInstrument, distortion);
AudioConnection_F32          patchCord13(compInstrument, EQ2);
AudioConnection_F32          patchCord14(distortion, EQ1);
AudioConnection_F32          patchCord15(EQ1, 0, mixerMode, 1);
AudioConnection_F32          patchCord16(EQ2, 0, mixerMode, 2);
AudioConnection_F32          patchCord17(mixerMode, 0, mixerL, 0);
AudioConnection_F32          patchCord18(mixerMode, 0, mixerR, 0);
AudioConnection_F32          patchCord19(mixerMode, chorusPreFilter);
AudioConnection_F32          patchCord20(mixerMode, 0, delayMixer, 0);
AudioConnection_F32          patchCord21(mixerMode, 0, reverbMixer, 0);
AudioConnection_F32          patchCord22(reverbMixer, reverbPreFilter);
AudioConnection_F32          patchCord23(delayMixer, delayFilter);
AudioConnection_F32          patchCord24(chorusMod, 0, chorusModDelay, 1);
AudioConnection_F32          patchCord25(chorusMod, lfoLevel);
AudioConnection_F32          patchCord26(chorusPreFilter, 0, chorusModDelay, 0);
AudioConnection_F32          patchCord27(reverbPreFilter, reverb);
AudioConnection_F32          patchCord28(delayFilter, delayLine);
AudioConnection_F32          patchCord29(chorusPostDelay, 0, mixerR, 2);
AudioConnection_F32          patchCord30(chorusModDelay, chorusPostFilter);
AudioConnection_F32          patchCord31(reverb, 0, mixerL, 3);
AudioConnection_F32          patchCord32(reverb, 1, mixerR, 3);
AudioConnection_F32          patchCord33(delayLine, 0, delayMixer, 3);
AudioConnection_F32          patchCord34(delayLine, 0, mixerR, 4);
AudioConnection_F32          patchCord35(delayLine, 0, mixerL, 4);
AudioConnection_F32          patchCord36(mixerR, 0, audioOut, 1);
AudioConnection_F32          patchCord37(chorusPostFilter, chorusPostDelay);
AudioConnection_F32          patchCord38(chorusPostFilter, 0, mixerL, 2);
AudioConnection_F32          patchCord39(mixerL, 0, audioOut, 0);
// GUItool: end automatically generated code



const int numled = 1;
const int pin = 17;
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED
WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_RGB);

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

#define PIN_AK4528_VT   9
#define PIN_AK4528_PDN  22
#define PIN_DIST        2
#define PIN_EDGE        3
#define PIN_CLN1        5
#define PIN_CLN2        4

elapsedMillis hold_Dist;
elapsedMillis hold_Edge;
elapsedMillis hold_Cln1;
elapsedMillis hold_Cln2;

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

void setMike(bool enable)
{
  mixerR.gain(1, enable ? mikeGain : 0.0f);
  mixerL.gain(1, enable ? mikeGain : 0.0f);
}

void setChorus(bool enable)
{
  mixerR.gain(2, enable ? -chorusGain : 0.0f);
  mixerL.gain(2, enable ? chorusGain : 0.0f);
}

void setReverb(bool enable)
{
  mixerR.gain(3, enable ? reverbGain : 0.0f);
  mixerL.gain(3, enable ? reverbGain : 0.0f);
}

void setDelay(bool enable)
{
  mixerR.gain(4, enable ? delayGain : 0.0f);
  mixerL.gain(4, enable ? -delayGain : 0.0f);
}

void setChannel(X100_Channels ch)
{
  if (ch == channel) return;
  
  AudioNoInterrupts();
  mixerMode.gain(1, 0.0f);
  mixerMode.gain(2, 0.0f);
  AudioInterrupts();
  delay(20);
  
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

  //Serial.print("Channel "); Serial.println(ch);
  channel = ch;
}

void setup()
{
  leds.begin();
  leds.setBrightness(255);
  
  AudioMemory(5);
  AudioMemory_F32(192);
  
  preEqInstrument.setHighpass(0, 200.0f, 0.7071f);
  preEqInstrument.setLowShelf(1, 1000.0f, -30.0f, 0.2f);
  preEqInstrument.setLowpass(2, 2000.0f, 1.7f);
  preEqInstrument.begin();

  compInstrument.gate(-75.0f, 0.0f, 5.0f, 6.0f, -12.0f);
  compInstrument.compression(-55.0f, 0.0015f, 2.5f, 4.5f);

  EQ1.equalizerNew(eq1n, &eq1f[0], &eq1g[0], 30, &eq1c[0], 60.0f);

  EQ2.setLowShelf(0, 300.0f, -9.0f, 0.5f);
  EQ2.setHighShelf(1, 3500.0f, 6.0f, 0.5f);
  EQ2.setLowpass(2, 11000.0f, 0.7071f);
  EQ2.begin();

  mixerMode.gain(0, 0.0f);
  mixerMode.gain(1, 0.0f);
  mixerMode.gain(2, 0.0f);
  mixerMode.gain(3, 0.0f);

  preMike.setHighpass(0, 200.0f, 0.7071f);
  preMike.setLowpass(1, 8000.0f, 0.7071f);
  preMike.begin();

  compMike.gate(-75.0f, 0.0f, 5.0f, 6.0f, -12.0f);
  compMike.compression(-55.0f, 0.0015f, 2.5f, 4.5f);

  chorusPreFilter.setNotch(0, 6500.0f, 0.7f);
  chorusPreFilter.setLowpass(1, 8000.0f, 0.7071f);
  chorusPreFilter.begin();
  chorusModDelay.begin(chorusDelayLine, chorusDelaySamples);
  chorusMod.begin(0.032f, 1.7f, WAVEFORM_TRIANGLE);
  chorusPostFilter.setLowpass(0, 8000.0f, 0.5f);
  chorusPostFilter.begin();
  chorusPostDelay.delay(0, 17);
  setChorus(true);

  reverbPreFilter.setLowpass(1, 11000.0f, 0.3f);
  reverbPreFilter.begin();
  reverb.roomsize(0.9f);
  reverb.damping(0.001f);
  setReverb(true);

  delayLine.delay(0, 333);
  delayFilter.setHighpass(0, 500.0f, 0.7071f);
  delayFilter.setLowpass(1, 3000.0f, 0.7071f);
  delayFilter.begin();
  delayMixer.gain(0, 0.7f);
  delayMixer.gain(1, 1.0f);
  delayMixer.gain(3, 0.3f);
  setDelay(false);

  setInstrument(true);
  setMike(false);
  
  mixerR.gain(5, 0.0f);
  mixerL.gain(5, 0.0f);
  mixerR.gain(6, 0.0f);
  mixerL.gain(6, 0.0f);
  mixerR.gain(7, 0.0f);
  mixerL.gain(7, 0.0f);

  pinMode(PIN_AK4528_PDN, OUTPUT);
  digitalWriteFast(PIN_AK4528_PDN, LOW);
  delay(100);
  pinMode(PIN_AK4528_VT, OUTPUT);
  digitalWriteFast(PIN_AK4528_VT, HIGH);
  delay(500);
  digitalWriteFast(PIN_AK4528_PDN, HIGH);
  delay(100);
  
  pinMode(PIN_DIST, INPUT_PULLUP);
  pinMode(PIN_EDGE, INPUT_PULLUP);
  pinMode(PIN_CLN1, INPUT_PULLUP);
  pinMode(PIN_CLN2, INPUT_PULLUP);

  setChannel(X100_Cln1);

  lfoLevel.begin();
}

void loop()
{
  if (lfoLevel.available() && levelIn.available())
  {
    float* lfoData = lfoLevel.readBuffer();
    int lfo = map(lfoData[0], -0.035f, 0.035f, 2.0f, 15.0f);
    lfoLevel.freeBuffer();
    lfoLevel.clear();

    float lin = levelIn.read();
    int level = map(lin, 0.0f, 1.0f, 0.0f, 200.0f);

    int red = max(lfo, (channel == X100_Dist || channel == X100_Cln1 ? level : 0));
    int green = channel == X100_Cln1 || channel == X100_Edge ? level : 0;
    int blue = channel == X100_Cln2 ? level : 0;
    
    leds.setPixel(0, leds.Color(green, red, blue));
    leds.show();

    /*
    Serial.print("-126\t0\t");
    Serial.print(lin, 4);
    Serial.print("\t");
    Serial.print(compInstrument.readCurrentGain(), 4);
    Serial.print("\t");
    Serial.println(compInstrument.readDetector(), 4);
    */
  }
  
  if (digitalReadFast(PIN_DIST)) 
  {
    if (hold_Dist > 50 && hold_Dist < 500) setChannel(X100_Dist);
    else if (hold_Dist > 1000) { setReverb(false); }
    hold_Dist = 0;
  }

  if (digitalReadFast(PIN_EDGE))
  {
    if (hold_Edge > 50 && hold_Edge < 500) setChannel(X100_Edge);
    else if (hold_Edge > 1000 && hold_Cln1 == 0) { setDelay(true); }
    else if (hold_Edge > 1000 && hold_Cln1 > 1000) { setInstrument(false); hold_Cln1 = 0; }
    hold_Edge = 0;
  }

  if (digitalReadFast(PIN_CLN1))
  {
    if (hold_Cln1 > 50 && hold_Cln1 < 500) setChannel(X100_Cln1);
    else if (hold_Cln1 > 1000 && hold_Edge == 0) { setInstrument(true); setDelay(false); setReverb(true); setChorus(true); }
    else if (hold_Edge > 1000 && hold_Cln1 > 1000) { setInstrument(false); hold_Edge = 0; }
    hold_Cln1 = 0;
  }

  if (digitalReadFast(PIN_CLN2))
  {
    if (hold_Cln2 > 50 && hold_Cln2 < 500) setChannel(X100_Cln2);
    else if (hold_Cln2 > 1000) { setChorus(false); }
    hold_Cln2 = 0;
  }
}
