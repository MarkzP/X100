#include "AudioStream_F32.h"
#include <Bounce.h>
#include <Encoder.h>
//#include <1euroFilter.h>
#include "Display.h"
#include "Effect.h"


#define PIN_ENC_A      5
#define PIN_ENC_B      4
#define PIN_ENC_C      12

#define PIN_BTN_0       3
#define PIN_BTN_1       2
#define PIN_BTN_2       1
#define PIN_BTN_3       0
#define PIN_BTN_4       6

#define PIN_POT_0      14
#define PIN_POT_1      15
#define PIN_POT_2      16
#define PIN_POT_3      17
#define PIN_POT_4      18
#define PIN_POT_5      19

EPARAMS(pre_eq)
{
  Parameter("hp_f", Parameter::PT_Float, 100.0f, 50.0f, 800.0f, 10.0f),
  Parameter("ls_f", Parameter::PT_Float, 1000.0f, 800.0f, 1600.0f, 25.0f),
  Parameter("ls_g", Parameter::PT_Float, -4.5f, -12.0f, 12.0f),
  Parameter("lp_f", Parameter::PT_Float, 12500.0f, 3000.0f, 18000.0f, 250.0f),
  Parameter("lp_q", Parameter::PT_Float, 0.5f, 0.5f, 3.5f, 0.05f),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(pre_eq)
{
  int i = 0;
  Parameter& hp_f = e[i++];
  Parameter& ls_f = e[i++];
  Parameter& ls_g = e[i++];
  Parameter& lp_f = e[i++];
  Parameter& lp_q = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  
  if (enableChanged) preampEq.doClassInit();

  if (enable)
  {
    if (enableChanged || hp_f.changed()) preampEq.setHighpass(0, hp_f, 0.7071f);
    if (enableChanged || ls_f.changed() || ls_g.changed()) preampEq.setLowShelf(1, ls_f, ls_g, 0.3f);
    if (enableChanged || lp_f.changed() || lp_q.changed()) preampEq.setLowpass(2, lp_f, lp_q);
  }
  
  preampEq.begin();  
});

EPARAMS(gate)
{
  Parameter("threshold", Parameter::PT_Float, -75.0f, -100.0f, 0.0f),
  Parameter("attack", Parameter::PT_Float, 0.0f, 0.0f, 5.0f),
  Parameter("release", Parameter::PT_Float, 3.0f, 0.0f, 5.0f),
  Parameter("reduction", Parameter::PT_Float, -14.0f, -100.0f, 0.0f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(gate)
{
  int i = 0;
  Parameter& threshold = e[i++];
  Parameter& attack = e[i++];
  Parameter& release = e[i++];
  Parameter& reduction = e[i++];
  Parameter& enable = e[i++];

  dynamics.gate(threshold, attack, release, 6.0f, reduction, enable);
});

EPARAMS(compression)
{
  Parameter("threshold", Parameter::PT_Float, -50.0f, -100.0f, 0.0f),
  Parameter("attack", Parameter::PT_Float, 0.01f),
  Parameter("release", Parameter::PT_Float, 2.5f, 0.0f, 5.0f),
  Parameter("ratio", Parameter::PT_Float, 1.5f, 1.0f, 20.0f, 0.2f),
  Parameter("knee", Parameter::PT_Float, 6.0f, 0.0f, 12.0f, 0.5f),
  Parameter("maxatt", Parameter::PT_Float, 20.0f, 0.0f, 90.0f, 0.5f),
  Parameter("makeup", Parameter::PT_Float, 3.0f, -12.0f, 12.0f, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(compression)
{
  int i = 0;
  Parameter& threshold = e[i++];
  Parameter& attack = e[i++];
  Parameter& release = e[i++];
  Parameter& ratio = e[i++];
  Parameter& knee = e[i++];
  Parameter& maxatt = e[i++];
  Parameter& makeup = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || threshold.changed() || attack.changed() || ratio.changed() || knee.changed() || maxatt.changed()) dynamics.compression(threshold, attack, release, ratio, knee, maxatt, enable);
  if (enableChanged || makeup.changed()) dynamics.makeupGain(enable ? makeup : 0.0f);
  if (enableChanged) dynamics.autoMakeupGain(1.5f, enable);
});

EPARAMS(auto_filter)
{
  Parameter("sensitivity", Parameter::PT_Float, 9.0f, 0.0f, 10.0f),
  Parameter("freq", Parameter::PT_Freq, 250.0f, 100.0f, 10000.0f),
  Parameter("resonance", Parameter::PT_Float, 3.5f, 0.5f, 5.0f, 0.05f),
  Parameter("rate", Parameter::PT_Float, 5.0f, 0.1f, 50.0f, 0.1f),
  Parameter("depth", Parameter::PT_Float, 0.0f),
  Parameter("shape", Parameter::PT_Enum, 0.0f, 0.0f, 4.0f, 1.0f, (const char*[]){"sin", "tri", "saw", "rev", "squ"}),
  Parameter("smoothing", Parameter::PT_Float, 200.0f, 0.0f, 500.0f),
  Parameter("lp", Parameter::PT_Float, 0.25f),
  Parameter("bp", Parameter::PT_Float, 0.55f),
  Parameter("hp", Parameter::PT_Float, 0.0f),
  Parameter("dry", Parameter::PT_Float, 0.0f),
  Parameter("enable", Parameter::PT_Bool, false),  
};
EFFECT(auto_filter)
{
  int i = 0;
  Parameter& sensitivity = e[i++];
  Parameter& freq = e[i++];
  Parameter& resonance = e[i++];
  Parameter& rate = e[i++];
  Parameter& depth = e[i++];
  Parameter& shape = e[i++];
  Parameter& smoothing = e[i++];
  Parameter& lp = e[i++];
  Parameter& bp = e[i++];
  Parameter& hp = e[i++];
  Parameter& dry = e[i++];
  Parameter& enable = e[i++];
  
  bool enableChanged = enable.changed();

  if (enable)
  {
    if (enableChanged)
    {
      svFilter.octaveControl(3.0f);
      filterLfo.amplitude(1.0f);      
    }
    if (enableChanged || sensitivity.changed()) svModMixer.gain(0, sensitivity);
    if (enableChanged || freq.changed()) svFilter.frequency(freq);
    if (enableChanged || resonance.changed()) svFilter.resonance(resonance);
    if (enableChanged || rate.changed()) filterLfo.frequency(rate);
    if (enableChanged || depth.changed()) svModMixer.gain(1, depth);
    if (enableChanged || shape.changed()) switch ((int)shape)
    {
      case 1: filterLfo.begin(WAVEFORM_TRIANGLE); break;
      case 2: filterLfo.begin(WAVEFORM_SAWTOOTH); break;
      case 3: filterLfo.begin(WAVEFORM_SAWTOOTH_REVERSE); break;
      case 4: filterLfo.begin(WAVEFORM_SQUARE); break;
      default: filterLfo.begin(WAVEFORM_SINE); break;
    }      
    if (enableChanged || smoothing.changed()) svFilter.smoothing(smoothing);
    if (enableChanged || lp.changed()) filterMixer.gain(1, lp);
    if (enableChanged || bp.changed()) filterMixer.gain(2, bp);
    if (enableChanged || hp.changed()) filterMixer.gain(3, hp);
    if (enableChanged || dry.changed()) filterMixer.gain(0, dry);
  }
  else
  {
    filterLfo.amplitude(0.0f);
    svModMixer.gain(0, 0.0f);
    svModMixer.gain(1, 0.0f);
    filterMixer.gain(0, 1.0f);
    filterMixer.gain(1, 0.0f);
    filterMixer.gain(2, 0.0f);
    filterMixer.gain(3, 0.0f);    
  }
});

