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
      DD4,
      Test
    };

    AudioEffectMultiChorus_F32(void): AudioStream_F32(1, inputQueueArray)
    {
      reset();
    }

    AudioEffectMultiChorus_F32(const AudioSettings_F32 &settings): AudioStream_F32(1, inputQueueArray),
      _blockPre(settings),
      _blockL(settings),
      _blockR(settings),
      _dryFilter(settings.sample_rate_Hz),
      _preEffectFilter(settings.sample_rate_Hz),
      _postEffectFilterL(settings.sample_rate_Hz),
      _postEffectFilterR(settings.sample_rate_Hz)
    {
      reset();
    }

    void begin()
    {
      _voices.begin(_delayline, _delay_samples);
      apply(true);
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
      audio_block_f32_t *blockDry = AudioStream_F32::receiveWritable_f32(0);
      if (!blockDry) return;

      if (_type == Bypass)
      {
        AudioStream_F32::transmit(blockDry, 0);
        AudioStream_F32::transmit(blockDry, 1);
        AudioStream_F32::release(blockDry);
        return;
      }

      audio_block_f32_t *blockPre = &_blockPre;
      audio_block_f32_t *blockL = &_blockL;
      audio_block_f32_t *blockR = &_blockR;

      _preEffectFilter.filterBlock(blockDry, blockPre);

      _voices.processBlock(blockPre, blockL, blockR);

      _dryFilter.filterBlock(blockDry);
      BlockOperations::scale(blockDry, _dry);

      _postEffectFilterL.filterBlock(blockL);
      _postEffectFilterR.filterBlock(blockR);
      BlockOperations::scale(blockL, _wet);
      BlockOperations::scale(blockR, _wet);

      BlockOperations::add(blockDry, blockL);
      BlockOperations::add(blockDry, blockR);

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockDry);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    audio_block_f32_t _blockPre;
    audio_block_f32_t _blockL;
    audio_block_f32_t _blockR;
    
    static constexpr int _max_voices = 4;
    static constexpr float _delay_time_ms = 20.0f;

    typedef SineLfo Lfo;

#ifdef LQ
hkkh
    typedef LQDelay ChorusDelay;
#else
    typedef Delay ChorusDelay;
