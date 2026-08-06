#include "AudioStream_F32.h"
#include <Bounce.h>
#include <Encoder.h>
#include "Effect.h"


EPARAMS(preamp)
{
  Parameter("level", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(preamp)
{
  int i = 0;
  Parameter& level = e[i++];
  Parameter& enable = e[i++];

  if (level.changed()) hdr.level(level);
  if (enable.changed()) preamp.enable(enable);
});


EPARAMS(envelope)
{
  Parameter("open", Parameter::PT_Float, 0.5f),
  Parameter("close", Parameter::PT_Float, 0.5f),
  Parameter("delay", Parameter::PT_Float, 0.5f),
  Parameter("attack", Parameter::PT_Float, 0.3f),
  Parameter("release", Parameter::PT_Float, 0.0f),
  Parameter("attenuation", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(envelope)
{
  int i = 0;
  Parameter& open = e[i++];
  Parameter& close = e[i++];
  Parameter& delay = e[i++];
  Parameter& attack = e[i++];
  Parameter& release = e[i++];
  Parameter& attenuation = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  if (enableChanged || open.changed()) envelope.open(open);
  if (enableChanged || close.changed()) envelope.close(close);
  if (enableChanged || delay.changed()) envelope.delay(delay);
  if (enableChanged || attack.changed()) envelope.attack(attack);
  if (enableChanged || release.changed()) envelope.release(release);
  if (enableChanged || attenuation.changed()) envelope.attenuation(attenuation);
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
  Parameter("release", Parameter::PT_Float, 0.5f),
  Parameter("sensitivity", Parameter::PT_Float, 0.5f),
  Parameter("output", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false),  
};
EFFECT(ota)
{
  int i = 0;
  Parameter& attack = e[i++];
  Parameter& release = e[i++];
  Parameter& sensitivity = e[i++];
  Parameter& output = e[i++];
  Parameter& enable = e[i++];
  
  bool enableChanged = enable.changed();

  if (enableChanged || attack.changed()) ota.attack(attack);
  if (enableChanged || release.changed()) ota.release(release);
  if (enableChanged || sensitivity.changed()) ota.sensitivity(sensitivity);
  if (enableChanged || output.changed()) ota.output(output);
  if (enableChanged) ota.enable(enable);
});


EPARAMS(distortion)
{
  Parameter("gain", Parameter::PT_Float, 0.5f),
  Parameter("color", Parameter::PT_Float, 0.5f),
  Parameter("skew", Parameter::PT_Float, 0.0f),
  Parameter("bottom", Parameter::PT_Float, 0.5f),
  Parameter("mid", Parameter::PT_Float, 0.5f),
  Parameter("tone", Parameter::PT_Float, 0.75f),
  Parameter("level", Parameter::PT_Float, 0.5f),
  Parameter("enable", Parameter::PT_Bool, false)
};
EFFECT(distortion)
{
  int i = 0;
  Parameter& gain = e[i++];
  Parameter& color = e[i++];
  Parameter& skew = e[i++];
  Parameter& bottom = e[i++];
  Parameter& mid = e[i++];  
  Parameter& tone = e[i++];
  Parameter& level = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || gain.changed()) distortion.gain(gain);
  if (enableChanged || color.changed() || skew.changed()) distortion.color(color, skew);
  if (enableChanged || bottom.changed() || mid.changed() || tone.changed()) distortion.tone(bottom, mid, tone);
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
  Parameter("level", Parameter::PT_Float, 0.0f, -9.0f, 9.0f),
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
  Parameter("rate", Parameter::PT_Float, 0.1f),
  Parameter("stages", Parameter::PT_Coeff, 4.0f, 2.0f, 20.0f, 2.0f),
  Parameter("resonance", Parameter::PT_Coeff, 0.3f),
  Parameter("f1", Parameter::PT_Coeff, 100.0f, 50.0f, 500.0f, 1.0f),
  Parameter("f2", Parameter::PT_Coeff, 900.0f, 500.0f, 5000.0f, 10.0f),
  Parameter("dry", Parameter::PT_Float, 0.6f),
  Parameter("wet", Parameter::PT_Float, 0.6f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(phaser)
{
  int i = 0;
  Parameter& rate = e[i++];  
  Parameter& stages = e[i++];
  Parameter& resonance = e[i++];
  Parameter& f1 = e[i++];
  Parameter& f2 = e[i++];
  Parameter& dry = e[i++];
  Parameter& wet = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
    
  if (enableChanged || rate.changed()) phaser.rate(rate);
  if (enableChanged || stages.changed()) phaser.stages(stages);
  if (enableChanged || resonance.changed()) phaser.resonance(resonance);
  if (enableChanged || f1.changed() || f2.changed()) phaser.frequency(f1, f2);
  if (enableChanged || dry.changed()) phaser.dry(dry);
  if (enableChanged || wet.changed()) phaser.wet(wet);
  if (enableChanged) phaser.enable(enable);
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
  Parameter("preset", Parameter::PT_Enum, 0.0f, 0.0f, 7.0f, 1.0f, (const char*[]){" mch", " tsc", " ce2", " bf2", " dd1", " dd2", " dd3", " dd4"}),
  Parameter("rate", Parameter::PT_Float, 0.5f),
  Parameter("depth", Parameter::PT_Float, 0.5f),
  Parameter("resonance", Parameter::PT_Float, 0.5f),
  Parameter("color", Parameter::PT_Float, 0.5f),
  Parameter("dry", Parameter::PT_Float, 0.8f),
  Parameter("wet", Parameter::PT_Float, 0.8f),
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

  if (enableChanged || preset.changed()) chorus.preset(preset);
  if (enableChanged || rate.changed()) chorus.rate(rate);
  if (enableChanged || depth.changed()) chorus.depth(depth);
  if (enableChanged || resonance.changed()) chorus.resonance(resonance);
  if (enableChanged || color.changed()) chorus.color(color);
  if (enableChanged || dry.changed()) chorus.dry(dry);
  if (enableChanged || wet.changed()) chorus.wet(wet);
  if (enableChanged) chorus.enable(enable);
});


EPARAMS(reverb)
{
  Parameter("delay", Parameter::PT_Float, 0.5f),
  Parameter("hp", Parameter::PT_Freq, 100.0f, 50.0f, 4000.0f),
  Parameter("lp", Parameter::PT_Freq, 5600.0f, 100.0f, 14000.0f),
  Parameter("density", Parameter::PT_Float, 0.7f),
  Parameter("decay", Parameter::PT_Float, 0.5f),
  Parameter("damp", Parameter::PT_Float, 0.2f),
  Parameter("dry", Parameter::PT_Float, 1.0f),
  Parameter("wet", Parameter::PT_Float, 0.3f),
  Parameter("enable", Parameter::PT_Bool, false)
};
EFFECT(reverb)
{
  int i = 0;
  Parameter& delay = e[i++];
  Parameter& hp = e[i++];
  Parameter& lp = e[i++];
  Parameter& density = e[i++];
  Parameter& decay = e[i++];
  Parameter& damp = e[i++];
  Parameter& dry = e[i++];
  Parameter& wet = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();

  if (enableChanged || delay.changed()) reverb.delay(delay);
  if (enableChanged || hp.changed()) reverb.highpass(hp);
  if (enableChanged || lp.changed()) reverb.lowpass(lp);
  if (enableChanged || density.changed()) reverb.density(density);
  if (enableChanged || decay.changed()) reverb.decay(decay);
  if (enableChanged || damp.changed()) reverb.damping(damp);
  if (enableChanged || dry.changed()) reverb.dry(dry);
  if (enableChanged || wet.changed()) reverb.wet(wet);
  if (enableChanged) reverb.enable(enable);
});


EPARAMS(delay)
{
  Parameter("time", Parameter::PT_Float, 500.0f, 10.0f, 1000.0f, 10.0f),
  Parameter("repeat", Parameter::PT_Float, 0.20f),
  Parameter("hp", Parameter::PT_Freq, 50.0f, 50.0f, 4000.0f),
  Parameter("lp", Parameter::PT_Freq, 14000.0f, 2000.0f, 14000.0f),
  Parameter("lp_q", Parameter::PT_Float, 0.7f, 0.5f, 3.0f),
  Parameter("rate", Parameter::PT_Float, 0.5f),
  Parameter("depth", Parameter::PT_Float, 0.5f),
  Parameter("spread", Parameter::PT_Float, 0.5f),
  Parameter("dry", Parameter::PT_Float, 1.0f), 
  Parameter("wet", Parameter::PT_Float, 0.3f),
  Parameter("enable", Parameter::PT_Bool, false),
};
EFFECT(delay)
{
  int i = 0;
  Parameter& time = e[i++];
  Parameter& repeat = e[i++];
  Parameter& hp = e[i++];
  Parameter& lp = e[i++];
  Parameter& lp_q = e[i++];
  Parameter& rate = e[i++];
  Parameter& depth = e[i++];
  Parameter& spread = e[i++];
  Parameter& dry = e[i++];
  Parameter& wet = e[i++];
  Parameter& enable = e[i++];

  bool enableChanged = enable.changed();
  if (enableChanged || time.changed()) stereodelay.time(time);
  if (enableChanged || repeat.changed()) stereodelay.repeat(repeat);
  if (enableChanged || hp.changed()) stereodelay.highpass(hp);
  if (enableChanged || lp.changed() || lp_q.changed()) stereodelay.lowpass(lp, lp_q);
  if (enableChanged || rate.changed()) stereodelay.rate(rate);
  if (enableChanged || depth.changed()) stereodelay.depth(depth);
  if (enableChanged || spread.changed()) stereodelay.spread(spread);
  if (enableChanged || dry.changed()) stereodelay.dry(dry);
  if (enableChanged || wet.changed()) stereodelay.wet(wet);
  if (enableChanged) stereodelay.enable(enable);
});


