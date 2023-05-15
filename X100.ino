#include "effect_simple_delay_F32.h"
#include "effect_tremolo_F32.h"
#include "filter_state_variable_F32.h"
#include "effect_dynamics_F32.h"              // https://github.com/MarkzP/AudioEffectDynamics_F32
#include "effect_modulated_delay_F32.h"       // https://github.com/MarkzP/ModulatedDelay_F32
#include "effect_freeverb_F32.h"              // https://github.com/MarkzP/Freeverb_F32
#include "effect_nonlinear_F32.h"             // https://github.com/MarkzP/NonLinear_F32
#include "synth_lfo_F32.h"

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S_F32        audioIn;        //xy=95,93
AudioSynthWaveformSine_F32 testTone;       //xy=97,187
AudioSynthNoiseWhite_F32 testNoise;      //xy=99,142
AudioMixer4_F32          inputMixer;     //xy=250,106
AudioAnalyzePeak_F32     levelIn;        //xy=411,190
AudioFilterBiquad_F32    preampEq;       //xy=420,107
AudioEffectDynamics_F32  dynamics;       //xy=570,113
AudioEffectGain_F32      wahSensitivity; //xy=745,170
AudioFilterStateVariable_F32 wahFilter;      //xy=921,163
AudioMixer4_F32          wahMixer;       //xy=1081,130
AudioFilterBiquad_F32    distPre;        //xy=1226,130
AudioEffectNonLinear_F32 distortion;     //xy=1373,130
AudioSynthWaveform_F32   tremoloLfo;     //xy=1507,197
AudioFilterBiquad_F32    distPost;       //xy=1513,130
AudioEffectTremolo_F32   tremoloVca;     //xy=1656,136
AudioFilterBiquad_F32    chorusInputFilter; //xy=1850,199
AudioMixer4_F32          chorusInputL;   //xy=2176,101
AudioSynthLfo_F32        chorusLfoL;     //xy=2181,154
AudioMixer4_F32          chorusInputC;   //xy=2186,204
AudioSynthLfo_F32        chorusLfoC;     //xy=2188,258
AudioMixer4_F32          chorusInputR;   //xy=2192,315
AudioSynthLfo_F32        chorusLfoR;     //xy=2199,369
AudioEffectModulatedDelay_F32 chorusModDelayL; //xy=2355,130
AudioEffectModulatedDelay_F32 chorusModDelayC; //xy=2367,224
AudioEffectModulatedDelay_F32 chorusModDelayR; //xy=2382,337
AudioFilterBiquad_F32    chorusPostFilterL; //xy=2546,131
AudioFilterBiquad_F32    chorusPostFilterC; //xy=2563,223
AudioFilterBiquad_F32    chorusPostFilterR; //xy=2572,337
AudioMixer4_F32          chorusOutputL;  //xy=2765,125
AudioMixer4_F32          chorusOutputR;  //xy=2765,274
AudioMixer4_F32          delayMixerR;    //xy=2959,321
AudioMixer4_F32          reverbMixer;    //xy=2960,199
AudioMixer4_F32          delayMixerL;    //xy=2965,80
AudioEffectSimpleDelay_F32 delayR;         //xy=3103,321
AudioEffectSimpleDelay_F32 delayL;         //xy=3107,80
AudioFilterBiquad_F32    reverbPreFilter; //xy=3123,199
AudioFilterBiquad_F32    delayFilterR;   //xy=3243,321
AudioFilterBiquad_F32    delayFilterL;   //xy=3244,80
AudioEffectSimpleDelay_F32 reverbPreDelay; //xy=3297,199
AudioEffectFreeverbStereo_F32 reverb;         //xy=3445,199
AudioMixer4_F32          mixerL;         //xy=3613,144
AudioMixer4_F32          mixerR;         //xy=3616,293
AudioFilterBiquad_F32    cabSimL;        //xy=3750,144
AudioFilterBiquad_F32    cabSimR;        //xy=3752,293
AudioEffectGain_F32      volumeL;        //xy=3904.2500648498535,143.50000286102295
AudioEffectGain_F32      volumeR;        //xy=3904.250057220459,292.50000381469727
AudioAnalyzePeak_F32     levelOutL;      //xy=3905.7500648498535,185.50000190734863
AudioAnalyzePeak_F32     levelOutR;      //xy=3907.5000648498535,240.50000381469727
AudioOutputI2S_F32       audioOut;       //xy=4123.2500648498535,215.75000381469727
AudioConnection_F32          patchCord1(audioIn, 0, inputMixer, 0);
AudioConnection_F32          patchCord2(audioIn, 1, inputMixer, 1);
AudioConnection_F32          patchCord3(testTone, 0, inputMixer, 3);
AudioConnection_F32          patchCord4(testNoise, 0, inputMixer, 2);
AudioConnection_F32          patchCord5(inputMixer, preampEq);
AudioConnection_F32          patchCord6(inputMixer, levelIn);
AudioConnection_F32          patchCord7(preampEq, 0, dynamics, 0);
AudioConnection_F32          patchCord8(dynamics, 0, wahFilter, 0);
AudioConnection_F32          patchCord9(dynamics, 0, wahMixer, 0);
AudioConnection_F32          patchCord10(dynamics, 1, wahSensitivity, 0);
AudioConnection_F32          patchCord11(wahSensitivity, 0, wahFilter, 1);
AudioConnection_F32          patchCord12(wahFilter, 0, wahMixer, 1);
AudioConnection_F32          patchCord13(wahFilter, 1, wahMixer, 2);
AudioConnection_F32          patchCord14(wahFilter, 2, wahMixer, 3);
AudioConnection_F32          patchCord15(wahMixer, distPre);
AudioConnection_F32          patchCord16(distPre, distortion);
AudioConnection_F32          patchCord17(distortion, distPost);
AudioConnection_F32          patchCord18(tremoloLfo, 0, tremoloVca, 1);
AudioConnection_F32          patchCord19(distPost, 0, tremoloVca, 0);
AudioConnection_F32          patchCord20(tremoloVca, 0, chorusOutputL, 0);
AudioConnection_F32          patchCord21(tremoloVca, 0, chorusOutputR, 0);
AudioConnection_F32          patchCord22(tremoloVca, chorusInputFilter);
AudioConnection_F32          patchCord23(chorusInputFilter, 0, chorusInputL, 0);
AudioConnection_F32          patchCord24(chorusInputFilter, 0, chorusInputC, 0);
AudioConnection_F32          patchCord25(chorusInputFilter, 0, chorusInputR, 0);
AudioConnection_F32          patchCord26(chorusInputL, 0, chorusModDelayL, 0);
AudioConnection_F32          patchCord27(chorusLfoL, 0, chorusModDelayL, 1);
AudioConnection_F32          patchCord28(chorusInputC, 0, chorusModDelayC, 0);
AudioConnection_F32          patchCord29(chorusLfoC, 0, chorusModDelayC, 1);
AudioConnection_F32          patchCord30(chorusInputR, 0, chorusModDelayR, 0);
AudioConnection_F32          patchCord31(chorusLfoR, 0, chorusModDelayR, 1);
AudioConnection_F32          patchCord32(chorusModDelayL, chorusPostFilterL);
AudioConnection_F32          patchCord33(chorusModDelayC, chorusPostFilterC);
AudioConnection_F32          patchCord34(chorusModDelayR, chorusPostFilterR);
AudioConnection_F32          patchCord35(chorusPostFilterL, 0, chorusOutputL, 1);
AudioConnection_F32          patchCord36(chorusPostFilterL, 0, chorusOutputR, 1);
AudioConnection_F32          patchCord37(chorusPostFilterL, 0, chorusInputC, 1);
AudioConnection_F32          patchCord38(chorusPostFilterL, 0, chorusInputR, 1);
AudioConnection_F32          patchCord39(chorusPostFilterL, 0, chorusInputL, 1);
AudioConnection_F32          patchCord40(chorusPostFilterC, 0, chorusOutputL, 2);
AudioConnection_F32          patchCord41(chorusPostFilterC, 0, chorusOutputR, 2);
AudioConnection_F32          patchCord42(chorusPostFilterC, 0, chorusInputC, 2);
AudioConnection_F32          patchCord43(chorusPostFilterC, 0, chorusInputR, 2);
AudioConnection_F32          patchCord44(chorusPostFilterC, 0, chorusInputL, 2);
AudioConnection_F32          patchCord45(chorusPostFilterR, 0, chorusOutputL, 3);
AudioConnection_F32          patchCord46(chorusPostFilterR, 0, chorusOutputR, 3);
AudioConnection_F32          patchCord47(chorusPostFilterR, 0, chorusInputC, 3);
AudioConnection_F32          patchCord48(chorusPostFilterR, 0, chorusInputR, 3);
AudioConnection_F32          patchCord49(chorusPostFilterR, 0, chorusInputL, 3);
AudioConnection_F32          patchCord50(chorusOutputL, 0, mixerL, 0);
AudioConnection_F32          patchCord51(chorusOutputL, 0, reverbMixer, 0);
AudioConnection_F32          patchCord52(chorusOutputL, 0, delayMixerL, 0);
AudioConnection_F32          patchCord53(chorusOutputR, 0, mixerR, 0);
AudioConnection_F32          patchCord54(chorusOutputR, 0, reverbMixer, 1);
AudioConnection_F32          patchCord55(chorusOutputR, 0, delayMixerR, 0);
AudioConnection_F32          patchCord56(delayMixerR, delayR);
AudioConnection_F32          patchCord57(reverbMixer, reverbPreFilter);
AudioConnection_F32          patchCord58(delayMixerL, delayL);
AudioConnection_F32          patchCord59(delayR, delayFilterR);
AudioConnection_F32          patchCord60(delayL, delayFilterL);
AudioConnection_F32          patchCord61(reverbPreFilter, reverbPreDelay);
AudioConnection_F32          patchCord62(delayFilterR, 0, delayMixerR, 1);
AudioConnection_F32          patchCord63(delayFilterR, 0, mixerR, 2);
AudioConnection_F32          patchCord64(delayFilterL, 0, delayMixerL, 1);
AudioConnection_F32          patchCord65(delayFilterL, 0, mixerL, 2);
AudioConnection_F32          patchCord66(reverbPreDelay, reverb);
AudioConnection_F32          patchCord67(reverb, 0, mixerL, 1);
AudioConnection_F32          patchCord68(reverb, 1, mixerR, 1);
AudioConnection_F32          patchCord69(mixerL, cabSimL);
AudioConnection_F32          patchCord70(mixerR, cabSimR);
AudioConnection_F32          patchCord71(cabSimL, volumeL);
AudioConnection_F32          patchCord72(cabSimL, levelOutL);
AudioConnection_F32          patchCord73(cabSimR, volumeR);
AudioConnection_F32          patchCord74(cabSimR, levelOutR);
AudioConnection_F32          patchCord75(volumeL, 0, audioOut, 0);
AudioConnection_F32          patchCord76(volumeR, 0, audioOut, 1);
// GUItool: end automatically generated code




