#include "effect_simple_delay_F32.h"
#include "effect_tremolo_F32.h"
#include "filter_state_variable_F32.h"
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
AudioInputI2S_F32        audioIn;        //xy=70.4444465637207,46
AudioSynthWaveformSine_F32 testTone;       //xy=70.22222137451172,135.77777481079102
AudioSynthNoiseWhite_F32 testNoise;    //xy=72.22222137451172,90.00000190734863
AudioMixer4_F32          inputMixer;     //xy=251,59
AudioAnalyzePeak_F32      levelIn;           //xy=268.8888931274414,151.11112022399902
AudioSynthWaveform_F32   wahLfo;         //xy=389,162
AudioFilterBiquad_F32    preEq;          //xy=396,58
AudioFilterBiquad_F32    scFilter;        //xy=455.5555191040039,104.44444465637207
AudioEffectDynamics_F32  dynamics;       //xy=558.4444580078125,63.99999809265137
AudioMixer4_F32          wahModMixer;    //xy=697.090892791748,209.90909385681152
AudioFilterStateVariable_F32 wahFilter;      //xy=830.727294921875,124.63635635375977
AudioMixer4_F32          wahMixer;       //xy=914.9090995788574,50.72727584838867
AudioEffectNonLinear_F32 distortion;     //xy=1049.818229675293,52.636362075805664
AudioSynthWaveform_F32   tremoloLfo;     //xy=1058.4545669555664,193.4545383453369
AudioEffectTremolo_F32   tremoloVca;     //xy=1196.818130493164,59.54544925689697
AudioFilterEqualizer_F32 postEq;         //xy=1328.6363258361816,59.09090995788574
AudioMixer4_F32          delayMixer;     //xy=1491.0128746032715,208.42213821411133
AudioMixer4_F32          chorusMixer;    //xy=1491.7402229309082,40.69486999511719
AudioSynthWaveform_F32   chorusLfo;      //xy=1495.831111907959,125.60396194458008
AudioEffectSimpleDelay_F32 delayLine;      //xy=1640.1947631835938,249.51306343078613
AudioEffectModulatedDelay_F32 chorusModDelay; //xy=1677.7401809692383,48.24032020568848
AudioFilterBiquad_F32    delayFilter;    //xy=1820.2856750488281,251.05851936340332
AudioFilterBiquad_F32    chorusPostFilter; //xy=1870.9219970703125,50.24032497406006
AudioEffectSimpleDelay_F32 chorusPostDelay; //xy=2062.376663208008,238.51304817199707
AudioEffectSimpleDelay_F32 delayPostDelay; //xy=2065.8310775756836,117.24032211303711
AudioMixer4_F32          reverbMixer;    //xy=2244.2856636047363,177.7857780456543
AudioMixer4_F32          effectMixerL;   //xy=2245.2856636047363,107.7857780456543
AudioMixer4_F32          effectMixerR;   //xy=2245.2856636047363,246.7857780456543
AudioEffectSimpleDelay_F32 reverbPreDelay; //xy=2407.2856636047363,176.7857780456543
AudioEffectFreeverbStereo_F32 reverb;         //xy=2554.2856636047363,176.7857780456543
AudioMixer4_F32          mixerR;         //xy=2709.2856636047363,262.7857780456543
AudioMixer4_F32          mixerL;         //xy=2718.2856636047363,122.7857780456543
AudioOutputI2S_F32       audioOut;       //xy=2849.2856636047363,159.7857780456543
AudioAnalyzeRMS_F32      levelOut;           //xy=2864.841220855713,100.1191177368164
AudioConnection_F32          patchCord1(audioIn, 0, inputMixer, 0);
AudioConnection_F32          patchCord2(audioIn, 1, inputMixer, 1);
AudioConnection_F32          patchCord3(testTone, 0, inputMixer, 3);
AudioConnection_F32          patchCord4(testNoise, 0, inputMixer, 2);
AudioConnection_F32          patchCord5(inputMixer, preEq);
AudioConnection_F32          patchCord6(inputMixer, levelIn);
AudioConnection_F32          patchCord7(wahLfo, 0, wahModMixer, 1);
AudioConnection_F32          patchCord8(preEq, 0, dynamics, 0);
AudioConnection_F32          patchCord9(preEq, scFilter);
AudioConnection_F32          patchCord10(scFilter, 0, dynamics, 1);
AudioConnection_F32          patchCord11(dynamics, 0, wahFilter, 0);
AudioConnection_F32          patchCord12(dynamics, 0, wahMixer, 0);
AudioConnection_F32          patchCord13(dynamics, 1, wahModMixer, 0);
AudioConnection_F32          patchCord14(wahModMixer, 0, wahFilter, 1);
AudioConnection_F32          patchCord15(wahFilter, 0, wahMixer, 1);
AudioConnection_F32          patchCord16(wahFilter, 1, wahMixer, 2);
AudioConnection_F32          patchCord17(wahFilter, 2, wahMixer, 3);
AudioConnection_F32          patchCord18(wahMixer, distortion);
AudioConnection_F32          patchCord19(distortion, 0, tremoloVca, 0);
AudioConnection_F32          patchCord20(tremoloLfo, 0, tremoloVca, 1);
AudioConnection_F32          patchCord21(tremoloVca, postEq);
AudioConnection_F32          patchCord22(postEq, 0, effectMixerL, 0);
AudioConnection_F32          patchCord23(postEq, 0, effectMixerR, 0);
AudioConnection_F32          patchCord24(postEq, 0, delayMixer, 0);
AudioConnection_F32          patchCord25(postEq, 0, chorusMixer, 0);
AudioConnection_F32          patchCord26(delayMixer, delayLine);
AudioConnection_F32          patchCord27(chorusMixer, 0, chorusModDelay, 0);
AudioConnection_F32          patchCord28(chorusLfo, 0, chorusModDelay, 1);
AudioConnection_F32          patchCord29(delayLine, delayFilter);
AudioConnection_F32          patchCord30(chorusModDelay, chorusPostFilter);
AudioConnection_F32          patchCord31(delayFilter, 0, effectMixerR, 2);
AudioConnection_F32          patchCord32(delayFilter, 0, delayMixer, 1);
AudioConnection_F32          patchCord33(delayFilter, delayPostDelay);
AudioConnection_F32          patchCord34(chorusPostFilter, 0, effectMixerL, 1);
AudioConnection_F32          patchCord35(chorusPostFilter, chorusPostDelay);
AudioConnection_F32          patchCord36(chorusPostFilter, 0, chorusMixer, 1);
AudioConnection_F32          patchCord37(chorusPostDelay, 0, effectMixerR, 1);
AudioConnection_F32          patchCord38(delayPostDelay, 0, effectMixerL, 2);
AudioConnection_F32          patchCord39(reverbMixer, reverbPreDelay);
AudioConnection_F32          patchCord40(effectMixerL, 0, mixerL, 0);
AudioConnection_F32          patchCord41(effectMixerL, 0, reverbMixer, 0);
AudioConnection_F32          patchCord42(effectMixerR, 0, mixerR, 0);
AudioConnection_F32          patchCord43(effectMixerR, 0, reverbMixer, 1);
AudioConnection_F32          patchCord44(reverbPreDelay, reverb);
AudioConnection_F32          patchCord45(reverb, 0, mixerL, 1);
AudioConnection_F32          patchCord46(reverb, 1, mixerR, 1);
AudioConnection_F32          patchCord47(mixerR, 0, audioOut, 1);
AudioConnection_F32          patchCord48(mixerL, 0, audioOut, 0);
AudioConnection_F32          patchCord49(mixerL, levelOut);
// GUItool: end automatically generated code



