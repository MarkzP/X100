#ifndef _effect_trichorus_F32_h_
#define _effect_trichorus_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectMultiChorus_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:2  //this line used for automatic generation of GUI node
    //GUI: shortName:effect_MultiChorus
  public:
    enum Type
    {
      Default,
      TSC,
      CE2,
      BF2,
      DD1,
      DD2,
      DD3,
      DD4,
      Bypass
    };

    AudioEffectMultiChorus_F32(void): AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioEffectMultiChorus_F32(const AudioSettings_F32 &settings): AudioStream_F32(1, inputQueueArray),
      _blockL(settings),
      _blockR(settings),
      _voices(settings.sample_rate_Hz)
    {
    }

    void begin()
    {
      apply(true);
    }

    void preset(int preset)
    {
      Type ct = (Type)preset;
      if (ct == _type) return;
      _type = ct;
      apply(true);
    }

    void rate(float rate)
    {
      _rate = rate < 0.0f ? 0.0f : rate > 1.0f ? 1.0f : rate;
      apply();
    }

    void depth(float depth)
    {
      _depth = depth < 0.0f ? 0.0f : depth > 1.0f ? 1.0f : depth;
      apply();
    }

    void resonance(float resonance)
    {
      _resonance = resonance < 0.0f ? 0.0f : resonance > 1.0f ? 1.0f : resonance;
      apply();
    }

    void color(float color)
    {
      _color = color < 0.0f ? 0.0f : color > 1.0f ? 1.0f : color;
      apply();
    }

    void wet(float wet)
    {
      _voices.wet(wet);
    }

    void dry(float dry)
    {
      _voices.dry(dry);
    }

    void enable(bool enable = false)
    {
      _voices.enable(enable);
    }

    virtual void update(void)
    {
      audio_block_f32_t *blockDry = AudioStream_F32::receiveReadOnly_f32(0);
      if (!blockDry) return;

      _voices.processBlock(blockDry, &_blockL, &_blockR);

      AudioStream_F32::transmit(&_blockL, 0);
      AudioStream_F32::transmit(&_blockR, 1);

      AudioStream_F32::release(blockDry);
    }

  private:
    
    class Voice
    {
      public:
        Voice(float sample_rate_Hz, const LfoBase& lfo, const DelayBase& delay, float defaultPhase):
          _defaultOffset(defaultPhase * LfoBase::Degree),
          _lfo(lfo),
          _delay(delay),
          _hpf(sample_rate_Hz),
          _lpf(sample_rate_Hz)
        {}

        Voice& reset()
        {
          _hpf.setBypass();
          _lpf.mix(1.0f, 0.0f, 0.0f, 0.0f);
          _lpf.resonance(0.7f);
          _lpf.frequency(10000.0f);
          _active = false;
          _offset = _defaultOffset;
          _min = 0.0f;
          _range = 0.0f;
          _depth = 0.5f;
          _mixL = 0.0f;
          _mixR = 0.0f;
          _resonance = 0.0f;
          return *this;
        }

        Voice& phase(float deg)
        {
          _offset = deg * LfoBase::Degree;
          return *this;
        }

        Voice& hpf(float freq)
        {
          _hpf.setHighpass((double)freq);
          return *this;
        }

        Voice& lpf(float low, float high = 0.0f, float resonance = 0.7f)
        {
          if (high < low) high = low;
          
          _lpCenter = (high + low) * 0.5f;
          _lpRange = (high - low) * 0.5f;

          _lpf.mix(0.0f, 1.0f, 1.0f, 0.0f);
          _lpf.resonance(resonance);

          return *this;
        }

        Voice& min(float min)
        {
          _min = DelayBase::msToSamples(min);
          return *this;
        }

        Voice& range(float range)
        {
          _range = DelayBase::msToSamples(range);
          return *this;
        }

        Voice& depth(float depth)
        {
          _depth = (depth < 0.0f ? 0.0f : depth > 1.0f ? 1.0f : depth) * 0.5f;
          return *this;
        }

        Voice& mixL(float mixL)
        {
          _mixL = mixL;
          return *this;
        }

        Voice& mixR(float mixR)
        {
          _mixR = mixR;
          return *this;
        }

        Voice& resonance(float resonance)
        {
          _resonance = resonance;
          return *this;
        }

        void begin()
        {
          _active = _mixL != 0.0f || _mixR != 0.0f || _resonance != 0.0f;
        }

        void process(float *pL, float *pR, float *pFB)
        {
          if (!_active) return;

          float lfo = _lfo.peek(_offset);

          // convert -1.0 to 1.0 -> 0.0 to 1.0, centered at 0.5 when depth = 0.0
          float delayMod = (lfo * _depth) + 0.5f;
          float delayIndex = (delayMod * _range) + _min;
          float sample = _delay.read(delayIndex);

          _lpf.frequency((lfo * _lpRange) + _lpCenter);

          sample = _hpf.filter(sample);
          sample = _lpf.filter(sample);

          *pL += sample * _mixL;
          *pR += sample * _mixR;
          *pFB += sample * _resonance;
        }

      private:
        const float _defaultOffset;
        const LfoBase& _lfo;
        const DelayBase& _delay;
        Biquad _hpf;
        StateVariableFilter<2> _lpf;
        bool _active = false;
        float _offset = _defaultOffset;
        float _min = 0.0f;
        float _range = 0.0f;
        float _depth = 1.0f;
        float _lpCenter = 12000.0f;
        float _lpRange = 0.0f;
        float _mixL = 0.0f;
        float _mixR = 0.0f;
        float _resonance = 0.0f;
    };


    class Voices
    {
      public:
        Voices(float sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT):
          _v{
            { sample_rate_Hz, _lfoA, _delay, 0.0f },
            { sample_rate_Hz, _lfoA, _delay, 120.0f },
            { sample_rate_Hz, _lfoA, _delay, 180.0f },
            { sample_rate_Hz, _lfoB, _delay, 0.0f },
            { sample_rate_Hz, _lfoB, _delay, 90.0f },
          }
        { }

        void process(float sample, float *pL, float *pR)
        {
          _lfoA.next();
          _lfoB.next();

          _delay.write(sample + _feedback);
          _feedback = 0.0f;
          
          if (_enable)
          {
            *pL = 0.0f;
            *pR = 0.0f;

            for (int i = 0; i < _nv; i++) _v[i].process(pL, pR, &_feedback);

            *pL = (sample * _dry) + (*pL * _wet);
            *pR = (sample * _dry) + (*pR * _wet);
          }
          else
          {
            *pL = sample;
            *pR = sample;
          }
        }

        void processArray(float *pSrc, float *pDstL, float *pDstR, uint32_t len)
        {
          if (pSrc == nullptr || pDstL == nullptr || pDstR == nullptr || len == 0) return;

          float *end = pSrc + len;
          do
          {
            process(*pSrc++, pDstL++, pDstR++);
          }
          while (pSrc < end);          
        }

        void processBlock(audio_block_f32_t *blockPre, audio_block_f32_t *blockL, audio_block_f32_t *blockR)
        {
          processArray(blockPre->data, blockL->data, blockR->data, blockPre->length);
        }
        
        Voice& operator[](int index)
        {
          return _v[index];
        }
        
        Voices& reset()
        {
          _lfoA.harmonic(1, 0.0f);
          for (int i = 0; i < _nv; i++) _v[i].reset();
          return *this;
        }

        Voices& rate(float rateA, float rateB = 0.0f)
        {
          _lfoA.freq(rateA);
          _lfoB.freq(rateB);
          return *this;
        }

        Voices& harmonic(int order, float level)
        {
          _lfoA.harmonic(order, level);
          return *this;
        }

        Voices& hpf(float hpf)
        {
          for (int i = 0; i < _nv; i++) _v[i].hpf(hpf);
          return *this;
        }

        Voices& lpf(float low, float high = 0.0f, float resonance = 0.7f)
        {
          for (int i = 0; i < _nv; i++) _v[i].lpf(low, high, resonance);
          return *this;
        }

        Voices& min(float min)
        {
          for (int i = 0; i < _nv; i++) _v[i].min(min);
          return *this;
        }

        Voices& range(float range)
        {
          for (int i = 0; i < _nv; i++) _v[i].range(range);
          return *this;
        }

        Voices& depth(float depth)
        {
          for (int i = 0; i < _nv; i++) _v[i].depth(depth);
          return *this;
        }

        Voices& dry(float dry)
        {
          _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
          return *this;
        }

        Voices& wet(float wet)
        {
          _wet = wet < -1.0f ? -1.0f : wet > 1.0f ? 1.0f : wet;
          return *this;
        }

        Voices& enable(bool enable)
        {
          _enable = enable;
          return *this;
        }

        void begin()
        {
          for (int i = 0; i < _nv; i++) _v[i].begin();
        }

      private:
        static constexpr int _nv = 5;
        static constexpr float _delay_time_ms = 18.0f;
        ModifiedSineLfo _lfoA;
        SineLfo _lfoB;
        RAM1Delay< DelayBase::bufferSizeMs(_delay_time_ms) > _delay;
        Voice _v[_nv];
        float _feedback = 0.0f;
        float _dry = 1.0f;
        float _wet = 1.0f;
        bool _enable = false;
    };

    audio_block_f32_t *inputQueueArray[1];
    audio_block_f32_t _blockL;
    audio_block_f32_t _blockR;
    Voices _voices;

    // controls
    Type _type = Bypass;
    float _rate = 0.5f;
    float _depth = 0.5f;
    float _resonance = 0.5f;
    float _color = 0.5f;

    static float mapExp(float value, float min = 0.0f, float max = 1.0f, float x = 2.0f)
    {
      return (powf(value, x) * (max - min)) + min;
    }

    void apply(bool typeChanged = false)
    {
      switch (_type)
      {
        case Default:
          if (typeChanged)
          {
            _voices.reset()
                .hpf(50.0f)
                .lpf(1200.0f, 8000.0f);
            _voices[0]
                .range(6.0f)
                .mixL(1.0f);
            _voices[1]
                .range(6.0f)
                .mixR(1.0f);
            _voices[3]
                .hpf(500.0f)
                .lpf(500.0f, 10000.0f, 1.5f)
                .range(1.8f)
                .mixL(-0.5f)
                .mixR(0.5f);
            _voices.begin();
          }

          _voices.rate(mapExp(_rate, 0.2f, 3.0f), mapExp(_rate, 0.237f, 4.219f, 3.0f))
              .min(mapExp(_color, 8.0f, 14.0f))
              .depth(mapExp(_depth, 0.2f, 1.0f));
          _voices[0].resonance(mapExp(_resonance, 0.0f, -0.7f));
          _voices[3].min(mapExp(_color, 2.0f, 7.0f));
          break;

        case TSC:
          if (typeChanged)
          {
            _voices.reset()
                .range(7.0f)
                .hpf(150.0f)
                .lpf(9000.0f);
            _voices[0]
                .mixL(0.986f)
                .mixR(-0.014f);
            _voices[1]
                .mixL(0.45f)
                .mixR(0.45f);
            _voices[2]
                .phase(240.0f)
                .mixL(-0.014f)
                .mixR(0.986f);
            _voices.begin();
          }

          _voices.min(mapExp(_color, 1.0f, 5.0f))
              .rate(mapExp(_rate, 0.1f, 3.0f, 3.0f))
              .depth(mapExp(_depth, 0.2f, 1.0f))
              .harmonic(7, mapExp(_resonance, 0.0f, 2.0f) * (1.0f - _rate) * (1.0f - _depth));
          break;

        case CE2:
          if (typeChanged)
          {
            _voices.reset();
            _voices[0]
                .range(9.0f)
                .hpf(150.0f)
                .lpf(6900.0f)
                .mixL(1.0)
                .mixR(-1.0f)
                .begin();
          }

          _voices.rate(mapExp(_rate, 0.3f, 3.5f))
              .min(mapExp(_color, 3.0f, 7.5f, 1.0f))
              .depth(mapExp(_depth, 0.3f, 1.0f));
          break;

        case BF2:
          if (typeChanged)
          {
            _voices.reset()
                .range(8.0f)
                .hpf(350.0f)
                .lpf(9600.0f);
            _voices[0].phase(0.0f)
                .mixL(1.0f);
            _voices[1].phase(90.0f)
                .mixR(1.0f);
            _voices.begin();
          }

          _voices.rate(mapExp(_rate, 0.0625, 6.5f, 3.5f))
            .min(mapExp(_color, 0.01f, 0.25f))
            .depth(mapExp(_depth, 0.25f, 1.0f, 3.0f));
          _voices[0].resonance(mapExp(_resonance, 0.4f, 0.8f));
          break;

        case DD1:
        case DD2:
        case DD3:
        case DD4:
          if (typeChanged)
          {
            _voices.reset()
                .hpf(80.0f)
                .lpf(9600.0f);
            _voices[0].phase(0.0f)
                .mixL(_type == DD4 ? 0.35f : 1.0f)
                .mixR(-0.8f);
            _voices[1].phase(180.0f)
                .mixR(_type == DD4 ? 0.35f : 1.0f)
                .mixL(-0.8f);
            _voices.begin();
          }

          _voices.rate(mapExp(_rate, 0.8f, 1.2f) * (_type == DD1 || _type == DD2 ? 0.3f : 0.5f))
              .min(mapExp(_color, 0.8f, 1.2f) * (_type == DD1 ? 8.0f : _type == DD2 ? 5.0f : 6.0f))
              .range(mapExp(_depth, 0.8f, 1.2f) * (_type == DD1 ? 4.0f : _type == DD2 ? 5.0f : 3.0f));
          break;

        default: // bypass
          if (typeChanged) _voices.reset();
          break;
      }
    }
};

#endif
