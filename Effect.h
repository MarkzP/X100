#ifndef _effect_h
#define _effect_h

#include <Print.h>
const float _fbands[] = { 50.0f,   56.0f,   63.0f,   71.0f,   80.0f,   89.0f,   100.0f,   112.0f,   125.0f,   140.0f,   158.0f,   175.0f,   200.0f,   225.0f,   250.0f,   280.0f,   315.0f,   400.0f,   440.0f,
                          500.0f,  560.0f,  630.0f,  710.0f,  800.0f,  890.0f,  1000.0f,  1120.0f,  1250.0f,  1400.0f,  1580.0f,  1750.0f,  2000.0f,  2250.0f,  2500.0f,  2800.0f,  3150.0f,  4000.0f,  4400.0f,
                          5000.0f, 5600.0f, 6300.0f, 7100.0f, 8000.0f, 8900.0f, 10000.0f, 11200.0f, 12500.0f, 14000.0f, 15800.0f, 17500.0f, 19000.0f
                        };
                        
class Parameter
{
  public:
    enum ParameterType
    {
      PT_Float = 0,
      PT_Bool,
      PT_Freq,
      PT_Enum,
      PT_Coeff,
    };

    Parameter(const char *name, ParameterType type, float value, float min = 0.0f, float max = 1.0f, float step = 0.0f, const char **labels = nullptr)
    {
      _name = name;
      _type = type;
      switch (_type)
      {
        case PT_Float:
        case PT_Coeff:
          _value = value;
          _min = min;
          _max = max;
          _step = step == 0.0f ? fabsf(max - min) * 0.01f : step;
          if (_step < 0.01f) _step = 0.01f;
          break;
        case PT_Bool:
          _value = value < 0.5f ? 0.0f : 1.0f;
          _min = 0.0f;
          _max = 1.0f;
          _value = value;
          break;
        case PT_Enum:
          _labels = labels;
          _value = (float)(int)value;
          _min = 0.0f;
          _max = (float)(int)max;
          _step = 1.0f;
          break;
        case PT_Freq:
        {
          _value = freqToBand(value);
          _min = min == 0 ? 0 : freqToBand(min);
          _max = max == 1 ? _maxband : freqToBand(max);
          _step = 1.0f;
          break;
        }
      }
     
      _default = _value;
      _lastval = -_value;      
    }

    bool stepChange(int steps)
    {
      switch (_type)
      {
        case PT_Float:
        case PT_Coeff:
          _value = ((round((_value - _min) / _step) + (float)steps) * _step) + _min;
          break;
        case PT_Freq:
        case PT_Enum:
          _value += (float)steps;
          break;
        case PT_Bool:
          _value = steps > 0 ? 1.0f : steps < 0 ? 0.0f : _value;
          break;
      }

      if (_value > _max) _value = _max;
      if (_value < _min) _value = _min;

      return _value != _lastval;
    }

    bool scaledChange(float s)
    {
      switch (_type)
      {
        case PT_Float:
        case PT_Coeff:
          _value = map(s, 0.0f, 1.0f, _min, _max);
          break;
        case PT_Freq:
        case PT_Enum:
          _value = round(map(s, 0.0f, 1.0f, _min, _max));
          break;
        case PT_Bool:
          _value = s < 0.5f ? 0.0f : 1.0f;
          break;
      }
      if (_value > _max) _value = _max;
      if (_value < _min) _value = _min;

      _value = round(_value * 100.0f) * 0.01f;

      return _value != _lastval;
   }

    bool change(float value)
    {
      switch (_type)
      {
        case PT_Float:
        case PT_Coeff:
          _value = value;
          break;
        case PT_Enum:
          _value = round(value);
          break;
        case PT_Freq:
        {
          _value = freqToBand(value);
          break;          
        }
        case PT_Bool:
          _value = value < 0.5f ? 0.0f : 1.0f;
          break;
      }

      if (_value > _max) _value = _max;
      if (_value < _min) _value = _min;

      return _value != _lastval;
    }

    bool toggle()
    {
      switch (_type)
      {
        case PT_Float:
        case PT_Freq:
        case PT_Enum:
        case PT_Coeff:
          _value += _step;
          if (_value >= _max) _value = _min;
          break;
        case PT_Bool:
          _value = _value < 0.5f ? 1.0f : 0.0f;
          break;
      } 
      return _value != _lastval; 
    }

    void reset() { _value = _default; }

    bool changed() { return _value != _lastval; }
    bool customized() { return _value != _default; }
    void updated() { _lastval = _value; }

    const char *name() { return _name; }
    ParameterType type() { return _type; }
    float value() { return _type == PT_Freq ? _fbands[(int)_value] : _value; }
    float defaultValue() { return _type == PT_Freq ? _fbands[(int)_default] : _default; }
    float min() { return _type == PT_Freq ? _fbands[(int)_min] : _min; }
    float max() { return _type == PT_Freq ? _fbands[(int)_max] : _max; }
    float step() { return _step; }

    operator float() const { return _type == PT_Freq ? _fbands[(int)_value] : _value; }

    const char *tos()
    {
      int len = sizeof(_buf);
      switch (_type)
      {
        case PT_Float:
        case PT_Coeff:
          snprintf(_buf, len, "%+5.4f", _value); break;
        case PT_Bool: snprintf(_buf, len, " .%s", _value == 0.0f ? "off" : "on"); break;
        case PT_Freq:
        {
          float freq = _fbands[(int)_value];
          if (freq < 1000.0f) snprintf(_buf, len, "%4d", (int)freq);
          else  snprintf(_buf, len, "%+5.2f", freq * 0.001f); break;
          break;
        }
        case PT_Enum: snprintf(_buf, len, ".%s", _labels[(int)_value]); break;
      }
      return _buf;     
    }