//#define LOOPER_EXTMEM

#ifdef LOOPER_EXTMEM
#include "effect_looper_F32.h"
EXTMEM AudioEffectLooper_F32    looper;
AudioConnection_F32          patchCordLooperIn(postEq, looper);
AudioConnection_F32          patchCordLooperOutL(looper, 0, chorusMixer, 1);
AudioConnection_F32          patchCordMetroL(looper, 1, mixerL, 3);
AudioConnection_F32          patchCordMetroR(looper, 1, mixerR, 3);
#endif

typedef enum
{
  X100_mute   = 0,
  X100_Dist   = 1,
  X100_Edge   = 2,
  X100_Cln1 = 3,
  X100_Cln2 = 4,
  
} X100_Channels;

const int posteqnf = 9;
float posteqf[]  = {   50.0f,   100.0f,      200.0f,      400.0f,      800.0f,      1600.0f,      3200.0f,      6400.0f,    12800.0f };
float eq1g[]     = {  -30.0f,    -6.0f,       -1.5f,       -1.5f,       -9.0f,       -12.0f,         1.5f,        -1.5f,      -30.0f };
float eq2g[]     = {  -30.0f,    -9.0f,       -6.0f,        0.5f,        0.0f,         0.0f,         4.5f,         6.0f,      -15.0f };
float posteqg[]  = {  -30.0f,     0.0f,        0.0f,        0.0f,        0.0f,         0.0f,         0.0f,         0.0f,        0.0f };
const int posteqnc = 31;
float posteqc[100];
float posteqsl = 30.0f;