EPARAMS(phaser)
{
  Parameter("preset", Parameter::PT_Enum, 0.0f, 0.0f, 4.0f, 1.0f, (const char*[]){" p_90", " p_45", "p_100", " evh", "sm_st"}),  
  Parameter("rate", Parameter::PT_Float, 0.02f),
  Parameter("dry", Parameter::PT_Float, 0.7f),
  Parameter("wet", Parameter::PT_Float, 0.7f),
  Parameter("rt_x", Parameter::PT_Coeff, 2.0f, 0.01f, 4.0f, 0.01f),
  Parameter("rt_z", Parameter::PT_Coeff, 0.01f),
  Parameter("rt_r", Parameter::PT_Coeff, 10.0f, 0.0f, 10.0f, 0.01f),
  Parameter("stages", Parameter::PT_Coeff, 4.0f, 2.0f, 8.0f, 1.0f),
  Parameter("resonance", Parameter::PT_Coeff, 0.3f),
  Parameter("smoothing", Parameter::PT_Coeff, 20.0f, 0.0f, 500.0f),
  Parameter("f1", Parameter::PT_Coeff, 100.0f, 50.0f, 500.0f, 1.0f),
  Parameter("f2", Parameter::PT_Coeff, 900.0f, 500.0f, 5000.0f, 10.0f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(phaser)
{
  int i = 0;
  Parameter& preset = e[i++];  
  Parameter& rate = e[i++];  
  Parameter& dry = e[i++];
  Parameter& wet = e[i++];
  Parameter& rt_x = e[i++];
  Parameter& rt_z = e[i++];
  Parameter& rt_r = e[i++];
  Parameter& stages = e[i++];
  Parameter& resonance = e[i++];
  Parameter& smoothing = e[i++];
  Parameter& f1 = e[i++];
  Parameter& f2 = e[i++];
  Parameter& enable = e[i++];

  if (enable)
  {
    if (preset.changed())
    {
      switch ((int)preset)
      {
        case 0:
          rt_x.reset();
          rt_z.reset();
          rt_r.reset();
          stages.reset();
          resonance.reset();
          smoothing.reset();
          f1.reset();
          f2.reset();
          break;
        case 1:
          rt_x.change(2.0f);
          rt_z.change(0.01f);
          rt_r.change(10.0f);
          stages.change(2);
          resonance.change(0.0f);
          smoothing.change(20.0f);
          f1.change(50.0f);
          f2.change(2000.0f);
          break;
        case 2:
          rt_x.change(2.0f);
          rt_z.change(0.01f);
          rt_r.change(10.0f);
          stages.change(8);
          resonance.change(0.75f);
          smoothing.change(20.0f);
          f1.change(280.0f);
          f2.change(2000.0f);
          break;
        case 3:
          rt_x.change(2.0f);
          rt_z.change(0.01f);
          rt_r.change(10.0f);
          stages.change(4);
          resonance.change(0.9f);
          smoothing.change(5.0f);
          f1.change(150.0f);
          f2.change(1600.0f);
          break;
        case 4:
          rt_x.change(2.0f);
          rt_z.change(0.01f);
          rt_r.change(10.0f);
          stages.change(6);
          resonance.change(0.7f);
          smoothing.change(100.0f);
          f1.change(90.0f);
          f2.change(1300.0f);
          break;
        default:
          break;
      }
    }
    
    if (enable.changed() || rate.changed() || rt_x.changed() || rt_z.changed() || rt_r.changed()) phaser.rate((powf(rate, rt_x) * rt_r) + rt_z);
    if (enable.changed() || dry.changed()) phaser.dry(dry);
    if (enable.changed() || wet.changed()) phaser.wet(wet);
    if (enable.changed() || stages.changed()) phaser.stages(stages);
    if (enable.changed() || resonance.changed()) phaser.resonance(resonance);
    if (enable.changed() || smoothing.changed()) phaser.smoothing(smoothing);
    if (enable.changed() || f1.changed() || f2.changed()) phaser.frequency(f1, f2);
  }
  else
  {
    phaser.stages(0);
    phaser.dry(1.0f);
    phaser.wet(0.0f);
    phaser.resonance(0.0f);
  }  
});

EPARAMS(distortion)
{
  Parameter("gain", Parameter::PT_Float, 0.75f),
  Parameter("bottom", Parameter::PT_Float, 0.5f),
  Parameter("tone", Parameter::PT_Float, 0.9f),
  Parameter("bass", Parameter::PT_Float, 0.5f),
  Parameter("middle", Parameter::PT_Float, 0.5f),
  Parameter("treble", Parameter::PT_Float, 0.5f),
  Parameter("level", Parameter::PT_Float, 0.4f),  
  Parameter("gg_x", Parameter::PT_Coeff, 1.5f, 0.01f, 4.0f, 0.01f),
  Parameter("gg_z", Parameter::PT_Coeff, 10.0f, 0.0f, 100.0f, 1.0f),
  Parameter("gg_r", Parameter::PT_Coeff, 400.0f, 0.0f, 500.0f, 1.0f),
  Parameter("ll_x", Parameter::PT_Coeff, 1.5f, 0.01f, 4.0f, 0.01f),
  Parameter("ll_z", Parameter::PT_Coeff, 0.01f, 0.0f, 1.0f, 0.01f),
  Parameter("ll_r", Parameter::PT_Coeff, 0.27f, 0.0f, 2.0f, 0.02f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(distortion)
{
  int i = 0;
  Parameter& gain = e[i++];
  Parameter& bottom = e[i++];
  Parameter& tone = e[i++];
  Parameter& bass = e[i++];
  Parameter& middle = e[i++];
  Parameter& treble = e[i++];
  Parameter& level = e[i++];  
  Parameter& gg_x = e[i++];
  Parameter& gg_z = e[i++];
  Parameter& gg_r = e[i++];
  Parameter& ll_x = e[i++];
  Parameter& ll_z = e[i++];
  Parameter& ll_r = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enable)
  {
    if (enableChanged || gain.changed() || gg_z.changed() || gg_x.changed() || gg_r.changed()) distortion.gain((powf(gain, gg_x) * gg_r) + gg_z);
    if (enableChanged || bottom.changed()) distortion.bottom(bottom);
    if (enableChanged || tone.changed()) distortion.tone(tone);
    if (enableChanged || level.changed() || ll_z.changed() || ll_x.changed() || ll_r.changed()) distortion.level((powf(level, ll_x) * ll_r) + ll_z);
  }
  else 
  {
    distortion.begin();
  }

  if (enableChanged || bass.changed() || middle.changed() || treble.changed())
  {
    //toneStack.setGain(1.5f);
    toneStack.setTone(bass, middle, treble);
  }
});

EPARAMS(tremolo)
{
  Parameter("rate", Parameter::PT_Float, 5.0f, 0.1f, 50.0f, 0.1f),
  Parameter("depth", Parameter::PT_Float, 0.25f),
  Parameter("shape", Parameter::PT_Enum, 0.0f, 0.0f, 4.0f, 1.0f, (const char*[]){"sin", "tri", "saw", "rev", "squ"}),
  Parameter("smoothing", Parameter::PT_Float, 20.0f, 0.0f, 500.0f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(tremolo)
{
  int i = 0;
  Parameter& rate = e[i++];
  Parameter& depth = e[i++];
  Parameter& shape = e[i++];
  Parameter& smoothing = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enable)
  {
    if (enableChanged || rate.changed()) tremoloLfo.frequency(rate);
    if (enableChanged || depth.changed())
    { 
      tremoloLfo.amplitude(depth);
      tremoloVca.offset(1.0f - depth);
    }
    if (enableChanged || shape.changed()) switch ((int)shape)
    {
      case 1: tremoloLfo.begin(WAVEFORM_TRIANGLE); break;
      case 2: tremoloLfo.begin(WAVEFORM_SAWTOOTH); break;
      case 3: tremoloLfo.begin(WAVEFORM_SAWTOOTH_REVERSE); break;
      case 4: tremoloLfo.begin(WAVEFORM_SQUARE); break;
      default: tremoloLfo.begin(WAVEFORM_SINE); break;
    }
    if (enableChanged || smoothing.changed()) tremoloVca.smoothing(smoothing);
  }
  else
  {
    tremoloLfo.amplitude(0.0f);
    tremoloVca.offset(0.0f);
  }
});


EPARAMS(chorus)
{
  Parameter("preset", Parameter::PT_Enum, 0.0f, 0.0f, 7.0f, 1.0f, (const char*[]){" def", " ce2", " bf2", " tsc", " dd1", " dd2", " dd3", " dd4"}),
  Parameter("rate", Parameter::PT_Float, 0.5f),
  Parameter("depth", Parameter::PT_Float, 0.5f),
  Parameter("delay", Parameter::PT_Float, 0.5f),
  Parameter("resonance", Parameter::PT_Float, 0.5f),
  Parameter("dry", Parameter::PT_Float, 0.9f),
  Parameter("wet", Parameter::PT_Float, 0.7f),  
  Parameter("hp_f", Parameter::PT_Coeff, 150.0f, 50.0f, 800.0f, 10.0f),
  Parameter("lp_f", Parameter::PT_Coeff, 13000.0f, 4000.0f, 13000.0f, 250.0f),
  Parameter("rt_x", Parameter::PT_Coeff, 2.0f, 0.01f, 4.0f, 0.01f),
  Parameter("rt_z", Parameter::PT_Coeff, 0.4f, 0.0f, 10.0f, 0.1f),
  Parameter("rt_l", Parameter::PT_Coeff, 3.0f, 0.0f, 10.0f, 0.1f),
  Parameter("rt_c", Parameter::PT_Coeff, 1.5f, 0.0f, 10.0f, 0.1f),
  Parameter("rt_r", Parameter::PT_Coeff, 3.0f, 0.0f, 10.0f, 0.1f),  
  Parameter("dp_x", Parameter::PT_Coeff, 2.0f, 0.01f, 4.0f, 0.01f),
  Parameter("dp_z", Parameter::PT_Coeff, 0.0f),
  Parameter("dp_l", Parameter::PT_Coeff, 0.12f),  
  Parameter("dp_c", Parameter::PT_Coeff, 0.20f),  
  Parameter("dp_r", Parameter::PT_Coeff, 0.12f),  
  Parameter("in_l", Parameter::PT_Coeff, 1.0f, -1.0f, 1.0f, 0.02f),
  Parameter("in_c", Parameter::PT_Coeff, 1.0f, -1.0f, 1.0f, 0.02f),
  Parameter("in_r", Parameter::PT_Coeff, 1.0f, -1.0f, 1.0f, 0.02f),  
  Parameter("fb_x", Parameter::PT_Coeff, 2.0f, 0.01f, 4.0f, 0.01f),
  Parameter("fb_z", Parameter::PT_Coeff, 0.0f),
  Parameter("fb_l", Parameter::PT_Coeff, 0.0f, -1.0f, 1.0f, 0.02f),
  Parameter("fb_cl", Parameter::PT_Coeff, -0.4f, -1.0f, 1.0f, 0.02f),
  Parameter("fb_cc", Parameter::PT_Coeff, 0.7f, -1.0f, 1.0f, 0.02f),
  Parameter("fb_cr", Parameter::PT_Coeff, -0.4f, -1.0f, 1.0f, 0.02f),
  Parameter("fb_r", Parameter::PT_Coeff, 0.0f, -1.0f, 1.0f, 0.02f),  
  Parameter("w_ll", Parameter::PT_Coeff, 0.9f, -1.0f, 1.0f, 0.02f),
  Parameter("w_lr", Parameter::PT_Coeff, 0.0f, -1.0f, 1.0f, 0.02f),
  Parameter("w_cl", Parameter::PT_Coeff, 0.75f, -1.0f, 1.0f, 0.02f),
  Parameter("w_cr", Parameter::PT_Coeff, -0.75f, -1.0f, 1.0f, 0.02f),
  Parameter("w_rl", Parameter::PT_Coeff, 0.0f, -1.0f, 1.0f, 0.02f),  
  Parameter("w_rr", Parameter::PT_Coeff, 0.9f, -1.0f, 1.0f, 0.02f),
  Parameter("ph_c", Parameter::PT_Coeff, 90.0f, 0.0f, 359.0f, 1.0f),
  Parameter("ph_r", Parameter::PT_Coeff, 120.0f, 0.0f, 359.0f, 1.0f),  
  Parameter("sh_l", Parameter::PT_Enum, 0.0f, 0.0f, 1.0f, 1.0f, (const char*[]){" tri", "sine"}),
  Parameter("sh_c", Parameter::PT_Enum, 0.0f, 0.0f, 1.0f, 1.0f, (const char*[]){" tri", "sine"}),
  Parameter("sh_r", Parameter::PT_Enum, 0.0f, 0.0f, 1.0f, 1.0f, (const char*[]){" tri", "sine"}),
  Parameter("dl_x", Parameter::PT_Coeff, 0.7f, 0.01f, 4.0f, 0.01f),
  Parameter("dl_z", Parameter::PT_Coeff, 2.0f, 0.0f, 25.0f, 0.1f),
  Parameter("dl_l", Parameter::PT_Coeff, 22.0f, 0.0f, 25.0f, 0.5f),
  Parameter("dl_c", Parameter::PT_Coeff, 0.0f, 0.0f, 25.0f, 0.5f),
  Parameter("dl_r", Parameter::PT_Coeff, 23.0f, 0.0f, 25.0f, 0.5f),  
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(chorus)
{
  int i = 0;
  Parameter& preset = e[i++];
  Parameter& rate = e[i++];
  Parameter& depth = e[i++];
  Parameter& delay = e[i++];
  Parameter& resonance = e[i++];
  Parameter& dry = e[i++];
  Parameter& wet = e[i++];  
  Parameter& hp_f = e[i++];
  Parameter& lp_f = e[i++];
  Parameter& rt_x = e[i++];
  Parameter& rt_z = e[i++];
  Parameter& rt_l = e[i++];
  Parameter& rt_c = e[i++];
  Parameter& rt_r = e[i++];
  Parameter& dp_x = e[i++];
  Parameter& dp_z = e[i++];
  Parameter& dp_l = e[i++];
  Parameter& dp_c = e[i++];
  Parameter& dp_r = e[i++];  
  Parameter& in_l = e[i++];
  Parameter& in_c = e[i++];
  Parameter& in_r = e[i++];  
  Parameter& fb_x = e[i++];
  Parameter& fb_z = e[i++];
  Parameter& fb_l = e[i++];
  Parameter& fb_cl = e[i++];
  Parameter& fb_cc = e[i++];
  Parameter& fb_cr = e[i++];
  Parameter& fb_r = e[i++];  
  Parameter& w_ll = e[i++];
  Parameter& w_lr = e[i++];
  Parameter& w_cl = e[i++];
  Parameter& w_cr = e[i++];
  Parameter& w_rl = e[i++];  
  Parameter& w_rr = e[i++];
  Parameter& ph_c = e[i++];
  Parameter& ph_r = e[i++];  
  Parameter& sh_l = e[i++];
  Parameter& sh_c = e[i++];
  Parameter& sh_r = e[i++];
  Parameter& dl_x = e[i++];
  Parameter& dl_z = e[i++];
  Parameter& dl_l = e[i++];
  Parameter& dl_c = e[i++];
  Parameter& dl_r = e[i++];  
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enable)
  {
    if (preset.changed())
    {
      switch ((int)preset)
      {
        case 0:
          hp_f.reset();
          lp_f.reset();
          rt_z.reset();
          rt_x.reset();
          rt_l.reset();
          rt_c.reset();
          rt_r.reset();
          dp_z.reset();
          dp_x.reset();
          dp_l.reset();
          dp_c.reset();
          dp_r.reset();
          in_l.reset();
          in_c.reset();
          in_r.reset();
          fb_z.reset();
          fb_x.reset();
          fb_l.reset();
          fb_cl.reset();
          fb_cc.reset();
          fb_cr.reset();
          fb_r.reset();
          w_ll.reset();
          w_lr.reset();
          w_cl.reset();
          w_cr.reset();
          w_rr.reset();
          w_rl.reset();
          ph_c.reset();
          ph_r.reset();
          sh_l.reset();
          sh_c.reset();
          sh_r.reset();
          dl_z.reset();
          dl_x.reset();
          dl_l.reset();
          dl_c.reset();
          dl_r.reset();
          break;
        case 1: // CE2
          hp_f.change(250.0f);
          lp_f.change(6900.0f);
          rt_z.change(0.3f);
          rt_x.change(2.0f);
          rt_c.change(3.2f);
          dp_z.change(0.025f);
          dp_x.change(2.0f);
          dp_c.change(0.55f);
          in_l.change(0.0f);
          in_c.change(1.0f);
          in_r.change(0.0f);
          fb_z.change(0.0f);
          fb_cl.change(0.0f);
          fb_cc.change(0.0f);
          fb_cr.change(0.0f);
          w_ll.change(0.0f);
          w_lr.change(0.0f);
          w_cl.change(0.9f);
          w_cr.change(-0.9f);
          w_rr.change(0.0f);
          w_rl.change(0.0f);
          ph_c.change(0.0f);
          sh_c.change(0.0f);
          dl_z.change(6.8f);
          dl_c.change(0.0f);
          break;
        case 2: // BF2
          hp_f.change(350.0f);
          lp_f.change(6000.0f);
          rt_z.change(0.0625f);
          rt_x.change(3.5f);
          rt_c.change(7.0f);
          dp_z.change(0.020f);
          dp_x.change(1.0f);
          dp_c.change(0.5f);
          in_l.change(0.0f);
          in_c.change(1.0f);
          in_r.change(0.0f);
          fb_z.change(0.45f);
          fb_x.change(0.3f);
          fb_cl.change(0.0f);
          fb_cc.change(0.5f);
          fb_cr.change(0.0f);
          w_ll.change(0.0f);
          w_lr.change(0.0f);
          w_cl.change(1.0f);
          w_cr.change(-1.0f);
          w_rr.change(0.0f);
          w_rl.change(0.0f);
          ph_c.change(0.0f);
          sh_c.change(0.0f);
          dl_z.change(0.5f);
          dl_x.change(4.0f);
          dl_c.change(6.0f);
          break;
        case 3: // TSC
          hp_f.change(180.0f);
          lp_f.change(11000.0f);
          rt_z.change(0.10f);
          rt_x.change(4.0f);
          rt_l.change(5.0f);
          rt_c.change(5.0f);
          rt_r.change(5.0f);
          dp_z.change(0.10f);
          dp_x.change(3.0f);
          dp_l.change(0.17f);
          dp_c.change(0.17f);
          dp_r.change(0.17f);
          in_l.change(1.0f);
          in_c.change(1.0f);
          in_r.change(1.0f);
          fb_z.change(0.2f);
          fb_x.change(2.0f);
          fb_l.change(0.2f);
          fb_cl.change(-0.2f);
          fb_cc.change(0.3f);
          fb_cr.change(-0.2f);
          fb_r.change(0.2f);
          w_ll.change(0.8f);
          w_lr.change(0.0f);
          w_cl.change(-0.4f);
          w_cr.change(0.4f);
          w_rr.change(0.8f);
          w_rl.change(0.0f);
          ph_c.change(120.0f);
          ph_r.change(240.0f);
          sh_l.change(1.0f);
          sh_c.change(1.0f);
          sh_r.change(1.0f);
          dl_z.change(15.0f);
          dl_l.change(0.0f);
          dl_c.change(0.0f);
          dl_r.change(0.0f);
          break;
        case 4:
        case 5:
        case 6:
        case 7:
          hp_f.change(150.0f);
          lp_f.change(9000.0f);
          rt_x.change(2.0000f);
          rt_z.change((int)preset < 7 ? 0.25f : 0.5f);
          rt_l.change(0.0f);
          rt_c.change(0.0f);
          rt_r.change(0.0f);
          dp_x.change(2.0f);
          dp_z.change((int)preset == 5 ? 0.33333f : 0.2f);
          dp_l.change(0.0f);
          dp_c.change(0.0f);
          dp_r.change(0.0f);
          in_l.change(1.0f);
          in_c.change(0.0f);
          in_r.change(1.0f);
          fb_x.change(2.0f);
          fb_z.change(0.0f);
          fb_l.change(0.0f);
          fb_cl.change(0.0f);
          fb_cc.change(0.0f);
          fb_cr.change(0.0f);
          fb_r.change(0.0f);
          w_ll.change((int)preset == 7 ? 0.32f : 1.0f);
          w_lr.change(-0.6f);
          w_cl.change(0.0f);
          w_cr.change(0.0f);
          w_rl.change(-0.6f);
          w_rr.change((int)preset == 7 ? 0.32f : 1.0f);
          ph_c.change(0.0000f);
          ph_r.change(180.0810f);
          sh_l.change(0);
          sh_c.change(0);
          sh_r.change(0);
          dl_x.change(1.0000f);
          dl_z.change((int)preset == 4 ? 10.0f : 7.5f);
          dl_l.change(0.0f);
          dl_c.change(0.0f);
          dl_r.change(0.0f);        
          break;
        default:
          break;
      }
    }

    if (enableChanged || hp_f.changed() || lp_f.changed())
    {
      chorusPostFilterL.doClassInit();
      chorusPostFilterL.setHighpass(0, hp_f, 0.7071f);
      chorusPostFilterL.setLowpass(1, lp_f, 0.7071f);

      chorusPostFilterC.doClassInit();
      chorusPostFilterC.setHighpass(0, hp_f, 0.7071f);
      chorusPostFilterC.setLowpass(1, lp_f, 0.7071f);

      chorusPostFilterR.doClassInit();
      chorusPostFilterR.setHighpass(0, hp_f, 0.7071f);
      chorusPostFilterR.setLowpass(1, lp_f, 0.7071f);

      chorusPostFilterL.begin();
      chorusPostFilterC.begin();
      chorusPostFilterR.begin();      
    }

    float freq = powf(rate, rt_x);
    bool freqChanged = enableChanged || rate.changed() || rt_x.changed() || rt_z.changed();
    if (freqChanged || rt_l.changed()) chorusLfoL.frequency((freq * rt_l) + rt_z);
    if (freqChanged || rt_c.changed()) chorusLfoC.frequency((freq * rt_c) + rt_z);
    if (freqChanged || rt_r.changed()) chorusLfoR.frequency((freq * rt_r) + rt_z);

    float amp = powf(depth, dp_x);
    bool ampChanged = enableChanged || depth.changed() || dp_x.changed() || dp_z.changed();
    if (ampChanged || dp_l.changed()) chorusLfoL.amplitude((amp * dp_l) + dp_z);
    if (ampChanged || dp_c.changed()) chorusLfoC.amplitude((amp * dp_c) + dp_z);
    if (ampChanged || dp_r.changed()) chorusLfoR.amplitude((amp * dp_r) + dp_z);

    if (enableChanged || in_l.changed()) chorusInputL.gain(0, in_l);
    if (enableChanged || in_c.changed()) chorusInputC.gain(0, in_c);
    if (enableChanged || in_r.changed()) chorusInputR.gain(0, in_r);
    
    float feedback = powf(resonance, fb_x);
    bool feedbackChanged = enableChanged || resonance.changed() || fb_x.changed() || fb_z.changed();
    if (feedbackChanged || fb_l.changed()) chorusInputL.gain(1, (feedback * fb_l) + fb_z);
    if (feedbackChanged || fb_cl.changed()) chorusInputL.gain(2, (feedback * fb_cl) + fb_z);
    if (feedbackChanged || fb_cc.changed()) chorusInputC.gain(1, (feedback * fb_cc) + fb_z);
    if (feedbackChanged || fb_cr.changed()) chorusInputR.gain(2, (feedback * fb_cr) + fb_z);
    if (feedbackChanged || fb_r.changed()) chorusInputR.gain(1, (feedback * fb_r) + fb_z);

    if (enableChanged || dry.changed())
    {
      chorusMixerL.gain(0, dry);
      chorusMixerR.gain(0, dry);
    }

    if (enableChanged || wet.changed() || w_ll.changed()) chorusMixerL.gain(1, wet * w_ll);
    if (enableChanged || wet.changed() || w_lr.changed()) chorusMixerR.gain(1, wet * w_lr);

    if (enableChanged || wet.changed() || w_cl.changed()) chorusMixerL.gain(2, wet * w_cl);
    if (enableChanged || wet.changed() || w_cr.changed()) chorusMixerR.gain(2, wet * w_cr);

    if (enableChanged || wet.changed() || w_rl.changed()) chorusMixerL.gain(3, wet * w_rl);
    if (enableChanged || wet.changed() || w_rr.changed()) chorusMixerR.gain(3, wet * w_rr);

    if (enableChanged || sh_l.changed() || sh_c.changed() || sh_r.changed() || ph_c.changed() || ph_r.changed())
    {
      AudioNoInterrupts();
      chorusLfoL.begin(sh_l ? WAVEFORM_SINE : WAVEFORM_TRIANGLE);
      chorusLfoC.begin(sh_c ? WAVEFORM_SINE : WAVEFORM_TRIANGLE);
      chorusLfoR.begin(sh_r ? WAVEFORM_SINE : WAVEFORM_TRIANGLE);
      chorusLfoL.phase(0.0f);
      chorusLfoC.phase(ph_c);
      chorusLfoR.phase(ph_r);
      AudioInterrupts();
    }

    bool delayChanged = enableChanged || delay.changed() || dl_x.changed() || dl_z.changed();
    float delayTime = powf(delay, dl_x);
    if (delayChanged || dl_l.changed()) chorusModDelayL.delay((delayTime * dl_l) + dl_z);
    if (delayChanged || dl_c.changed()) chorusModDelayC.delay((delayTime * dl_c) + dl_z);
    if (delayChanged || dl_r.changed()) chorusModDelayR.delay((delayTime * dl_r) + dl_z);
  }
  else
  {
    chorusMixerL.gain(0, 1.0f);
    chorusMixerL.gain(1, 0.0f);
    chorusMixerL.gain(2, 0.0f);
    chorusMixerL.gain(3, 0.0f);
    chorusMixerR.gain(0, 1.0f);
    chorusMixerR.gain(1, 0.0f);
    chorusMixerR.gain(2, 0.0f);
    chorusMixerR.gain(3, 0.0f);
  }
});