EPARAMS(cab_sim)
{
  Parameter("direct", Parameter::PT_Float, 1.0f),
  Parameter("size", Parameter::PT_Float, 0.5f),
  Parameter("room", Parameter::PT_Float, 0.5f),
  Parameter("notch_q", Parameter::PT_Coeff, 0.5f, 0.5f, 5.0f, 0.1f),
  Parameter("notch_low", Parameter::PT_Coeff, 500.0f, 500.0f, 2500.0f, 50.0f),
  Parameter("notch_high", Parameter::PT_Coeff, 800.0f, 800.0f, 7000.0f, 100.0f),
  Parameter("listen", Parameter::PT_Enum, 0.0f, 0.0f, 4.0f, 1.0f, (const char*[]){"out", "low", "mid", "hi", "byps"}),
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
  Parameter("enable", Parameter::PT_Bool, true),
};
EFFECT(cab_sim)
{
  int i = 0;
  Parameter& direct = e[i++];
  Parameter& size = e[i++];
  Parameter& room = e[i++];
  Parameter& notch_q = e[i++];
  Parameter& notch_low = e[i++];
  Parameter& notch_high = e[i++];
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

  bool enableChanged = enable.changed();

  if (enableChanged || direct.changed()) cabsim.direct(direct);
  if (enableChanged || size.changed()) cabsim.size(size);
  if (enableChanged || room.changed()) cabsim.room(room);
  if (enableChanged || notch_q.changed() || notch_low.changed() || notch_high.changed()) cabsim.notch(notch_q, notch_low, notch_high);
  if (enableChanged) cabsim.enable(enable);
  sonic.begin(enable ? (int)listen : 4, lowMid, midHigh, lComp, mComp, hComp, lOut, mOut, hOut, attack, release, width);
});