#define HW_UX1Hack
//#define HW_TGAPro

//2. high pass 400hz
//3. low pass ??
//4. low pass fc=3125.0f Q=2.4281f




const int chorusDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.050f;
DMAMEM float chorusDelayLineL[chorusDelaySamples];
DMAMEM float chorusDelayLineC[chorusDelaySamples];
DMAMEM float chorusDelayLineR[chorusDelaySamples];

const int reverbPreDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.200f;
DMAMEM float reverbPreDelayLine[reverbPreDelaySamples];

const int delaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.800f;
DMAMEM float delayLineL[delaySamples];
DMAMEM float delayLineR[delaySamples];

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


void readLevels()
{
  if (levelIn.available()) lastLevelIn = levelIn.read();
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

void setPeak(AudioFilterBiquad_F32 &filter, uint32_t stage, float frequency, float gain, float q, bool print = false)
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

  if (print) Serial.printf("b0=%.6f\tb1=%.6f\tb2=%.6f\ta1=%.6f\ta2=%.6f\r\n", float(coeff[0]), float(coeff[1]), float(coeff[2]), float(coeff[3]), float(coeff[4]));
  
  filter.setCoefficients(stage, coeff); 
}

void set1stOrderHighPass(AudioFilterBiquad_F32 &filter, uint32_t stage, float frequency)
{
  double coeff[5];
  double w0 = (double)frequency * (2.0 * 3.141592654 / (double)AUDIO_SAMPLE_RATE_EXACT);
  double k = tan(w0 / 2.0);
  double a = 1.0 + k;

  /* b0 */ coeff[0] = 1 / a;
  /* b1 */ coeff[1] = -coeff[0];
  /* b2 */ coeff[2] = 0.0;
  /* a1 */ coeff[3] = -(1.0 - k) / a;
  /* a2 */ coeff[4] = 0.0;

  filter.setCoefficients(stage, coeff);
}

