#include "AudioStream_F32.h"
#include <Bounce.h>
#include <Encoder.h>
#include "Effect.h"


EPARAMS(preamp)
{
  Parameter("level", Parameter::PT_Float, 0.5f),
  Parameter("Output", Parameter::PT_Enum, 0.0f, 0.0f, 3.0f, 1.0f, (const char*[]){" hdr", " low", "high", "mute"}),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(preamp)
{
  int i = 0;
  Parameter& level = e[i++];
  Parameter& output = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || level.changed()) preamp.level(level);
  if (enableChanged || output.changed()) hdr.setOutput(output);
  if (enableChanged) preamp.enable(enable);
});


EPARAMS(envelope)
{
  Parameter("open", Parameter::PT_Float, 0.5f),
  Parameter("close", Parameter::PT_Float, 0.5f),
  Parameter("attack", Parameter::PT_Float, 0.3f),
  Parameter("release", Parameter::PT_Float, 0.0f),
  //Parameter("sensitivity", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(envelope)
{
  int i = 0;
  Parameter& open = e[i++];
  Parameter& close = e[i++];
  Parameter& attack = e[i++];
  Parameter& release = e[i++];
  //Parameter& sensitivity = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  if (enableChanged || open.changed()) envelope.open(open);
  if (enableChanged || close.changed()) envelope.close(close);
  if (enableChanged || attack.changed()) envelope.attack(attack);
  if (enableChanged || release.changed()) envelope.release(release);
  //if (enableChanged || sensitivity.changed()) envelope.sensitivity(sensitivity);
  if (enableChanged) envelope.enable(enable);
});


EPARAMS(auto_wah)
{
  Parameter("sensitivity", Parameter::PT_Float, 0.5f),
  Parameter("speed", Parameter::PT_Float, 0.5f),
  Parameter("dry", Parameter::PT_Float, 0.0f),
  Parameter("wet", Parameter::PT_Float, 1.0f),
  Parameter("enable", Parameter::PT_Bool, false),  
};
EFFECT(auto_wah)
{
  int i = 0;
  Parameter& sensitivity = e[i++];
  Parameter& speed = e[i++];
  Parameter& dry = e[i++];
  Parameter& wet = e[i++];
  Parameter& enable = e[i++];
  
  bool enableChanged = enable.changed();

  if (enableChanged || sensitivity.changed()) wah.sensitivity(sensitivity);
  if (enableChanged || speed.changed()) wah.speed(speed);
  if (enableChanged || dry.changed()) wah.dry(dry);
  if (enableChanged || wet.changed()) wah.wet(wet);
  if (enableChanged) wah.enable(enable);
});


EPARAMS(ota)
{
  Parameter("attack", Parameter::PT_Float, 0.0f),
  Parameter("sensitivity", Parameter::PT_Float, 0.5f),
  Parameter("output", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),  
};
EFFECT(ota)
{
  int i = 0;
  Parameter& attack = e[i++];
  Parameter& sensitivity = e[i++];
  Parameter& output = e[i++];
  Parameter& enable = e[i++];
  
  bool enableChanged = enable.changed();

  if (enableChanged || attack.changed()) ota.attack(attack);
  if (enableChanged || sensitivity.changed()) ota.sensitivity(sensitivity);
  if (enableChanged || output.changed()) ota.output(output);
  if (enableChanged) ota.enable(enable);
});


EPARAMS(distortion)
{
  Parameter("gain", Parameter::PT_Float, 0.5f),
  Parameter("bottom", Parameter::PT_Float, 0.5f),
  Parameter("color", Parameter::PT_Float, 0.5f),
  Parameter("skew", Parameter::PT_Float, 0.0f),
  Parameter("mid", Parameter::PT_Float, 0.5f),
  Parameter("tone", Parameter::PT_Float, 0.75f),
  Parameter("level", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(distortion)
{
  int i = 0;
  Parameter& gain = e[i++];
  Parameter& bottom = e[i++];
  Parameter& color = e[i++];
  Parameter& skew = e[i++];
  Parameter& mid = e[i++];  
  Parameter& tone = e[i++];
  Parameter& level = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || gain.changed()) distortion.gain(gain);
  if (enableChanged || bottom.changed()) distortion.bottom(bottom);
  if (enableChanged || color.changed() || skew.changed()) distortion.color(color, skew);
  if (enableChanged || tone.changed() || mid.changed()) distortion.tone(tone, mid);
  if (enableChanged || level.changed()) distortion.level(level);
  if (enableChanged) distortion.enable(enable);
});