  private:
    const char *_name;
    ParameterType _type;
    const char** _labels;
    float _lastval;
    float _value;
    float _default;
    float _min;
    float _max;
    float _step;
    bool _changed;
    char _buf[16];

    static constexpr int _numbands = sizeof(_fbands) / sizeof(_fbands[0]);
    static constexpr int _maxband = _numbands - 1;

    int freqToBand(float freq)
    {
      int band = 0;
      for (; band < _numbands; band++)
      {
        if (_fbands[band] >= freq) break;
      }
      return band;
    }
};

class Effect
{
  public:
    Effect(const char *name, int nparams, Parameter* params, void(*update)(Effect &))
    {
      _name = name;
      _update = update;
      _nparams = nparams;
      _params = params;
      _iparam = 0;

      if (currentEffect == nullptr)
      {
        _next = this;
        _prev = this;
      }
      else
      {
        currentEffect->_prev->_next = this;
        _prev = currentEffect->_prev;
        currentEffect->_prev = this;
        _next = currentEffect;
      }
      currentEffect = this;
    }

    Parameter *param()
    {
      return &_params[_iparam];
    }

    Parameter& operator [](int i)
    {
      return _params[i < 0 || i >= _nparams ? _iparam : i ];
    }

    Parameter& operator [](const char* name)
    {
      for (int i = 0; i < _nparams; i++)
      {
        if (strncmp(name, _params[i].name(), 32) == 0) return _params[i];
      }
      return _params[_iparam];
    }

    Parameter *param(const char* name)
    {
      if (name != nullptr)
      {
        for (int i = 0; i < _nparams; i++)
        {
          if (strncmp(name, _params[i].name(), 32) == 0) return &_params[i];
        }
      }
      return nullptr;
    }

    void nextParam()
    {
      int cp = _iparam;
      do
      {
        if (++cp >= _nparams) cp = 0;
        if (_params[cp].type() != Parameter::PT_Coeff)
        {
          _iparam = cp;
          break;
        }
      } while (_iparam != cp);
    }

    void prevParam()
    {
      int cp = _iparam;
      do
      {
        if (--cp < 0) cp = _nparams - 1;
        if (_params[cp].type() != Parameter::PT_Coeff)
        {
          _iparam = cp;
          break;
        }
      } while (_iparam != cp);
    }

    int selected() { return _iparam; }

    void change(int n, float values[])
    {
      for (int i = 0; i < n && i < _nparams; i++)
      {
        if (_params[i].change(values[i])) _iparam = i;
      }
    }

    void update()
    {
      for (int i = 0; i < _nparams; i++)
      {
        if (_params[i].changed())
        {
          _update(*this);
          for (; i < _nparams; i++) _params[i].updated();
          return;
        }
      }
    }

    const char *name() { return _name; }

    Print *save(Print *p)
    {
      p->printf("%s.reset();\r\n", _name);
      for (int i = 0; i < _nparams; i++)
      {
        if (_params[i].customized())
        {
          p->printf("%s.%s=%.3f;\r\n", _name, _params[i].name(), _params[i].value());
        }
      }
      return p;
    }

    Print *dump(Print *p)
    {
      p->printf("[%s]\r\n", _name);
      for (int i = 0; i < _nparams; i++)
      {
        p->printf("%s=%d,%.3f,%.3f,%.3f,%.3f,%.3f;\r\n", _params[i].name(), _params[i].type(), _params[i].value(), _params[i].defaultValue(), _params[i].min(), _params[i].max(), _params[i].step());
      }
      return p;
    }    

    void reset()
    {
      for (int i = 0; i < _nparams; i++) _params[i].reset();
      update();
    }

    static Effect *current() { return currentEffect; }

    static void next()
    {
      if (currentEffect == nullptr) return;
      currentEffect = currentEffect->_next;
    }

    static void prev()
    {
      if (currentEffect == nullptr) return;
      currentEffect = currentEffect->_prev;
    }

    static Effect* effect(const char* name)
    {
      if (name == nullptr) return nullptr;
      if (currentEffect == nullptr) return nullptr;
      Effect *pe = currentEffect;
      do
      {
        if (strncmp(name, pe->_name, 32) == 0) return pe;
        pe = pe->_next;
      } while (pe != currentEffect);
      return nullptr;      
    }

    static void saveAll(Print *p)
    {
      if (currentEffect == nullptr) return;
      Effect *pe = currentEffect;
      do
      {
        pe->save(p);
        pe = pe->_next;
      } while (pe != currentEffect);      
    }

    static void dumpAll(Print *p)
    {
      if (currentEffect == nullptr) return;
      Effect *pe = currentEffect;
      do
      {
        pe->dump(p);
        pe = pe->_next;
      } while (pe != currentEffect);      
    }

    static void begin()
    {
      if (currentEffect == nullptr) return;
      Effect *pe = currentEffect;
      do
      {
        pe->update();
        pe = pe->_next;
      } while (pe != currentEffect);
    }

  private:
    Effect *_prev;
    Effect *_next;

    static Effect *currentEffect;

    const char *_name;
    void(*_update)(Effect &);

    int _iparam;
    int _nparams;
    Parameter *_params;
};

Effect *Effect::currentEffect;

#define EPARAMS(_n_) Parameter _efxp_ ## _n_[] =
#define EFFECT(_n_) Effect _efx_ ## _n_(#_n_, sizeof(_efxp_ ## _n_) / sizeof(_efxp_ ## _n_[0]), _efxp_ ## _n_, [](Effect &e)-> void 

#endif