void set1stOrderLowPass(AudioFilterBiquad_F32 &filter, uint32_t stage, float frequency)
{
  double coeff[5];
  double w0 = (double)frequency * (2.0 * 3.141592654 / (double)AUDIO_SAMPLE_RATE_EXACT);
  double k = tan(w0 / 2.0);
  double a = 1.0 + k;

  /* b0 */ coeff[0] = k / a;
  /* b1 */ coeff[1] = coeff[0];
  /* b2 */ coeff[2] = 0.0;
  /* a1 */ coeff[3] = -(1.0 - k) / a;
  /* a2 */ coeff[4] = 0.0;

  filter.setCoefficients(stage, coeff);
}

void set8thOrderHighPass(AudioFilterBiquad_F32 &filter, float frequency)
{
  filter.setHighpass(0, frequency * 2.18872623053f, 1.22566942541f);
  filter.setHighpass(1, frequency * 1.95319575902f, 0.710852074442f);
  filter.setHighpass(2, frequency * 1.83209260120f, 0.559609164796f);
  filter.setHighpass(3, frequency * 1.77846591177f, 0.505991069397f);
}

void set8thOrderLowPass(AudioFilterBiquad_F32 &filter, float frequency)
{
  filter.setLowpass(0, frequency * 2.18872623053f, 1.22566942541f);
  filter.setLowpass(1, frequency * 1.95319575902f, 0.710852074442f);
  filter.setLowpass(2, frequency * 1.83209260120f, 0.559609164796f);
  filter.setLowpass(3, frequency * 1.77846591177f, 0.505991069397f);
}