EPARAMS(volume)
{
  Parameter("volume", Parameter::PT_Float, 1.0f)
};
EFFECT(volume)
{
  int i = 0;
  Parameter& volume = e[i++];

  if (volume.changed()) setVolume(volume);
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

FLASHMEM void setState(short newState)
{
  _changeTimeout = 0;
  _lastStateChange = 0;

  switch (newState)
  {
    case r_select_preset:
    case r_select_preset_back:
      displayPresetName();
      break;
    case r_tuner:
    case r_tuner_back:
      print("Tuner");
      break;
    case r_select_effect:
    case r_select_effect_back:
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

FLASHMEM void revertState()
{
  setState(_r_prev);
}

FLASHMEM bool updateParam(const char* name, const char* param, float value)
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

FLASHMEM bool binding()
{
  return _r_state == r_binding;
}

FLASHMEM void checkStateExpiration()
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

    bool unbind()
    {
      _param = nullptr;
      _effect = nullptr;
      
      if (debug) Serial.printf("Control %s unbound\r\n", _name);

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

      if (debug) Serial.printf("Control %s not found (%s)\r\n", name, bname);

      return false;
    }

    static bool unbind(const char *name)
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

    static void saveAll(Print *p)
    {
      if (p == nullptr) return;
      
      IBindable *b = _first;
      while (b != nullptr)
      {
        b->print(p);
        b = b->_next;
      }
    }

    bool bound() { return _effect != nullptr && _param != nullptr; }

    float value() { return bound() ? _param->value() : 0.0f; }

    void print(Print *p)
    {
      if (p == nullptr) return;

      if (bound()) p->printf("bind(\"%s\",\"%s.%s\");\r\n", _name, _effect->name(), _param->name());
      else p->printf("unbind(\"%s\");\r\n", _name);
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

    Button(Button const&) = delete;
    Button& operator=(Button const&) = delete;

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

    Potentiometer(Potentiometer const&) = delete;
    Potentiometer& operator=(Potentiometer const&) = delete;

    void begin()
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
    const float _alpha = 0.25f;
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

#if defined(PIN_ENC_A) && defined(PIN_ENC_B)
Encoder knob(PIN_ENC_A, PIN_ENC_B);
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
int _lastCwPos = 0;
int _lastCcwPos = 0;

FLASHMEM void cw(int pos)
{
  unsigned long now = millis();
  if ((now - _lastCcw) < _debounce) return;
  unsigned long delta = now - _lastCw;
  if (delta < 10) return;
  //if (pos == _lastCwPos) return;
  _lastCw = now;
  _lastCwPos = pos;
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

FLASHMEM void ccw(int pos)
{
  unsigned long now = millis();
  if ((now - _lastCw) < _debounce) return;
  unsigned long delta = now - _lastCcw;
  if (delta < 10) return;
  //if (pos == _lastCcwPos) return;
  _lastCcw = now;
  _lastCcwPos = pos;
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

FLASHMEM void shortClick()
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

FLASHMEM void longClick()
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
  if (!SD.mediaPresent())
  {
    if (debug) Serial.print("No SD card\r\n");
    return;
  }

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


FLASHMEM void displayTuner(float freq, int note, int semitone, float cents)
{
  if (_r_state != r_tuner && _r_state != r_tuner_back) return;
  _changeTimeout = 2500;
  _lastStateChange = 0;
  displayTunerHW(freq, note, semitone, cents);
}

#if defined(PIN_ENC_A) && defined(PIN_ENC_B)
int readKnob()
{
  long pos = knob.read() + 2;
  bool neg = pos < 0;
  pos = abs(pos) / 4;
  return neg ? -pos : pos;
}
#endif

FLASHMEM void UI_Setup()
{
  #if defined(PIN_ENC_A) && defined(PIN_ENC_B)
  pinMode(PIN_ENC_A, INPUT_PULLUP);
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

#if defined(PIN_ENC_A) && defined(PIN_ENC_B)
  knob.write(99999);
  knobPos = readKnob();
#endif

  Effect::begin();
  displayEffectName();

  loadPreset("default");
}

unsigned long last = 0;
void UI_Loop()
{
#if defined(PIN_ENC_A) && defined(PIN_ENC_B)
  long newPos = readKnob();

  if (newPos != knobPos)
  {
    if (newPos > knobPos) cw(newPos);
    else ccw(newPos);
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