EPARAMS(reverb)
{
  Parameter("size", Parameter::PT_Float, 0.4f),
  Parameter("lo_damp", Parameter::PT_Float, 0.3f),
  Parameter("hi_damp", Parameter::PT_Float, 0.1f),
  Parameter("wet", Parameter::PT_Float, 0.13f),
  Parameter("dry", Parameter::PT_Float, 0.9f),
  Parameter("lp", Parameter::PT_Float, 0.8f),
  Parameter("diffusion", Parameter::PT_Float, 0.65f),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(reverb)
{
  int i = 0;
  Parameter& size = e[i++];
  Parameter& lo_damp = e[i++];
  Parameter& hi_damp = e[i++];
  Parameter& wet = e[i++];
  Parameter& dry = e[i++];
  Parameter& lp = e[i++];
  Parameter& diffusion = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  
  if (enable)
  {
    if (enableChanged || wet.changed() || dry.changed())
    {
      reverbMixerL.gain(0, dry);
      reverbMixerL.gain(1, wet);
      reverbMixerR.gain(0, dry);
      reverbMixerR.gain(1, wet);
    }

/*
    if (enableChanged || size.changed()) plateReverb.size(size);
    if (enableChanged || lo_damp.changed()) plateReverb.lodamp(lo_damp);
    if (enableChanged || hi_damp.changed()) plateReverb.hidamp(hi_damp);
    if (enableChanged || lp.changed()) plateReverb.lowpass(lp);
    if (enableChanged || diffusion.changed()) plateReverb.diffusion(diffusion);
    */
  }
  else
  {
    reverbMixerL.gain(0, 1.0f);
    reverbMixerL.gain(1, 0.0f);
    reverbMixerR.gain(0, 1.0f);
    reverbMixerR.gain(1, 0.0f);
  }
});