/*******************************************************************************************************************/

void setInputMixer(float instrument = 1.0f, float mike = 0.0f)
{
  AudioNoInterrupts();
  inputMixer.gain(1, instrument);
  inputMixer.gain(0, mike);
  AudioInterrupts();
}

void setPreampEq(bool enable = true, float hpf = 100.0f, float lsf = 1000.0f, float lsg = -6.0f, float lpf = 8500.0f, float lpq = 0.7071f)
{
  preampEq.doClassInit();
  if (enable)
  {
    preampEq.setHighpass(0, hpf, 0.7071f);
    preampEq.setLowShelf(1, lsf, lsg, 0.3f);
    preampEq.setLowpass(2, lpf, lpq);
  }
  preampEq.begin();  
}

void setDetector()
{
  dynamics.detector(AudioEffectDynamics_F32::DetectorType_RMS, 0.04f, 0.0f);
}

void setGate(bool enable = true, float threshold = -75.0f, float attack = 0.0001f, float release = 3.0f, float reduction = -14.0f)
{
  dynamics.gate(threshold, attack, release, 6.0f, reduction, enable);
}

void setCompression(bool enable = true, float threshold = -50.0f, float attack = 0.001f, float release = 2.5f, float ratio = 1.5f, float makeup = 6.0f)
{
  AudioNoInterrupts();
  dynamics.compression(threshold, attack, release, ratio, 6.0f, enable);
  dynamics.makeupGain(enable ? makeup : 0.0f);
  dynamics.autoMakeupGain(1.5f, enable);
  AudioInterrupts();
}

void setLimiter(bool enable = true, float threshold = 0.0f)
{
  dynamics.limit(threshold, 0.0015f, 0.3f, enable);
}

void setAutoWah(bool enable = false, float sensitivity = 9.0f, float centerFreq = 250.0f, float resonance = 3.5f)
{
  AudioNoInterrupts();
  wahSensitivity.setGain(sensitivity);
  wahFilter.frequency(centerFreq);
  wahFilter.resonance(resonance);
  wahFilter.octaveControl(3.0f);
  wahFilter.controlSmoothing(0.2f);
  wahMixer.gain(0, enable ? 0.0f : 1.0f);
  wahMixer.gain(1, enable ? 0.25f : 0.0f);
  wahMixer.gain(2, enable ? 0.55f : 0.0f);
  wahMixer.gain(3, 0.0f);
  AudioInterrupts();
}