const int chorusDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.030f;;
DMAMEM float chorusDelayLine[chorusDelaySamples];

const int chorusPostDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.030f;
DMAMEM float chorusPostDelayLine[chorusPostDelaySamples];

const int delayLineSamples = AUDIO_SAMPLE_RATE_EXACT * 1.0f;
DMAMEM float delayLineBuffer[delayLineSamples];

const int delayPostDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.100f;
DMAMEM float delayPostDelayLine[delayPostDelaySamples];

const int reverbPreDelaySamples = AUDIO_SAMPLE_RATE_EXACT * 0.100f;
DMAMEM float reverbPreDelayLine[reverbPreDelaySamples];

float instrumentGain =   1.000f;
float chorusGain = 0.750f;
float reverbGain = 0.018f;
float delayGain = 0.175f;

X100_Channels channel = X100_mute;

void setInstrument(bool enable)
{
  effectMixerL.gain(0, instrumentGain);
  effectMixerR.gain(0, instrumentGain);
  mixerL.gain(0, enable ? 1.0f : 0.0f);
  mixerR.gain(0, enable ? 1.0f : 0.0f);
}

void setChorus(bool enable)
{
  chorusMixer.gain(0, 1.0f);
  chorusMixer.gain(1, 0.2f);
  chorusModDelay.delay(23.0f);
  chorusLfo.begin(0.032f, 1.7f, WAVEFORM_TRIANGLE);
  chorusPostFilter.setNotch(0, 6500.0f, 0.7f);
  chorusPostFilter.setLowpass(1, 8000.0f, 0.5f);
  chorusPostFilter.begin();
  chorusPostDelay.delay(17.0f);
  effectMixerL.gain(1, enable ? chorusGain : 0.0f);
  effectMixerR.gain(1, enable ? -chorusGain : 0.0f);
}

void setReverb(bool enable)
{
  reverbMixer.gain(0, enable ? 0.5f : 0.0f);
  reverbMixer.gain(1, enable ? 0.5f : 0.0f);
  reverb.roomsize(0.8f);
  reverb.damping(0.002f);
  reverbPreDelay.delay(45.0f);
  mixerL.gain(1, reverbGain);
  mixerR.gain(1, reverbGain);
}

void setDelay(bool enable)
{
  delayMixer.gain(0, 1.0f);
  delayMixer.gain(1, 0.3f);
  delayLine.delay(500);
  delayFilter.setHighpass(0, 250.0f, 0.7071f);
  delayFilter.setLowpass(1, 4000.0f, 0.7071f);
  delayFilter.begin();
  delayPostDelay.delay(17.0f);
  effectMixerL.gain(2, enable ? delayGain : 0.0f);
  effectMixerR.gain(2, enable ? delayGain : 0.0f);
}