#endif
    
    static constexpr float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    static constexpr int _delay_samples = ChorusDelay::bufferSizeMs(_delay_time_ms);
    ChorusDelay::CDT _delayline[_delay_samples];
    
    static float delayToSamples(float ms)
    {
      ms = ms < 0.0f ? 0.0f : ms;
      return ms * _sample_rate_Hz * 0.001f;
    }

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
          _mixL = 0.0f;
          _mixR = 0.0f;
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

        void begin()
        {
          _active = _mixL != 0.0f || _mixL != 0.0f;
          
          // Serial.printf("_offset=%.2f, _min=%.1f, _range=%.1f, _depth=%.2f, _mixL=%.2f, _mixR=%.2f, _active=%d\n", _offset, _min, _range, _depth, _mixL, _mixR, _active);
        }

        void process(Lfo& lfo, ChorusDelay& delay, float *L, float *R)
        {
          if (!_active) return;

          // convert -1.0 to 1.0 -> 0.0 to 1.0, centered at 0.5 when depth = 0.0
          float mod = (lfo.peek(_offset) * _depth) + 0.5f;

          // convert to samples
          float mod_index = (mod * _range) + _min;

          float sample = delay.read(mod_index);

          *L += sample * _mixL;
          *R += sample * _mixR;
        }

      private:
        bool _active = false;
        float _offset = 0.0f;
        float _min = 0.0f;
        float _range = 0.0f;
        float _depth = 1.0f;
        float _mixL = 0.0f;
        float _mixR = 0.0f;
    };

    template <int N = 4>
    class Voices
    {
      public:
        void begin(ChorusDelay::CDT *delay, int d_length)
        {
          _delay.begin(delay, d_length);
        }

        inline void process(float sample, float *L, float *R)
        {
          _lfo.next();

          float feedback = _feedback * _delay.read(_fb_samples);
          _delay.write(sample + feedback);
          
          *L = 0.0f;
          *R = 0.0f;

          for (int i = 0; i < N; i++) _v[i].process(_lfo, _delay, L, R);
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
          _feedback = 0.0f;
          _fb_samples = 0.0f;
          for (int i = 0; i < N; i++) _v[i].reset();
          return *this;
        }

        Voices& freq(float freq)
        {
          _lfo.freq(freq);
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

        Voices& feedback(float feedback, float time)
        {
          _feedback = feedback < -1.0f ? -1.0f : feedback > 1.0f ? 1.0f : feedback;
          _fb_samples = delayToSamples(time < 0.0f ? 0.0f : time);
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
        Lfo _lfo;
        ChorusDelay _delay;
        float _feedback = 0.0f;
        float _fb_samples = 0.0f;
    };

    // controls
    Type _type = Bypass;
    float _rate = 0.5f;
    float _depth = 0.5f;
    float _resonance = 0.5f;
    float _feedback = 0.0f;
    float _color = 0.5f;
    float _wet = 1.0f;
    float _dry = 1.0f;

    CascadeBiquad<> _dryFilter;
    CascadeBiquad<> _preEffectFilter;
    CascadeBiquad<> _postEffectFilterL;
    CascadeBiquad<> _postEffectFilterR;
    Voices<_max_voices> _voices;

    float mapExp(float value, float min = 0.0f, float max = 1.0f, float x = 2.0f)
    {
      return (powf(value, x) * (max - min)) + min;
    }

    void reset()
    {
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
      _voices.begin();
    }

    void apply(bool typeChanged = false)
    {
      switch (_type)
      {
        case Default:
          if (typeChanged)
          {
            reset();
            _voices
                .range(6.0f);
            _voices[0]
                .mixL(1.0f);
            _voices[1]
                .range(1.8f)
                .mixL(-0.6f)
                .mixR(0.6f);
            _voices[2]
                .mixR(1.0f);
          }

          _voices
              .freq(mapExp(_rate, 0.2f, 3.0f))
              .min(mapExp(_color, 6.0f, 12.0f))
              .depth(mapExp(_depth, 0.2f, 1.0f))
              .feedback(-_resonance * 0.7f, mapExp(_color, 6.0f, 12.0f) + mapExp(_depth, 0.2f, 1.0f));
          _voices[1]
              .min(mapExp(_color, 2.0f, 5.0f));
          break;

        case TSC:
          if (typeChanged)
          {
            reset();
            _preEffectFilter
                .setLowpass(9000.0f);
            _voices
                .min(3.0f);
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
          }

          _voices.freq(mapExp(_rate, 0.2f, 3.5f, 3.0f));
          _voices
            .depth(mapExp(_depth, 0.2f, 1.0f));
          break;

        case CE2:
          if (typeChanged)
          {
            reset();
            _preEffectFilter
                .setHighpass1p1z(150.0f);
            _postEffectFilterL
                .setLowpass(6900.0f);
            _postEffectFilterR
                .setLowpass(6900.0f);
            _voices[0]
                .range(7.0f)
                .mixL(1.0)
                .mixR(-1.0f);
          }

          _voices.freq(mapExp(_rate, 0.3f, 3.5f));
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
                .mixL(1.0f)
                .mixR(1.0f);
          }

          _voices.freq(mapExp(_rate, 0.0625, 6.5f, 3.5f));
          _voices[0]
              .min(mapExp(_color, 0.0f, 0.5f))
              .depth(mapExp(_depth, 0.3f, 1.0f, 3.0f));
          break;

        case DD1:
        case DD2:
        case DD3:
        case DD4:
          if (typeChanged)
          {
            reset();
            _voices.freq(_type == DD1 || _type == DD2 ? 0.25f : 0.5f);
            _preEffectFilter
                .setHighpass1p1z(150.0f);
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

        case Test:
          if (typeChanged)
          {
            reset();
            _voices
                .range(20.0f);
            _voices[0]
                .mixL(1.0f)
                .mixR(1.0f);
          }

          _voices
            .depth(_depth)
            .freq(_rate);
          break;

        default: // bypass
          if (typeChanged) reset();
          break;
      }

      beginAll();
    }
};

#endif