void setDistortion(bool enable = false, float gain = 1.0f, float curve = 0.0f, float level = 1.0f)
{
  AudioNoInterrupts();
  distPre.doClassInit();
  distPost.doClassInit();
  if (enable)
  {
    distPre.setHighpass(0, 175.0f, 0.5f);
    distPre.setLowShelf(1, 1000.0f, -4.0f, 0.3f);
    distPre.setLowpass(2, 4000.0f, 1.0f);
    distPost.setLowpass(1, 8000.0f * 1.6033575f, 0.805538f);
    distPost.setLowpass(2, 8000.0f * 1.4301716f, 0.521935f);
  }
  distPre.begin();
  distPost.begin();
  distortion.gain(enable ? gain : 0.5f, enable ? curve : 0.0f);
  distortion.level(enable ? level : 2.0f);
  AudioInterrupts();
}

void setToneStack(bool enable = true, float bass = 0.0f, float mid = 0.0f, float treble = 0.0f, float lpf = 8000.0f)
{
  distPost.doClassInit();
  if (enable)
  {
    distPost.setLowShelf(0, 500.0f, bass, 0.5f);
    setPeak(distPost, 1, 1375.0f, mid, 0.5f);
    distPost.setHighShelf(2, 2000.0f, treble, 0.5f);
    distPost.setLowpass(3, lpf, 0.7071f);
  }
  distPost.begin();
}

void setTremolo(bool enable = false, float rate = 3.5f, float depth = 0.3f)
{
  AudioNoInterrupts();
  tremoloLfo.begin(enable ? depth : 0.0f, rate, WAVEFORM_SINE);
  tremoloVca.controlSmoothing(0.02f);
  tremoloVca.offset(enable ? (1.0f - depth) : 0.0f);
  AudioInterrupts();
}

void setChorus(bool enable = true, float rate = 1.7f, float depth = 0.032f, float mix = 0.6f, float resonance = 0.5f, bool sine = false)
{
  float halfmix = mix * 0.5f;
  float halfres = resonance * 0.5f;
  AudioNoInterrupts();
  chorusOutputL.gain(0, enable ? 1.0f - halfmix : 1.0f);
  chorusOutputR.gain(0, enable ? 1.0f - halfmix : 1.0f);

  chorusInputFilter.doClassInit();
  chorusInputFilter.setHighpass(0, 50.0f, 0.7071f);
  chorusInputFilter.setLowpass(1, 6900.0f * 1.6033575f, 0.805538f);
  chorusInputFilter.setLowpass(2, 6900.0f * 1.4301716f, 0.521935f);
  chorusInputFilter.begin();
  
  chorusLfoL.begin(depth, rate, sine ? WAVEFORM_SINE : WAVEFORM_TRIANGLE);
  chorusLfoL.phase(0.0f);
  chorusInputL.gain(0, 1.0f - halfres);
  chorusInputL.gain(1, 0.0f);
  chorusInputL.gain(2, 0.0f);
  chorusInputL.gain(3, 0.0f);
  chorusModDelayL.delay(23.0f);
  chorusPostFilterL.doClassInit();
  chorusPostFilterL.setHighpass(0, 150.0f, 0.7071f);
  chorusPostFilterL.setNotch(1, 6500.0f, 0.7071f);
  chorusPostFilterL.setLowpass(2, 8000.0f * 1.6033575f, 0.805538f);
  chorusPostFilterL.setLowpass(3, 8000.0f * 1.4301716f, 0.521935f);
  chorusPostFilterL.begin();
  chorusOutputL.gain(1, enable ? mix : 0.0f);
  chorusOutputR.gain(1, 0.0f);

  chorusLfoC.begin(depth, rate * 0.5f, sine ? WAVEFORM_SINE : WAVEFORM_TRIANGLE);
  chorusLfoC.phase(90.0f);
  chorusInputC.gain(0, 1.0f - halfres);
  chorusInputC.gain(1, 0.0f);
  chorusInputC.gain(2, resonance);
  chorusInputC.gain(3, 0.0f);
  chorusModDelayC.delay(8.0f);
  chorusPostFilterC.doClassInit();
  chorusPostFilterC.setHighpass(0, 250.0f, 0.7071f);
  chorusPostFilterC.setNotch(1, 6500.0f, 0.7071f);
  chorusPostFilterC.setLowpass(2, 8000.0f * 1.6033575f, 0.805538f);
  chorusPostFilterC.setLowpass(3, 8000.0f * 1.4301716f, 0.521935f);
  chorusPostFilterC.begin();
  chorusOutputL.gain(2, enable ? halfmix : 0.0f);
  chorusOutputR.gain(2, enable ? halfmix : 0.0f);

  chorusLfoR.begin(depth, rate, sine ? WAVEFORM_SINE : WAVEFORM_TRIANGLE);
  chorusLfoR.phase(120.0f);
  chorusInputR.gain(0, 1.0f - halfres);
  chorusInputR.gain(1, 0.0f);
  chorusInputR.gain(2, 0.0f);
  chorusInputR.gain(3, 0.0f);
  chorusModDelayR.delay(23.0f);
  chorusPostFilterR.doClassInit();
  chorusPostFilterR.setHighpass(0, 150.0f, 0.7071f);
  chorusPostFilterR.setNotch(1, 6500.0f, 0.7071f);
  chorusPostFilterR.setLowpass(2, 8000.0f * 1.6033575f, 0.805538f);
  chorusPostFilterR.setLowpass(3, 8000.0f * 1.4301716f, 0.521935f);
  chorusPostFilterR.begin();
  chorusOutputL.gain(3, 0.0f);
  chorusOutputR.gain(3, enable ? mix : 0.0f);

  AudioInterrupts();
}