void setWah(bool enable)
{
  chorusLfo.begin(0.0f, 1.7f, WAVEFORM_SINE);
  wahModMixer.gain(0, 45.0f);
  wahModMixer.gain(1, 1.0f);
  wahMixer.gain(0, enable ? 0.0f : 1.0f);
  wahMixer.gain(1, 0.0f);
  wahMixer.gain(2, enable ? 1.0f : 0.0f);
  wahMixer.gain(3, 0.0f);
}

void setTremolo(bool enable)
{
  tremoloLfo.begin(enable? 0.3f : 0.0f, 3.5f, WAVEFORM_SINE);
  tremoloVca.controlSmoothing(0.02f);
  tremoloVca.offset(8.5f);
}

void setChannel(X100_Channels ch)
{
  if (ch == channel) return;

  inputMixer.gain(0, 0.0f);
  inputMixer.gain(1, 0.0f);
  delay(20);

  preEq.setHighpass(0, 200.0f, 0.7071f);
  preEq.setLowShelf(1, 1000.0f, -16.0f, 0.2f);
  preEq.setLowpass(2, 2800.0f, 1.5f);
  preEq.begin();
  
  scFilter.setHighpass(0, 20.0f, 0.7071f);
  scFilter.begin();

  dynamics.detector(AudioEffectDynamics_F32::DetectorType_DiodeBridge, 0.002f, 0.0f);
  dynamics.gate(-75.0f, 0.0f, 3.0f, 6.0f, -14.0f);
  dynamics.compression(-55.0f, 0.0015f, 2.5f, 4.5f, 3.0f);
  dynamics.autoMakeupGain(6.0f);

  postEq.equalizerNew(posteqnf, &posteqf[0], ch == X100_Cln2 ? &eq2g[0] : &eq1g[0], posteqnc, &posteqc[0], posteqsl);     

  distortion.gain(ch == X100_Dist ? 45.0f
                  : ch == X100_Edge ? 6.0f
                  : 1.0f);

  distortion.curve(ch == X100_Dist ? 0.2f
                :  ch == X100_Edge ? 1.0f
                :  ch == X100_Cln1 ? 1.0f
                :  0.0f);

  distortion.clip(1.0f);
  distortion.tone(ch == X100_Dist ? 0.80f : 1.0f);

  distortion.level(ch == X100_Dist ?  0.18f
                  : ch == X100_Edge ? 0.34f
                                    : 1.0f);
       
  delay(20);
  inputMixer.gain(0, 0.0f);
  inputMixer.gain(1, ch == X100_mute ? 0.0f : 1.0f);
  channel = ch;
}

void setup()
{
 
  AudioMemory(10);
  AudioMemory_F32(100);

  chorusModDelay.begin(chorusDelayLine, chorusDelaySamples);
  chorusPostDelay.begin(chorusPostDelayLine, chorusPostDelaySamples);
  delayLine.begin(delayLineBuffer, delayLineSamples);
  delayPostDelay.begin(delayPostDelayLine, delayPostDelaySamples);
  reverbPreDelay.begin(reverbPreDelayLine, reverbPreDelaySamples);

#ifdef LOOPER_EXTMEM  
  looper.begin();
  looper.signature();
  mixerL.gain(3, 0.0f);
  mixerR.gain(3, 0.0f); 
#endif

  setChorus(true);
  setReverb(true);
  setDelay(false);
  setWah(false);
  setTremolo(false);
  setInstrument(true);
  HW_Setup();
  initSercom();
  setChannel(X100_Cln1);

  testNoise.amplitude(0.0f);
  testTone.begin();
  testTone.amplitude(0.0f);
  testTone.frequency(1000.0f);
}

void loop() {
  HW_Loop();
  handleSercom();
}
