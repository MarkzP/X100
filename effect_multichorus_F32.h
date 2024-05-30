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
      Bypass,
      Default,
      TSC,
      CE2,
      BF2,
      DD1,
      DD2,
      DD3,
      DD4
    };

    AudioEffectMultiChorus_F32(void): AudioStream_F32(1, inputQueueArray)
    {
      reset();
    }

    AudioEffectMultiChorus_F32(const AudioSettings_F32 &settings): AudioStream_F32(1, inputQueueArray),
      _lfo(settings.sample_rate_Hz),
      _dryFilter(settings.sample_rate_Hz),
      _preEffectFilter(settings.sample_rate_Hz),
      _postEffectFilterL(settings.sample_rate_Hz),
      _postEffectFilterR(settings.sample_rate_Hz)
    {
      reset();
    }

    void begin()
    {
      _delay.begin(_delayline, _delay_samples);
    }

    void type(Type ct)
    {
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
      _wet = wet < -1.0f ? -1.0f : wet > 1.0f ? 1.0f : wet;
      apply();
    }

    void dry(float dry)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
      apply();
    }

    virtual void update(void)
    {
      audio_block_f32_t* blockDry = AudioStream_F32::receiveWritable_f32(0);
      if (!blockDry) return;

      if (_type == Bypass)
      {
        AudioStream_F32::transmit(blockDry, 0);
        AudioStream_F32::transmit(blockDry, 1);
        AudioStream_F32::release(blockDry);
        return;
      }

      audio_block_f32_t* blockPre = AudioStream_F32::allocate_f32();
      audio_block_f32_t* blockL = AudioStream_F32::allocate_f32();
      audio_block_f32_t* blockR = AudioStream_F32::allocate_f32();
      if (!blockPre || !blockL || !blockR)
      {
        if (blockR) AudioStream_F32::release(blockR);
        if (blockL) AudioStream_F32::release(blockL);
        if (blockPre) AudioStream_F32::release(blockPre);
        AudioStream_F32::release(blockDry);
        return;
      }

      _preEffectFilter.filterBlock(blockDry, blockPre);

      float feedback = _feedback;
      for (uint16_t i = 0; i < blockPre->length; i++)
      {
        _lfo.increment();

        _delay.write(blockPre->data[i] + feedback);

        feedback = 0.0f;
        float L = 0.0f;
        float R = 0.0f;

        _voices.process(_lfo, _delay, feedback, L, R);

        blockL->data[i] = L;
        blockR->data[i] = R;
      }
      _feedback = feedback;

      _dryFilter.filterBlock(blockDry);
      BlockOperations::scale(blockDry, _mix_dry);

      _postEffectFilterL.filterBlock(blockL);
      _postEffectFilterR.filterBlock(blockR);

      BlockOperations::add(blockDry, blockL);
      BlockOperations::add(blockDry, blockR);

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
      AudioStream_F32::release(blockPre);
      AudioStream_F32::release(blockDry);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    
    static constexpr int _max_voices = 4;
    static constexpr float _delay_time_ms = 17.0f;

    typedef TriangleLfo Lfo;
    typedef LQModDelay ChorusDelay;
    
    static constexpr int _delay_samples = ChorusDelay::bufferSize(_delay_time_ms);
    int16_t _delayline[_delay_samples];

    class Voice
    {
      public:      
        Voice& reset()
        {
          _active = false;
          _offset = 0.0f;
          _min = 0.0f;
          _range = 0.0f;
          _depth = 1.0f;
          _smooth = 0.001f;
          _resonance = 0.0f;
          _mixL = 0.0f;
          _mixR = 0.0f;
          _wet = 0.0f;
          _wetL = 0.0f;
          _wetR = 0.0f;
          return *this;
        }

        Voice& phase(float deg)
        {
          _offset = deg * Lfo::Degree;
          return *this;
        }

        Voice& min(float min)
        {
          _min = delayToSamples(min);
          return *this;
        }

        Voice& range(float range)
        {
          _range = delayToSamples(range);
          return *this;
        }

        Voice& depth(float depth)
        {
          _depth = (depth < 0.0f ? 0.0f : depth > 1.0f ? 1.0f : depth) * 0.5f;
          return *this;
        }

        Voice& smooth(float smooth)
        {
          _smooth = smooth < 0.0f ? 0.0f : smooth > 1.0f ? 1.0f : smooth;
          return *this;
        }

        Voice& resonance(float resonance)
        {
          _resonance = resonance < 0.0f ? 0.0f : resonance > 1.0f ? 1.0f : resonance;
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

        Voice& wet(float wet)
        {
          _wet = wet;
          return *this;
        }

        void begin()
        {
          _wetL = _wet * _mixL;
          _wetR = _wet * _mixR;
          _active = _wetL != 0.0f || _wetR != 0.0f || _resonance != 0.0f;
          
          // Serial.printf("_offset=%.2f, _min=%.1f, _range=%.1f, _depth=%.2f, _smooth=%.5f, _resonance=%.2f, _wetL=%.2f, _wetR=%.2f, _active=%d\n", _offset, _min, _range, _depth, _smooth, _resonance, _wetL, _wetR, _active);
        }

        void process(Lfo& lfo, ChorusDelay& delay, float& feedback, float&L, float& R)
        {
          if (!_active) return;

          // convert -1.0 to 1.0 -> 0.0 to 1.0, centered at 0.5 when depth = 0.0
          float phase = (lfo.peek(_offset) * _depth) + 0.5f;

          // filter modulation factor
          _mod += (phase - _mod) * _smooth;

          // convert to samples
          float mod_index = (_mod * _range) + _min;

          float sample = delay.read(mod_index);

          feedback += sample * _resonance;
          L += sample * _wetL;
          R += sample * _wetR;
        }

      private:
        static constexpr float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
        bool _active = false;
        float _offset = 0.0f;
        float _min = 0.0f;
        float _range = 0.0f;
        float _depth = 1.0f;
        float _smooth = 0.001f;
        float _resonance = 0.0f;
        float _mixL = 0.0f;
        float _mixR = 0.0f;
        float _wet = 0.0f;
        float _wetL = 0.0f;
        float _wetR = 0.0f;

        float _mod = 0.0f;

        float delayToSamples(float ms)
        {
          ms = ms < 0.0f ? 0.0f : ms;
          return ms * _sample_rate_Hz * 0.001f;
        }
    };

    template <int N = 4>
    class Voices
    {
      public:
        void process(Lfo& lfo, ChorusDelay& delay, float& feedback, float&L, float& R)
        {
          for (int i = 0; i < N; i++) _v[i].process(lfo, delay, feedback, L, R);
        }
        
        Voice& operator[](int index)
        {
          return _v[index];
        }
        
        Voices& reset()
        {
          for (int i = 0; i < N; i++) _v[i].reset();
          return *this;
        }

        Voices& min(float min)
        {
          for (int i = 0; i < N; i++) _v[i].min(min);
          return *this;
        }

        Voices& range(float range)
        {
          for (int i = 0; i < N; i++) _v[i].range(range);
          return *this;
        }

        Voices& depth(float depth)
        {
          for (int i = 0; i < N; i++) _v[i].depth(depth);
          return *this;
        }

        Voices& smooth(float smooth)
        {
          for (int i = 0; i < N; i++) _v[i].smooth(smooth);
          return *this;
        }

        Voices& wet(float wet)
        {
          for (int i = 0; i < N; i++) _v[i].wet(wet);
          return *this;
        }
        
        void begin()
        {
          for (int i = 0; i < N; i++) _v[i].begin();
        }

      private:
        Voice _v[N];
    };

    // controls
    Type _type = Bypass;
    float _rate = 0.5f;
    float _depth = 0.5f;
    float _resonance = 0.5f;
    float _color = 0.5f;
    float _wet = 1.0f;
    float _dry = 1.0f;

    Lfo _lfo;
    ChorusDelay _delay;
    CascadeBiquad<> _dryFilter;
    CascadeBiquad<> _preEffectFilter;
    CascadeBiquad<> _postEffectFilterL;
    CascadeBiquad<> _postEffectFilterR;
    Voices<_max_voices> _voices;

    float _feedback = 0.0f;
    float _mix_dry = 1.0f;
    float _mix_wet = 0.0f;

    float mapExp(float value, float min = 0.0f, float max = 1.0f, float x = 2.0f)
    {
      return (powf(value, x) * (max - min)) + min;
    }

    void reset()
    {
      _mix_wet = 0.0f;
      _mix_dry = 1.0f;
      _voices.reset();
      _voices[0].phase(000.0f);
      _voices[1].phase(120.0f);
      _voices[2].phase(240.0f);
      _voices[3].phase(180.0f);
      _dryFilter.reset();
      _preEffectFilter.reset();
      _postEffectFilterL.reset();
      _postEffectFilterR.reset();
    }

    void beginAll()
    {
      _dryFilter.begin();
      _preEffectFilter.begin();
      _postEffectFilterL.begin();
      _postEffectFilterR.begin();

      _mix_wet = (1.0f - (fabsf(_dry) * 0.45f)) * _wet;
      _mix_dry = (1.0f - (fabsf(_wet) * 0.45f)) * _dry;

      _voices.wet(_mix_wet).begin();
    }

    void apply(bool typeChanged = false)
    {
      switch (_type)
      {
        case Default:
          if (typeChanged)
          {
            reset();
            _preEffectFilter
                .setHighpass(100.0f);
            _voices
                .range(6.0f)
                .smooth(0.001f);
            _voices[0]
                .mixL(1.0f);
            _voices[1]
                .range(1.8f)
                .mixL(-0.6f)
                .mixR(0.6f);
            _voices[2]
                .mixR(1.0f);
            _voices[3]
                .range(4.0f);
          }

          _lfo.freq(mapExp(_rate, 0.2f, 3.0f));
          _voices
              .min(mapExp(_color, 6.0f, 12.0f))
              .depth(mapExp(_depth, 0.2f, 1.0f));
          _voices[1]
              .min(mapExp(_color, 2.0f, 5.0f));
          _voices[3]
              .min(0.0f)
              .resonance(mapExp(_resonance, 0.3f, 0.83f));
          break;

        case TSC:
          if (typeChanged)
          {
            reset();
            _preEffectFilter
                .setLowpass(9000.0f);
            _voices.min(3.0f);
            _voices[0]
                .range(13.0f)
                .mixL(1.0)
                .mixR(0.014f);
            _voices[1]
                .range(10.0f)
                .mixL(-0.45f)
                .mixR(0.45f);
            _voices[2]
                .range(14.0f)
                .mixL(0.014f)
                .mixR(1.0);
            _voices[3]
                .min(0.5f)
                .range(5.0f);
          }

          _lfo.freq(mapExp(_rate, 0.2f, 3.5f, 3.0f));
          _voices
            .depth(mapExp(_depth, 0.2f, 1.0f))
            .smooth(mapExp(_color, 0.00002f, 0.0075f, 3.0f));
          _voices[3]
            .resonance(mapExp(_resonance, 0.3f, 0.8f));
          break;

        case CE2:
          if (typeChanged)
          {
            reset();
            _preEffectFilter
                .setHighpass1p1z(100.0f);
            _postEffectFilterL
                .setLowpass(6900.0f);
            _postEffectFilterR
                .setLowpass(6900.0f);
            _voices[0]
                .range(7.0f)
                .smooth(0.075f)
                .mixL(1.0)
                .mixR(-1.0f);
          }

          _lfo.freq(mapExp(_rate, 0.3f, 3.5f));
          _voices
              .min(mapExp(_color, 3.0f, 7.5f, 1.0f))
              .depth(mapExp(_depth, 0.3f, 1.0f));
          break;

        case BF2:
          if (typeChanged)
          {
            reset();
            _postEffectFilterL
                .setLowpass(6900.0f);
            _postEffectFilterR
                .setLowpass(6900.0f);
            _voices[0]
                .range(8.0f)
                .smooth(0.0075f)
                .mixL(1.0f)
                .mixR(-1.0f);
          }

          _lfo.freq(mapExp(_rate, 0.0625, 6.5f, 3.5f));
          _voices[0]
              .min(mapExp(_color, 0.0f, 0.5f))
              .depth(mapExp(_depth, 0.3f, 1.0f, 3.0f))
              .resonance(mapExp(_resonance, 0.6f, 0.85f));
          break;

        case DD1:
        case DD2:
        case DD3:
        case DD4:
          if (typeChanged)
          {
            reset();
            _lfo.freq(_type == DD1 || _type == DD2 ? 0.25f : 0.5f);
            _preEffectFilter
                .setHighpass(100.0f);
            _postEffectFilterL
                .setLowpass(9600.0f);
            _postEffectFilterR
                .setLowpass(9600.0f);
            _voices
                .min(_type == DD1 ? 8.0f : _type == DD2 ? 5.0f : 6.0f)
                .range(_type == DD1 ? 4.0f : _type == DD2 ? 5.0f : 3.0f);
            _voices[0]
                .mixL(_type == DD4 ? 0.38f : 1.0f)
                .mixR(-0.38f);
            _voices[3]
                .mixL(-0.38f)
                .mixR(_type == DD4 ? 0.38f : 1.0f);
          }
          break;

        default: // bypass
          if (typeChanged) reset();
          break;
      }

      beginAll();
    }
};

#endif