void setDimension(int mode = 3)
{
    AudioNoInterrupts();
    if (mode < 1) mode = 1;
    if (mode > 4) mode = 4;
    float delay = mode == 1 ? 10.0f : 7.5f;
    float rate = mode < 3 ? 0.25f : 0.5f;
    float modulation = mode == 2 ? 0.16667f : 0.1f;
    float dry = mode == 4 ? 0.25f : 0.45f;
    float mix = mode == 4 ? 0.7f : 0.5f;
    float feedback = 0.5f;
    
    chorusOutputL.gain(0, dry);
    chorusOutputR.gain(0, dry);
    
    chorusLfoL.begin(modulation, rate, WAVEFORM_TRIANGLE);
    chorusLfoL.phase(0.0f);
    chorusInputL.gain(0, 0.9f);
    chorusInputL.gain(1, feedback);
    chorusInputL.gain(2, 0.0f);
    chorusInputL.gain(3, -feedback * 0.4f);
    chorusModDelayL.delay(delay);
    chorusPostFilterL.doClassInit();
    chorusPostFilterL.setHighpass(0, 80.0f, 0.7071f);
    chorusPostFilterL.setNotch(1, 6500.0f, 0.7071f);
    chorusPostFilterL.setLowpass(2, 9000.0f, 0.5f);
    chorusPostFilterL.begin();
    chorusOutputL.gain(1, mix);
    chorusOutputR.gain(1, -mix * 0.15f);
  
    chorusInputC.gain(0, 0.0f);
    chorusInputC.gain(1, 0.0f);
    chorusInputC.gain(2, 0.0f);
    chorusInputC.gain(3, 0.0f);
    chorusOutputL.gain(2, 0.0f);
    chorusOutputR.gain(2, 0.0f);
  
    chorusLfoR.begin(modulation, rate, WAVEFORM_TRIANGLE);
    chorusLfoR.phase(180.0f);
    chorusInputR.gain(0, 0.9f);
    chorusInputR.gain(1, -feedback * 0.4f);
    chorusInputR.gain(2, 0.0f);
    chorusInputR.gain(3, feedback);
    chorusModDelayR.delay(delay);
    chorusPostFilterR.doClassInit();
    chorusPostFilterR.setHighpass(0, 80.0f, 0.7071f);
    chorusPostFilterR.setNotch(1, 6500.0f, 0.7071f);
    chorusPostFilterR.setLowpass(2, 9000.0f, 0.5f);
    chorusPostFilterR.begin();
    chorusOutputL.gain(3, -mix * 0.15f);
    chorusOutputR.gain(3, mix);
      
    AudioInterrupts();
}