EPARAMS(delay)
{
  Parameter("preset", Parameter::PT_Enum, 0.0f, 0.0f, 1.0f, 1.0f, (const char*[]){" def", " dd1"}),
  Parameter("delay", Parameter::PT_Float, 0.5f, 0.01f, 1.0f, 0.01f),
  Parameter("feedback", Parameter::PT_Float, 0.30f),
  Parameter("wet", Parameter::PT_Float, 0.15f),
  Parameter("dry", Parameter::PT_Float, 1.0f),
  Parameter("hp_f", Parameter::PT_Coeff, 150.0f, 50.0f, 800.0f, 10.0f),
  Parameter("lp_f", Parameter::PT_Coeff, 8500.0f, 3000.0f, 18000.0f, 250.0f),
  Parameter("in_l", Parameter::PT_Coeff, 1.0f),
  Parameter("in_r", Parameter::PT_Coeff, 1.0f),
  Parameter("fb_x", Parameter::PT_Coeff, 1.0f, 0.01f, 4.0f, 0.01f),
  Parameter("fb_z", Parameter::PT_Coeff, 0.0f),
  Parameter("fb_ll", Parameter::PT_Coeff, 1.0f),
  Parameter("fb_lr", Parameter::PT_Coeff, 0.0f),
  Parameter("fb_rl", Parameter::PT_Coeff, 0.0f),
  Parameter("fb_rr", Parameter::PT_Coeff, 1.0f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(delay)
{
  int i = 0;
  Parameter& preset = e[i++];
  Parameter& delay = e[i++];
  Parameter& feedback = e[i++];
  Parameter& wet = e[i++];
  Parameter& dry = e[i++];
  Parameter& hp_f = e[i++];
  Parameter& lp_f = e[i++];
  Parameter& in_l = e[i++];
  Parameter& in_r = e[i++];
  Parameter& fb_x = e[i++];
  Parameter& fb_z = e[i++];
  Parameter& fb_ll = e[i++];
  Parameter& fb_lr = e[i++];
  Parameter& fb_rl = e[i++];
  Parameter& fb_rr = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  bool filterChanged = false;

  if (enable)
  {
    if (preset.changed())
    {
      switch ((int)preset)
      {
        case 0:
          hp_f.reset();
          lp_f.reset();
          in_l.reset();
          in_r.reset();
          fb_x.reset();
          fb_z.reset();
          fb_ll.reset();
          fb_lr.reset();
          fb_rl.reset();
          fb_rr.reset();
          break;
        case 1:
          hp_f.change(50.0f);
          lp_f.change(15000.0f);
          in_l.change(1.0f);
          in_r.change(1.0f);
          fb_x.change(1.0f);
          fb_z.change(0.0f);
          fb_ll.change(1.0f);
          fb_lr.change(0.0f);
          fb_rl.change(0.0f);
          fb_rr.change(1.0f);
          break;
        default:
          break;
      }
    }
    
    if (enableChanged)
    {
      delayFilterL.doClassInit();
      delayFilterR.doClassInit();
      filterChanged = true;
    }
    
    if (enableChanged || delay.changed())
    {
      delayL.delay(delay * 1000.0f);
      delayR.delay(delay * 1000.0f);
    }
    
    if (enableChanged || feedback.changed())
    {
      delayFeedbackL.gain(1, feedback);
      delayFeedbackR.gain(1, feedback);
    }
    
    if (enableChanged || wet.changed())
    {
      delayMixerL.gain(1, wet);
      delayMixerR.gain(1, wet);
    }
    
    if (enableChanged || dry.changed())
    {
      delayMixerL.gain(0, dry);
      delayMixerR.gain(0, dry);
    }

    if (enableChanged || hp_f.changed())
    {
      delayFilterL.setHighpass(0, hp_f, 0.7071f);
      delayFilterR.setHighpass(0, hp_f, 0.7071f);
      filterChanged = true;
    }
    
    if (enableChanged || lp_f.changed())
    {
      delayFilterL.setLowpass(1, lp_f, 0.7071f);
      delayFilterR.setLowpass(1, lp_f, 0.7071f);
      filterChanged = true;
    }

    if (enableChanged || in_l.changed()) delayFeedbackL.gain(0, in_l);
    if (enableChanged || in_r.changed()) delayFeedbackR.gain(0, in_r);

    float fbk = powf(feedback, fb_x);
    bool feedbackChanged = enableChanged || feedback.changed() || fb_x.changed() || fb_z.changed();
    if (feedbackChanged || fb_ll.changed()) delayFeedbackL.gain(1, (fbk * fb_ll) + fb_z);
    if (feedbackChanged || fb_lr.changed()) delayFeedbackR.gain(2, (fbk * fb_lr) + fb_z);
    if (feedbackChanged || fb_rl.changed()) delayFeedbackL.gain(2, (fbk * fb_rl) + fb_z);
    if (feedbackChanged || fb_rr.changed()) delayFeedbackR.gain(1, (fbk * fb_rr) + fb_z);

    if (filterChanged)
    {
      delayFilterL.begin();
      delayFilterR.begin();
    }    
  }
  else
  {
    delayFeedbackL.gain(0, 0.0f);
    delayFeedbackL.gain(1, 0.0f);
    delayFeedbackL.gain(2, 0.0f);
    delayFeedbackR.gain(0, 0.0f);
    delayFeedbackR.gain(1, 0.0f);
    delayFeedbackR.gain(2, 0.0f);

    delayMixerL.gain(0, 1.0f);
    delayMixerR.gain(0, 1.0f);
    delayMixerL.gain(1, 0.0f);
    delayMixerR.gain(1, 0.0f);
    delayMixerL.gain(2, 0.0f);
    delayMixerR.gain(2, 0.0f);
  }

});

EPARAMS(cab_sim)
{
  Parameter("hp_f", Parameter::PT_Float, 280.0f, 50.0f, 800.0f, 10.0f),
  Parameter("pk_f", Parameter::PT_Float, 1300.0f, 800.0f, 1600.0f, 25.0f),
  Parameter("pk_g", Parameter::PT_Float, -14.0f, -18.0f, 18.0f, 0.5f),
  Parameter("l1_f", Parameter::PT_Float, 3500.0f, 3000.0f, 15000.0f, 250.0f),
  Parameter("l1_q", Parameter::PT_Float, 1.4, 0.5f, 3.5f, 0.05f),  
  Parameter("l2_f", Parameter::PT_Float, 14000.0f, 3000.0f, 15000.0f, 250.0f),
  Parameter("l2_q", Parameter::PT_Float, 0.7, 0.5f, 3.5f, 0.05f),  
  Parameter("size", Parameter::PT_Float, 0.20f),
  Parameter("damping", Parameter::PT_Float, 0.8f),
  Parameter("delay", Parameter::PT_Float, 6.5f, 0.0f, 200.0f, 0.01f),
  Parameter("direct", Parameter::PT_Float, 2.0f, 0.0f, 5.0f, 0.05f),
  Parameter("room", Parameter::PT_Float, 0.35f),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(cab_sim)
{
  int i = 0;
  Parameter& hp_f = e[i++];
  Parameter& pk_f = e[i++];
  Parameter& pk_g = e[i++];
  Parameter& l1_f = e[i++];
  Parameter& l1_q = e[i++]; 
  Parameter& l2_f = e[i++];
  Parameter& l2_q = e[i++]; 
  Parameter& size = e[i++];
  Parameter& damping = e[i++];
  Parameter& delay = e[i++];
  Parameter& direct = e[i++];
  Parameter& room = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  bool filterChanged = false;
  
  if (enableChanged)
  {
    cabSimL.doClassInit();
    cabSimR.doClassInit();
    filterChanged = true;
  }

  if (enable)
  {
    if (enableChanged)
    {
      roomMixer.gain(0, 0.8f);
      roomMixer.gain(1, 0.2f);
  
      roomFilter.doClassInit();
      roomFilter.setHighpass(0, 350.0f, 0.7071f);
      roomFilter.setLowpass(1, 12000.0f, 0.7071f);
      roomFilter.begin();
    }
    
    if (enableChanged || hp_f.changed())
    {
      cabSimL.setHighpass(0, hp_f, 0.7071f);
      cabSimR.setHighpass(0, hp_f, 0.7071f);
      filterChanged = true;
    }

    if (enableChanged || pk_f.changed() || pk_g.changed())
    {
      setPeak(cabSimL, 1, pk_f, pk_g, 0.5f);
      setPeak(cabSimR, 1, pk_f, pk_g, 0.5f);
      filterChanged = true;
    }

    if (enableChanged || l1_f.changed() || l1_q.changed())
    {
      cabSimL.setLowpass(2, l1_f, l1_q);
      cabSimR.setLowpass(2, l1_f, l1_q);
      filterChanged = true;
    }

    if (enableChanged || l2_f.changed() || l2_q.changed())
    {
      cabSimL.setLowpass(3, l2_f, l2_q);
      cabSimR.setLowpass(3, l2_f, l2_q);
      filterChanged = true;
    }

    if (enableChanged || size.changed()) roomReverb.roomsize(size);
    if (enableChanged || damping.changed()) roomReverb.damping(damping);
    if (enableChanged || delay.changed())
    {
      roomDelayL.delay(delay);
      roomDelayR.delay(delay * 0.75f);
    }

    if (enableChanged || direct.changed())
    {
      cabSimMixerL.gain(0, direct);
      cabSimMixerR.gain(0, direct);
    }

    if (enableChanged || room.changed())
    {
      cabSimMixerL.gain(1, room * 0.75f);
      cabSimMixerR.gain(1, room);
    }
  }
  else
  {
    cabSimMixerL.gain(0, 1.0f);
    cabSimMixerL.gain(1, 0.0f);
    cabSimMixerR.gain(0, 1.0f);
    cabSimMixerR.gain(1, 0.0f);  
  }

  if (filterChanged)
  {
    cabSimL.begin();
    cabSimR.begin();
  }
});


/**********************************************************************************************************************************/








typedef enum
{
  r_select_effect,
  r_select_param,
  r_select_param_back,
  r_set_value,
  r_binding,
  r_changed_value,
} r_state_t;

r_state_t _r_state;
r_state_t _r_prev;
elapsedMillis _lastStateChange;
unsigned long _changeTimeout = 0;

void displayEffectName(Effect* effect = nullptr)
{
  if (effect == nullptr) effect = Effect::current();
  print(effect->name());
}

void displayParamName(Effect* effect = nullptr, Parameter* param = nullptr)
{
  char buf[32];
  if (effect == nullptr) effect = Effect::current();
  if (param == nullptr) param = effect->param();
  snprintf(buf, 32, "%-2.2s.%-6.6s", effect->name(), param->name());
  print(buf);
}

void displayParamValue(Effect* effect = nullptr, Parameter* param = nullptr)
{
  char buf[32];
  if (effect == nullptr) effect = Effect::current();
  if (param == nullptr) param = effect->param();
  snprintf(buf, 32, "%-1.1s.%-3.3s%s", effect->name(), param->name(), param->tos());
  print(buf);
}

bool resetEffect(const char* name)
{
  Effect *e = Effect::effect(name);
  if (e == nullptr) return false;
  
  e->reset();

  return true;
}

bool updateEffect(const char* name, int n, float values[])
{
  Effect *e = Effect::effect(name);
  if (e == nullptr) return false;

  e->change(n, values);
  e->update();
   
  return true;
}

void setState(short newState)
{
  _changeTimeout = 0;
  _lastStateChange = 0;  
  
  switch (newState)
  {
    case r_select_effect:
      displayEffectName();
      break;
    case r_select_param:
    case r_select_param_back:
      displayParamName();
      break;
    case r_set_value:
      displayParamValue();
      break;
    case r_binding:
      print("binding.");
      _changeTimeout = 5000;
      break;
    case r_changed_value:
      _changeTimeout = 1500;
      break;
    default: return;
  }

  if (_r_state != (r_state_t)newState) _r_prev = _r_state;

  _r_state = (r_state_t)newState;
}

void revertState()
{
  setState(_r_prev);
}

bool updateParam(const char* name, const char* param, float value)
{
  Effect *e = Effect::effect(name);
  if (e == nullptr) return false;

  Parameter *p = e->param(param);
  if (p == nullptr) return false;
  
  p->change(value);
  e->update();
  displayParamValue(e, p);
  setState(r_changed_value);

  return true;
}

bool binding()
{
  return _r_state == r_binding;
}

void checkStateExpiration()
{
  if (_changeTimeout > 0 && _lastStateChange > _changeTimeout)
  {
    revertState();
  }
}

class IBindable
{
  public:
    IBindable(int i, const char* name)
    {
      snprintf(_name, _maxlen, "%s%d", name, i);

      _next = _first;
      _first = this;
    }

    bool bind(const char* bname)
    {
      if (bname == nullptr) return false;
      
      char ename[64];
      char pname[64];
      int i = 0;
      int p = 0;
      int len = strnlen(bname, 64);
      for (; i < len; i++)
      {
        if (bname[i] == '.' || bname[i] == 0) break;
        ename[p++] = bname[i];
      }
      ename[p] = 0;

      Effect *effect = Effect::effect(ename);
      if (effect == nullptr) return false;
      
      p = 0;
      i++;
      for (; i < len; i++)
      {
        if (bname[i] == 0) break;
        pname[p++] = bname[i];
      }
      pname[p] = 0;

      Parameter *param = effect->param(pname);
      if (param == nullptr) return false;
      
      _effect = effect;
      _param = param;
      
      onBind();
      //Serial.printf("Bound %s to %s.%s\r\n", _name, _effect->name(), _param->name());
      
      return true;
    }

    static bool bind(const char *name, const char* bname)
    {
      if (name == nullptr || bname == nullptr) return false;
      
      IBindable *b = _first;
      while (b != nullptr)
      {
        if (strncmp(name, b->_name, _maxlen) == 0) return b->bind(bname);
        b = b->_next;
      }

      return false;
    }

    static void saveAll(Print *p)
    {
      if (p == nullptr) return;
      
      IBindable *b = _first;
      while (b != nullptr)
      {
        if (b->bound()) p->printf("%s.bind(\"%s.%s\");\r\n", b->name(), b->_effect->name(), b->_param->name());
        b = b->_next;
      }
    }

    bool bound() { return _effect != nullptr && _param != nullptr; }

    float value() { return bound() ? _param->value() : 0.0f; }

    void print(Print *p)
    {
      if (bound()) p->printf("%s=\"%s.%s\";\r\n", _name, _effect->name(), _param->name());
    }

  protected:
    uint8_t _pin;
    Effect *_effect = nullptr;
    Parameter *_param = nullptr;

    const char *name() { return _name; }
    virtual void onBind() {}

  private:
    static constexpr int _maxlen = 32;
    char _name[_maxlen + 1];
    static IBindable *_first;
    IBindable *_next;
};

IBindable *IBindable::_first = nullptr;


class Button: public IBindable
{
  public:
    Button(int i, uint8_t pin): IBindable(i, "button"), _bounce(pin, 100)
    {
      _pin = pin;
    }

    void begin()
    {
      pinMode(_pin, INPUT_PULLUP);
    }

    void update()
    {
      _bounce.update();

      if (_bounce.fallingEdge())
      {
        if (binding())
        {
          _effect = Effect::current();
          _param = _effect->param();
          setState(r_set_value);
        }
        else if (_effect != nullptr && _param != nullptr)
        {
          _param->toggle();
          _effect->update();
          displayParamValue(_effect, _param);
          setState(r_changed_value);
        }
      }
      if (_bounce.read() == LOW)
      {
        if (_bounce.duration() > 1000 && !_suppressEvent)
        {
          _suppressEvent = true;
          loadPreset(name());
        }
      }
      else _suppressEvent = false;
    }

  private:
    Bounce _bounce;
    bool _suppressEvent;
    const char *_preset;

    //void onBind() {}
};

Button buttons[] = { Button(0, PIN_BTN_0), Button(1, PIN_BTN_1), Button(2, PIN_BTN_2), Button(3, PIN_BTN_3), };

class Potentiometer: public IBindable
{
  public:
    Potentiometer(int i, uint8_t pin)
      : IBindable(i, "pot")
      //, _f(50.0f, 2.0f, 0.1f)
    {
      _pin = pin;
    }

    void begin()
    {
      //pinMode(_pin, INPUT_DISABLE);

      _position = rawRead();
      _high = _position + _trig;
      _low = _position - _trig;
      
      //_f.begin(50.0f, 2.0f, 0.1f);
    }

    void update()
    {
      if (_lastUpdate < 20) return;
      _lastUpdate = 0;
      
      float bounds = _trig;//binding() ? _trig * 5.0f : _trig;
      _high = _position + bounds;
      _low = _position - bounds;

      float raw = rawRead();
      _position += (raw - _position) * _alpha;
      

      if (_position <= _low || _position >= _high)
      {
         _triggered = true;
         _lastTrig = 0;
      }

      if (_triggered && _lastTrig > 100)
      {
        _triggered = false;
      }

      if (_triggered)
      {
        if (binding())
        {
          _effect = Effect::current();
          _param = _effect->param();
          setState(r_set_value);
        }
        if (_effect != nullptr)
        {
          if (_param->scaledChange(_position))
          {
            _effect->update();
            displayParamValue(_effect, _param);
            setState(r_changed_value);
          }
        }
      }
    }
    
  private:
    float _position;
    float _high;
    float _low;
    float _min = 150.0f;
    float _max = 850.0f;
    const float _trig = 0.0075f;
    const float _alpha = 0.5f;
    bool _triggered = false;
    elapsedMillis _lastUpdate = 0;
    elapsedMillis _lastTrig = 0;
    //OneEuroFilter _f;

    float rawRead()
    {
      analogRead(_pin);
      float raw = (float)analogRead(_pin);
      if (raw > _max) _max = raw;
      if (raw < _min) _min = raw;

      return map(raw, _min + 10.0f, _max - 10.0f, 0.0f, 1.0f);
    }
    /*
    void onBind()
    {
      _param->scaledChange(_position);
      _effect->update();
    }
    */
};

Potentiometer pots[] = 
{
  Potentiometer(0, PIN_POT_0), Potentiometer(1, PIN_POT_1), Potentiometer(2, PIN_POT_2), // Potentiometer(PIN_POT_3), Potentiometer(PIN_POT_4),
};


Bounce knobBtn = Bounce(PIN_ENC_C, 50);
Encoder knob(PIN_ENC_A, PIN_ENC_B);
long knobPos = 0;
bool suppressKnobEvent = false;


elapsedMillis readPot;
unsigned int currentPot;

const unsigned long _debounce = 100;
unsigned long _lastCw;
unsigned long _lastCcw;
unsigned long _lastClick;

void cw()
{
  unsigned long now = millis();
  if ((now - _lastCcw) < _debounce) return;
  unsigned long delta = now - _lastCw;
  if (delta < 10) return;
  _lastCw = now;
  float speed = 100.0f / (float)delta;
  if (speed < 1.0f) speed = 1.0f;

  switch (_r_state)
  {
    case r_select_effect:
      Effect::next();
      displayEffectName();
      break;
    case r_select_param:
    case r_select_param_back:
      Effect::current()->nextParam();
      setState(r_select_param);
      break;
    case r_set_value:
      Effect::current()->param()->stepChange(speed);
      Effect::current()->update();
      displayParamValue();
      break;
    case r_binding:
    case r_changed_value:
      revertState();
      break;      
  }
}

void ccw()
{
  unsigned long now = millis();
  if ((now - _lastCw) < _debounce) return;
  unsigned long delta = now - _lastCcw;
  if (delta < 10) return;
  _lastCcw = now;
  float speed = 100.0f / (float)delta;
  if (speed < 1.0f) speed = 1.0f;
  
  switch (_r_state)
  {
    case r_select_effect:
      Effect::prev();
      displayEffectName();
      break;
    case r_select_param:
    case r_select_param_back:
      Effect::current()->prevParam();
      setState(r_select_param);
      break;
    case r_set_value:
      Effect::current()->param()->stepChange((int)-speed);
      Effect::current()->update();
      displayParamValue();
      break;
    case r_binding:
    case r_changed_value:
      revertState();
      break;
  }      
}

void shortClick()
{
  unsigned long now = millis();
  if ((now - _lastClick) < _debounce) return;
  _lastClick = now;

  switch (_r_state)
  {
    case r_select_effect:
      setState(r_select_param);
      break;
    case r_select_param:
      setState(r_set_value);
      break;
    case r_select_param_back:
      setState(r_select_effect);
      break;
    case r_set_value:
      setState(r_select_param_back);
      break;
    case r_binding:
    case r_changed_value:
      revertState();
      break;
  }
}

void longClick()
{
  switch (_r_state)
  {
    case r_select_effect:
      break;
    case r_select_param:
    case r_select_param_back:
    case r_set_value:
      setState(r_binding);
      break;
    case r_binding:
    case r_changed_value:
      break;
  }
}


char _presetName[64];
void printPreset(Print *p)
{
  Effect::saveAll(p);
  IBindable::saveAll(p);
  p->printf("presetName(\"%s\");\r\n", _presetName);  
}

void savePreset(const char* fname)
{
  if (SD.exists(fname))
  {
    char backup[64];
    snprintf(backup, sizeof(backup), "%s.bak", fname);
    SD.remove(backup);
    SD.rename(fname, backup);
  }

  File f = SD.open(fname, FILE_WRITE);
  if (f)
  {
    printPreset(&f);
    f.close();
    Serial.printf("Preset %s saved\r\n", fname);
  }
}

void presetName(const char *name)
{
  if (name == nullptr) return;
  
  int namelen = snprintf(_presetName, sizeof(_presetName), "%s", name);
  if (namelen <= 0) return;
  
  print(_presetName);
}

void UI_Setup()
{
  Display_Setup();

  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  pinMode(PIN_ENC_C, INPUT_PULLUP);

  for (int i = 0; i < (int)(sizeof(buttons) / sizeof(buttons[0])); i++)
  {
    buttons[i].begin();
  }

  for (int i = 0; i < (int)(sizeof(pots) / sizeof(pots[0])); i++)
  {
    pots[i].begin();
  }

  knob.write(9999);
  knobPos = (knob.read() + 2) / 4;

  Effect::begin();
  displayEffectName();

  IBindable::bind("button0", "compression.enable");
  IBindable::bind("button1", "distortion.enable");
  IBindable::bind("button2", "reverb.enable");
  IBindable::bind("button3", "delay.enable");

  IBindable::bind("pot0", "distortion.bass");
  IBindable::bind("pot1", "distortion.middle");
  IBindable::bind("pot2", "distortion.treble");

  loadPreset("default");
}

unsigned long last = 0;
void UI_Loop()
{
  long newPos = (knob.read() + 2) / 4;
  if (newPos != knobPos)
  {
    if (newPos > knobPos) cw();
    else ccw();
    knobPos = newPos;
  }

  checkStateExpiration();

  knobBtn.update();
  if (knobBtn.read() == LOW && knobBtn.duration() > 2000 && !suppressKnobEvent)
  {
    longClick();
    suppressKnobEvent = true;
  }
  if (knobBtn.risingEdge())
  {
    if (!suppressKnobEvent) shortClick();
    suppressKnobEvent = false;
  }

  for (int i = 0; i < (int)(sizeof(buttons) / sizeof(buttons[0])); i++)
  {
    buttons[i].update();
  }

  for (int i = 0; i < (int)(sizeof(pots) / sizeof(pots[0])); i++)
  {
    pots[i].update();
  }
}