EPARAMS(tonestack)
{
  Parameter("bass", Parameter::PT_Float, 0.5f),
  Parameter("middle", Parameter::PT_Float, 0.5f),
  Parameter("treble", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(tonestack)
{
  int i = 0;
  Parameter& bass = e[i++];
  Parameter& middle = e[i++];
  Parameter& treble = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || bass.changed() || middle.changed() || treble.changed())
  {
    toneStack.setTone(bass, middle, treble);
  }

  if (enableChanged) toneStack.enable(enable);
});


EPARAMS(equalizer)
{
  Parameter("100", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("200", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("400", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("800", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("1_6k", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("3_2k", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("6_4k", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("level", Parameter::PT_Float, 0.0f, -15.0f, 15.0f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(equalizer)
{
  int i = 0;
  Parameter& g1 = e[i++];
  Parameter& g2 = e[i++];
  Parameter& g3 = e[i++];
  Parameter& g4 = e[i++];
  Parameter& g5 = e[i++];
  Parameter& g6 = e[i++];
  Parameter& g7 = e[i++];
  Parameter& level = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || g1.changed()) equalizer.eq(1, g1);
  if (enableChanged || g2.changed()) equalizer.eq(2, g2);
  if (enableChanged || g3.changed()) equalizer.eq(3, g3);
  if (enableChanged || g4.changed()) equalizer.eq(4, g4);
  if (enableChanged || g5.changed()) equalizer.eq(5, g5);
  if (enableChanged || g6.changed()) equalizer.eq(6, g6);
  if (enableChanged || g7.changed()) equalizer.eq(7, g7);
  if (enableChanged || level.changed()) equalizer.level(level);
  if (enableChanged) equalizer.enable(enable);
});


EPARAMS(phaser)
{
  Parameter("preset", Parameter::PT_Enum, 0.0f, 0.0f, 4.0f, 1.0f, (const char*[]){" p_90", " p_45", "p_100", " evh", "sm_st"}),
  Parameter("rate", Parameter::PT_Float, 0.1f),
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


EPARAMS(tremolo)
{
  Parameter("rate", Parameter::PT_Float, 5.0f, 0.1f, 10.0f, 0.1f),
  Parameter("depth", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(tremolo)
{
  int i = 0;
  Parameter& rate = e[i++];
  Parameter& depth = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enable)
  {
    if (enableChanged || rate.changed()) tremolo.rate(rate);
    if (enableChanged || depth.changed()) tremolo.depth(depth);
  }
  else tremolo.depth(0.0f);
});


EPARAMS(chorus)
{
  Parameter("preset", Parameter::PT_Enum, 0.0f, 0.0f, 7.0f, 1.0f, (const char*[]){" def", " tsc", " ce2", " bf2", " dd1", " dd2", " dd3", " dd4"}),
  Parameter("rate", Parameter::PT_Float, 0.5f),
  Parameter("depth", Parameter::PT_Float, 0.5f),
  Parameter("resonance", Parameter::PT_Float, 0.5f),
  Parameter("color", Parameter::PT_Float, 0.5f),
  Parameter("dry", Parameter::PT_Float, 1.0f),
  Parameter("wet", Parameter::PT_Float, 0.7f),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(chorus)
{
  int i = 0;
  Parameter& preset = e[i++];
  Parameter& rate = e[i++];
  Parameter& depth = e[i++];
  Parameter& resonance = e[i++];
  Parameter& color = e[i++];
  Parameter& dry = e[i++];
  Parameter& wet = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || preset.changed())
  {
    if (!enable) chorus.type(AudioEffectMultiChorus_F32::Bypass);
    else switch ((int)preset)
    {
      case 0: chorus.type(AudioEffectMultiChorus_F32::Default); break;
      case 1: chorus.type(AudioEffectMultiChorus_F32::TSC); break;
      case 2: chorus.type(AudioEffectMultiChorus_F32::CE2); break;
      case 3: chorus.type(AudioEffectMultiChorus_F32::BF2); break;     
      case 4: chorus.type(AudioEffectMultiChorus_F32::DD1); break;
      case 5: chorus.type(AudioEffectMultiChorus_F32::DD2); break;
      case 6: chorus.type(AudioEffectMultiChorus_F32::DD3); break;
      case 7: chorus.type(AudioEffectMultiChorus_F32::DD4); break;
    }
  }
  if (enableChanged || rate.changed()) chorus.rate(rate);
  if (enableChanged || depth.changed()) chorus.depth(depth);
  if (enableChanged || resonance.changed()) chorus.resonance(resonance);
  if (enableChanged || color.changed()) chorus.color(color);
  if (enableChanged || dry.changed()) chorus.dry(dry);
  if (enableChanged || wet.changed()) chorus.wet(wet);
});

EPARAMS(reverb)
{
  Parameter("input", Parameter::PT_Float, 1.0f),
  Parameter("delay", Parameter::PT_Float, 0.5f),
  Parameter("bw", Parameter::PT_Float, 0.8f),
  Parameter("density", Parameter::PT_Float, 0.7f),
  Parameter("decay", Parameter::PT_Float, 0.5f),
  Parameter("damp", Parameter::PT_Float, 0.2f),
  Parameter("sensitivity", Parameter::PT_Float, 0.0f),
  Parameter("wet", Parameter::PT_Float, 0.2f),
  Parameter("dry", Parameter::PT_Float, 1.0f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(reverb)
{
  int i = 0;
  Parameter& input = e[i++];
  Parameter& delay = e[i++];
  Parameter& bw = e[i++];
  Parameter& density = e[i++];
  Parameter& decay = e[i++];
  Parameter& damp = e[i++];
  Parameter& sensitivity = e[i++];
  Parameter& wet = e[i++];
  Parameter& dry = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || input.changed()) reverb.input(input);
  if (enableChanged || delay.changed()) reverb.delay(delay);
  if (enableChanged || bw.changed()) reverb.bandwidth(bw);
  if (enableChanged || density.changed()) reverb.density(density);
  if (enableChanged || decay.changed()) reverb.decay(decay);
  if (enableChanged || damp.changed()) reverb.damping(damp);
  if (enableChanged || sensitivity.changed()) reverb.sensitivity(sensitivity);
  if (enableChanged || wet.changed()) reverb.wet(wet);
  if (enableChanged || dry.changed()) reverb.dry(dry);
  if (enableChanged) reverb.enable(enable);
});


EPARAMS(delay)
{
  Parameter("time", Parameter::PT_Float, 500.0f, 10.0f, 1000.0f, 10.0f),
  Parameter("repeat", Parameter::PT_Float, 0.20f),
  Parameter("drive", Parameter::PT_Float, 0.0f),
  Parameter("freq", Parameter::PT_Freq, 2500.0f, 100.0f, 10000.0f),
  Parameter("f_q", Parameter::PT_Float, 1.0f, 0.1f, 10.0f, 0.1f),
  Parameter("f_dr", Parameter::PT_Float, 0.0f),
  Parameter("f_lo", Parameter::PT_Float, 0.6f),
  Parameter("f_ba", Parameter::PT_Float, 0.9f),
  Parameter("f_hi", Parameter::PT_Float, 0.0f),
  Parameter("rate", Parameter::PT_Float, 0.5f),
  Parameter("depth", Parameter::PT_Float, 0.5f),
  Parameter("spread", Parameter::PT_Float, 0.5f),  
  Parameter("wet", Parameter::PT_Float, 0.2f),
  Parameter("dry", Parameter::PT_Float, 1.0f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(delay)
{
  int i = 0;
  Parameter& time = e[i++];
  Parameter& repeat = e[i++];
  Parameter& drive = e[i++];
  Parameter& freq = e[i++];
  Parameter& f_q = e[i++];
  Parameter& f_dr = e[i++];
  Parameter& f_lo = e[i++];
  Parameter& f_ba = e[i++];
  Parameter& f_hi = e[i++];
  Parameter& rate = e[i++];
  Parameter& depth = e[i++];
  Parameter& spread = e[i++];
  Parameter& wet = e[i++];
  Parameter& dry = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  if (enableChanged || time.changed()) stereodelay.time(time);
  if (enableChanged || repeat.changed()) stereodelay.repeat(repeat);
  if (enableChanged || drive.changed()) stereodelay.drive(drive);
  if (enableChanged || freq.changed() || f_q.changed() || f_dr.changed() || f_lo.changed() || f_ba.changed() || f_hi.changed()) stereodelay.filter(freq, f_q, f_dr, f_lo, f_ba, f_hi);
  if (enableChanged || rate.changed()) stereodelay.rate(rate);
  if (enableChanged || depth.changed()) stereodelay.depth(depth);
  if (enableChanged || spread.changed()) stereodelay.spread(spread);
  if (enableChanged || wet.changed()) stereodelay.wet(wet);
  if (enableChanged || dry.changed()) stereodelay.dry(dry);    
  if (enableChanged) stereodelay.enable(enable);
});


EPARAMS(sonic)
{
  Parameter("listen", Parameter::PT_Enum, 3.0f, 0.0f, 3.0f, 1.0f, (const char*[]){"low", "mid", "hi", "out"}),
  Parameter("low_mid", Parameter::PT_Float, 0.6f),
  Parameter("mid_high", Parameter::PT_Float, 0.4f),
  Parameter("l_comp", Parameter::PT_Float, 0.5f),
  Parameter("m_comp", Parameter::PT_Float, 0.5f),
  Parameter("h_comp", Parameter::PT_Float, 0.5f),
  Parameter("l_out", Parameter::PT_Float, 0.4f),
  Parameter("m_out", Parameter::PT_Float, 0.4f),
  Parameter("h_out", Parameter::PT_Float, 0.4f),
  Parameter("attack", Parameter::PT_Float, 0.5f),
  Parameter("release", Parameter::PT_Float, 0.5f),
  Parameter("width", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(sonic)
{
  int i = 0;
  Parameter& listen = e[i++];
  Parameter& lowMid = e[i++];
  Parameter& midHigh = e[i++];
  Parameter& lComp = e[i++];
  Parameter& mComp = e[i++];
  Parameter& hComp = e[i++];
  Parameter& lOut = e[i++];
  Parameter& mOut = e[i++];
  Parameter& hOut = e[i++];
  Parameter& attack = e[i++];
  Parameter& release = e[i++];
  Parameter& width = e[i++];
  Parameter& enable = e[i++];

  sonic.begin(enable ? (int)listen : 4, lowMid, midHigh, lComp, mComp, hComp, lOut, mOut, hOut, attack, release, width, false);
});


EPARAMS(cab_sim)
{
  Parameter("direct", Parameter::PT_Float, 1.0f),
  Parameter("size", Parameter::PT_Float, 0.5f),
  Parameter("room", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(cab_sim)
{
  int i = 0;
  Parameter& direct = e[i++];
  Parameter& size = e[i++];
  Parameter& room = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || direct.changed()) cabsim.direct(direct);
  if (enableChanged || size.changed()) cabsim.size(size);
  if (enableChanged || room.changed()) cabsim.room(room);
  if (enableChanged) cabsim.enable(enable);
});


/**********************************************************************************************************************************/



typedef enum
{
  r_select_preset,
  r_select_preset_back,
  r_tuner,
  r_tuner_back,
  r_select_effect,
  r_select_effect_back,
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

FLASHMEM void displayPresetName()
{
  print("Preset");
}

FLASHMEM void displayEffectName(Effect* effect = nullptr)
{
  if (effect == nullptr) effect = Effect::current();
  print(effect->name());
}

FLASHMEM void displayParamName(Effect* effect = nullptr, Parameter* param = nullptr)
{
  char buf[32];
  if (effect == nullptr) effect = Effect::current();
  if (param == nullptr) param = effect->param();
  snprintf(buf, 32, "%-2.2s.%-6.6s", effect->name(), param->name());
  print(buf);
}

FLASHMEM void displayParamValue(Effect* effect = nullptr, Parameter* param = nullptr)
{
  char buf[32];
  if (effect == nullptr) effect = Effect::current();
  if (param == nullptr) param = effect->param();
  snprintf(buf, 32, "%-1.1s.%-3.3s%s", effect->name(), param->name(), param->tos());
  print(buf);
}

void setState(short newState)
{
  _changeTimeout = 0;
  _lastStateChange = 0;

  switch (newState)
  {
    case r_select_preset:
    case r_select_preset_back:
      clear();
      displayPresetName();
      break;
    case r_tuner:
    case r_tuner_back:
      clear();
      print("Tuner");
      break;
    case r_select_effect:
    case r_select_effect_back:
      clear();
      displayEffectName();
      break;
    case r_select_param:
    case r_select_param_back:
      clear();
      displayParamName();
      break;
    case r_set_value:
      clear();
      displayParamValue();
      break;
    case r_binding:
      clear();
      print("binding.");
      _changeTimeout = 5000;
      break;
    case r_changed_value:
      _changeTimeout = 2500;
      break;
    default: return;
  }
  
  if (_r_state != (r_state_t)newState) 
  {
    _r_prev = _r_state;
  }
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
  setState(r_changed_value);
  displayParamValue(e, p);

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

    FLASHMEM bool bind(const char* bname)
    {
      if (bname == nullptr) return false;
      
      char ename[64];
      char pname[64];
      int i = 0;
      int p = 0;
      int len = bname == nullptr ? 0 : strnlen(bname, 64);
      if (len == 0)
      {
        if (debug) Serial.printf("Control %s unbound\r\n", _name);
        _param = nullptr;
        _effect = nullptr;
        return true;
      }
      
      for (; i < len; i++)
      {
        if (bname[i] == '.' || bname[i] == 0) break;
        ename[p++] = bname[i];
      }
      ename[p] = 0;

      Effect *effect = Effect::effect(ename);
      if (effect == nullptr)
      {
        if (debug) Serial.printf("Effect %s not found\r\n", ename);
        return false;
      }
      
      p = 0;
      i++;
      for (; i < len; i++)
      {
        if (bname[i] == 0) break;
        pname[p++] = bname[i];
      }
      pname[p] = 0;

      Parameter *param = effect->param(pname);
      if (param == nullptr)
      {
        if (debug) Serial.printf("Parameter %s.%s not found\r\n", ename, pname);
        return false;
      }
      
      _effect = effect;
      _param = param;
      
      onBind();
      if (debug) Serial.printf("Control %s Bound to %s.%s\r\n", _name, _effect->name(), _param->name());
      
      return true;
    }

    FLASHMEM bool unbind()
    {
      _param = nullptr;
      _effect = nullptr;
      
      if (debug) Serial.printf("Control %s unbound\r\n", _name);

      return true;    
    }

    FLASHMEM static bool bind(const char *name, const char* bname)
    {
      if (name == nullptr || bname == nullptr) return false;
      
      IBindable *b = _first;
      while (b != nullptr)
      {
        if (strncmp(name, b->_name, _maxlen) == 0) return b->bind(bname);
        b = b->_next;
      }

      if (debug) Serial.printf("Control %s not found (%s)\r\n", name, bname);

      return false;
    }

    FLASHMEM static bool unbind(const char *name)
    {
      if (name == nullptr) return false;
      
      IBindable *b = _first;
      while (b != nullptr)
      {
        if (strncmp(name, b->_name, _maxlen) == 0) return b->unbind();
        b = b->_next;
      }

      if (debug) Serial.printf("Control %s not found\r\n", name);

      return false;
    }

    FLASHMEM static void saveAll(Print *p)
    {
      if (p == nullptr) return;
      
      IBindable *b = _first;
      while (b != nullptr)
      {
        b->print(p);
        b = b->_next;
      }
    }

    FLASHMEM bool bound() { return _effect != nullptr && _param != nullptr; }

    FLASHMEM float value() { return bound() ? _param->value() : 0.0f; }

    FLASHMEM void print(Print *p)
    {
      if (p == nullptr) return;

      if (bound()) p->printf("bind(\"%s\",\"%s.%s\");\r\n", _name, _effect->name(), _param->name());
      else p->printf("unbind(\"%s\");\r\n", _name);
    }

  protected:
    uint8_t _pin;
    Effect *_effect = nullptr;
    Parameter *_param = nullptr;

    FLASHMEM const char *name() { return _name; }
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

    FLASHMEM void begin()
    {
      pinMode(_pin, INPUT_PULLUP);
    }

    FLASHMEM void update()
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
          setState(r_changed_value);
          displayParamValue(_effect, _param);
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

Button buttons[] =
{
#ifdef PIN_BTN_0
Button(0, PIN_BTN_0),
  #ifdef PIN_BTN_1
  Button(1, PIN_BTN_1),
    #ifdef PIN_BTN_2
    Button(2, PIN_BTN_2),
      #ifdef PIN_BTN_3
      Button(3, PIN_BTN_3),
        #ifdef PIN_BTN_4
        Button(4, PIN_BTN_4),
        #endif
      #endif
    #endif
  #endif
#endif
};

class Potentiometer: public IBindable
{
  public:
    Potentiometer(int i, uint8_t pin)
      : IBindable(i, "pot")
      //, _f(50.0f, 2.0f, 0.1f)
    {
      _pin = pin;
    }

    FLASHMEM void begin()
    {
      _position = rawRead();
      _high = _position + _trig;
      _low = _position - _trig;
    }

    void update()
    {
      if (_lastUpdate < 50) return;
      _lastUpdate = 0;
      
      float bounds = _trig;
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
    const float _alpha = 0.05f;
    bool _triggered = false;
    elapsedMillis _lastUpdate = 0;
    elapsedMillis _lastTrig = 0;

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
#ifdef PIN_POT_0  
Potentiometer(0, PIN_POT_0),
  #ifdef PIN_POT_1 
  Potentiometer(1, PIN_POT_1),
    #ifdef PIN_POT_2 
    Potentiometer(2, PIN_POT_2),
    #endif
  #endif
#endif
};

#ifdef PIN_ENC_A
#ifdef PIN_ENC_B
Encoder knob(PIN_ENC_A, PIN_ENC_B);
#endif
#endif

#ifdef PIN_ENC_C
Bounce knobBtn = Bounce(PIN_ENC_C, 50);
#endif

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
    case r_select_preset:
    case r_select_preset_back:
      // TODO: Show next preset name
      break;
    case r_select_effect:
    case r_select_effect_back:
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
    default:
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
    case r_select_preset:
    case r_select_preset_back:
      // TODO: Show prev preset name
      break;
    case r_select_effect:
    case r_select_effect_back:
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
    default:
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
    case r_select_preset:
    case r_select_preset_back:
      setState(r_tuner);
      break;
    case r_tuner:
      setState(r_select_effect);
      break;
    case r_tuner_back:
      setState(r_select_preset_back);
      break;
    case r_select_effect:
      setState(r_select_param);
      break;
    case r_select_effect_back:
      setState(r_tuner_back);
      break;
    case r_select_param:
      setState(r_set_value);
      break;
    case r_select_param_back:
      setState(r_select_effect_back);
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
    case r_select_param:
    case r_select_param_back:
    case r_set_value:
      setState(r_binding);
      break;
    default:
      break;
  }
}


char _presetName[64];
FLASHMEM void printPreset(Print *p)
{
  Effect::saveAll(p);
  IBindable::saveAll(p);
  p->printf("presetName(\"%s\");\r\n", _presetName);  
}

FLASHMEM void savePreset(const char* fname)
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
    if (debug) Serial.printf("Preset %s saved\r\n", fname);
  }
}

FLASHMEM void presetName(const char *name)
{
  if (name == nullptr) return;
  
  int namelen = snprintf(_presetName, sizeof(_presetName), "%s", name);
  if (namelen <= 0) return;
  
  print(_presetName);
}


void displayTuner(float freq, int note, int semitone, float cents)
{
  if (_r_state != r_tuner && _r_state != r_tuner_back) return;
  
  displayTunerHW(freq, note, semitone, cents);
}

FLASHMEM void UI_Setup()
{
  #ifdef PIN_ENC_A
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  #endif
  #ifdef PIN_ENC_B
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  #endif
  #ifdef PIN_ENC_C
  pinMode(PIN_ENC_C, INPUT_PULLUP);
  #endif
  
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

  loadPreset("default");
}

unsigned long last = 0;
void UI_Loop()
{
#ifdef PIN_ENC_A
  long newPos = (knob.read() + 2) / 4;
  if (newPos != knobPos)
  {
    if (newPos > knobPos) cw();
    else ccw();
    knobPos = newPos;
  }
#endif

  checkStateExpiration();

#ifdef PIN_ENC_C
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
#endif

  for (int i = 0; i < (int)(sizeof(buttons) / sizeof(buttons[0])); i++)
  {
    buttons[i].update();
  }

  for (int i = 0; i < (int)(sizeof(pots) / sizeof(pots[0])); i++)
  {
    pots[i].update();
  }
}