void setTriStereoChorus(float speed = 0.5f, float left = 0.5f, float center = 0.5f, float right = 0.5f)
{
    AudioNoInterrupts();
    left *= 0.8f;
    center *= 0.4f;
    right *= 0.8f;

    float modulation = 0.3f - (speed*speed*0.29f);
    float rate = 0.25f + (speed*speed*speed*speed*7.75f);
    
    chorusOutputL.gain(0, 1.0f - (left + center) * 0.8f);
    chorusOutputR.gain(0, 1.0f - (right + center) * 0.8f);
    
    chorusLfoL.begin(modulation, rate, WAVEFORM_SINE);
    chorusLfoL.phase(0.0f);
    chorusInputL.gain(0, 1.0f);
    chorusInputL.gain(1, 0.0f);
    chorusInputL.gain(2, 0.0f);
    chorusInputL.gain(3, 0.0f);
    chorusModDelayL.delay(30.0f);
    chorusPostFilterL.doClassInit();
    chorusPostFilterL.setHighpass(0, 250.0f, 0.7071f);
    chorusPostFilterL.setNotch(1, 6500.0f, 0.7071f);
    chorusPostFilterL.setLowpass(2, 10000.0f, 0.5f);
    chorusPostFilterL.begin();
    chorusOutputL.gain(1, left);
    chorusOutputR.gain(1, 0.0f);
  
    chorusLfoC.begin(modulation, rate, WAVEFORM_SINE);
    chorusLfoC.phase(120.0f);
    chorusInputC.gain(0, 1.0f);
    chorusInputC.gain(1, 0.0f);
    chorusInputC.gain(2, 0.0f);
    chorusInputC.gain(3, 0.0f);
    chorusModDelayC.delay(30.0f);
    chorusPostFilterC.doClassInit();
    chorusPostFilterC.setHighpass(0, 250.0f, 0.7071f);
    chorusPostFilterC.setNotch(1, 6500.0f, 0.7071f);
    chorusPostFilterC.setLowpass(2, 10000.0f, 0.5f);
    chorusPostFilterC.begin();
    chorusOutputL.gain(2, center);
    chorusOutputR.gain(2, center);
  
    chorusLfoR.begin(modulation, rate, WAVEFORM_SINE);
    chorusLfoR.phase(240.0f);
    chorusInputR.gain(0, 1.0f);
    chorusInputR.gain(1, 0.0f);
    chorusInputR.gain(2, 0.0f);
    chorusInputR.gain(3, 0.0f);
    chorusModDelayR.delay(30.0f);
    chorusPostFilterR.doClassInit();
    chorusPostFilterR.setHighpass(0, 250.0f, 0.7071f);
    chorusPostFilterR.setNotch(1, 6500.0f, 0.7071f);
    chorusPostFilterR.setLowpass(2, 10000.0f, 0.5f);
    chorusPostFilterR.begin();
    chorusOutputL.gain(3, 0.0f);
    chorusOutputR.gain(3, right);
      
    AudioInterrupts();
}

void setReverb(bool enable = true, float predelay = 90.0f, float room = 0.75f, float damping = 0.12f, float mix = 0.03f)
{
  AudioNoInterrupts();
  if (!enable) mix = 0.0f;
  reverbMixer.gain(0, 0.5f);
  reverbMixer.gain(1, 0.5f);
  reverbPreDelay.delay(predelay);
  reverb.roomsize(room);
  reverb.damping(damping);
  mixerL.gain(0, 1.0f - mix);
  mixerR.gain(0, 1.0f - mix);
  mixerL.gain(1, mix);
  mixerR.gain(1, mix);
  AudioInterrupts();
}

void setDelay(bool enable = true, float delayms = 375.0f, float feedback = 0.25f, float lpf = 6500.0f, float mix = 0.03f)
{
  AudioNoInterrupts();
  delayL.delay(delayms);
  delayR.delay(delayms);
  delayMixerL.gain(0, 1.0f);
  delayMixerR.gain(0, 1.0f);
  delayMixerL.gain(1, feedback);
  delayMixerR.gain(1, feedback);
  delayFilterL.doClassInit();
  delayFilterL.setLowpass(0, lpf, 0.7071f);
  delayFilterL.begin();
  delayFilterR.doClassInit();
  delayFilterR.setLowpass(0, lpf, 0.7071f);
  delayFilterR.begin();
  mixerL.gain(2, enable ? mix : 0.0f);
  mixerR.gain(2, enable ? mix : 0.0f);
  AudioInterrupts();
}

void setCabSim(bool enable = true, float scoop = 1300.0f, float gain = -9.0f)
{
  cabSimL.doClassInit();
  cabSimR.doClassInit();

  if (enable)
  {
    cabSimL.setHighpass(0, 350.0f, 0.7000f);
    setPeak(cabSimL, 1, scoop, gain, 0.5f);
    cabSimL.setLowpass(2, 3300.0f, 1.4000f);
  
    cabSimR.setHighpass(0, 350.0f, 0.7000f);
    setPeak(cabSimR, 1, scoop, gain, 0.5f);
    cabSimR.setLowpass(2, 3300.0f, 1.4000f);
  }

  cabSimL.begin();
  cabSimR.begin();
}

void setVolume(float volume = 1.0f)
{
  AudioNoInterrupts();
  volumeL.setGain(volume);
  volumeR.setGain(volume);
  AudioInterrupts();
}

void setup()
{
  AudioMemory(5);
  AudioMemory_F32(50);  

  chorusInputL.gain(0, 0.0f);
  chorusInputL.gain(1, 0.0f);
  chorusInputL.gain(2, 0.0f);
  chorusInputL.gain(3, 0.0f);
  chorusInputC.gain(0, 0.0f);
  chorusInputC.gain(1, 0.0f);
  chorusInputC.gain(2, 0.0f);
  chorusInputC.gain(3, 0.0f);
  chorusInputR.gain(0, 0.0f);
  chorusInputR.gain(1, 0.0f);
  chorusInputR.gain(2, 0.0f);
  chorusInputR.gain(3, 0.0f);
  delayMixerL.gain(0, 0.0f);
  delayMixerL.gain(1, 0.0f);
  delayMixerL.gain(2, 0.0f);
  delayMixerL.gain(3, 0.0f);
  delayMixerR.gain(0, 0.0f);
  delayMixerR.gain(1, 0.0f);
  delayMixerR.gain(2, 0.0f);
  delayMixerR.gain(3, 0.0f);
  reverbMixer.gain(0, 0.0f);
  reverbMixer.gain(1, 0.0f);
  reverbMixer.gain(2, 0.0f);
  reverbMixer.gain(3, 0.0f);

  distortion.begin();

  delayL.begin(delayLineL, delaySamples);
  delayR.begin(delayLineR, delaySamples);
  chorusModDelayL.begin(chorusDelayLineL, chorusDelaySamples);
  chorusModDelayC.begin(chorusDelayLineC, chorusDelaySamples);
  chorusModDelayR.begin(chorusDelayLineR, chorusDelaySamples);
  reverbPreDelay.begin(reverbPreDelayLine, reverbPreDelaySamples);

  mixerL.gain(0, 0.0f);
  mixerR.gain(0, 0.0f);
  mixerL.gain(1, 0.0f);
  mixerR.gain(1, 0.0f);
  mixerL.gain(2, 0.0f);
  mixerR.gain(2, 0.0f);
  mixerL.gain(3, 0.0f);
  mixerR.gain(3, 0.0f);

  testNoise.amplitude(0.0f);
  testTone.begin();
  testTone.amplitude(0.0f);
  testTone.frequency(1000.0f);

  HW_Setup();
  initSercom();

  setInputMixer();
  setDetector();
  setPreampEq();
  setGate();
  setCompression();
  setLimiter();
  setAutoWah();
  setDistortion();
  setToneStack();
  setTremolo();
  setChorus();
  setReverb();
  setDelay();
  setCabSim();  
  setVolume();
}

void loop() {
  readLevels();
  HW_Loop();
  handleSercom();
}
