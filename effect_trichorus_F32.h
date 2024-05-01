#ifndef effect_trichorus_F32_h_
#define effect_trichorus_F32_h_

class AudioEffectTriChorus_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:2  //this line used for automatic generation of GUI node
    //GUI: shortName:effect_TriChorus
  public:
    enum Type
    {
      Bypass,
      Default,
      CE2,
      TSC,
      DD1,
      DD2,
      DD3,
      DD4
    };

    AudioEffectTriChorus_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    }

    AudioEffectTriChorus_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
    }

    boolean begin(float *delayline, uint16_t delay_length)
    {
      _delayline = NULL;
      _delay_length = 0;
      _max_delay_length = 0;
      _w_index = 0;

      if (delayline == NULL) return false;

      _delayline = delayline;
      _delay_length = delay_length;
      _max_delay_length = delay_length;
      memset(_delayline, 0, delay_length * sizeof(float));

      apply();

      return true;
    }

    void chorusType(Type ct)
    {
      if (ct == _ct) return;
      _ct = ct;
      apply();
    }

    void rate(float rate)
    {
      rate = rate < 0.0f ? 0.0f : rate > 1.0f ? 1.0f : rate;
      if (rate == _rate) return;
      _rate = rate;
      apply();
    }

    void depth(float depth)
    {
      depth = depth < 0.0f ? 0.0f : depth > 1.0f ? 1.0f : depth;
      if (depth == _depth) return;
      _depth = depth;
      apply();
    }

    virtual void update(void)
    {
      if (!_delayline) return;

      audio_block_f32_t* blockL = AudioStream_F32::receiveWritable_f32(0);
      if (!blockL) return;

      audio_block_f32_t* blockR = AudioStream_F32::allocate_f32();
      if (!blockR)
      {
        AudioStream_F32::release(blockL);
        return;
      }

      uint16_t num_delay = _num_delay;
      uint16_t w_index = _w_index;
      float phase = _phase;
      float phaseIncrement = _phaseIncrement;
      float hpf = _hpf;
      float lpf1 = _lpf1;
      float lpf2 = _lpf2;
      float hpa = _hpa;
      float lpa = _lpa;
      float dp1 = _dp1;
      float dp2 = _dp2;
      float dp3 = _dp3;
      float mix_dry = _mix_dry;
      float mix_1_l = _mix_1_l;
      float mix_2_l = _mix_2_l;
      float mix_3_l = _mix_3_l;
      float mix_1_r = _mix_1_r;
      float mix_2_r = _mix_2_r;
      float mix_3_r = _mix_3_r;
      float res = _res;

      for (uint16_t i = 0; i < blockL->length; i++)
      {
        float dry = blockL->data[i];

        // Filter input sample
        float filtered = dry;
        filtered -= (hpf += (filtered - hpf) * hpa);
        filtered = (lpf1 += (filtered - lpf1) * lpa);
        filtered = (lpf2 += (filtered - lpf2) * lpa);
        _delayline[w_index] = filtered;

        // lfo
        phase += phaseIncrement;
        while (phase >= _maxPhase) phase -= _maxPhase;

        // Interpolate
        float d1 = num_delay < 1 ? 0.0f : interpolate(w_index, dp1, phase);
        float d2 = num_delay < 2 ? 0.0f : interpolate(w_index, dp2, num_delay < 3 ? _maxPhase - phase : phase + (_oneThird * _maxPhase));
        float d3 = num_delay < 3 ? 0.0f : interpolate(w_index, dp3, phase + (_twoThirds * _maxPhase));

        // Mix output
        blockL->data[i] = (dry * mix_dry) + (d1 * mix_1_l) + (d2 * mix_2_l) + (d3 * mix_3_l);
        blockR->data[i] = (dry * mix_dry) + (d1 * mix_1_r) + (d2 * mix_2_r) + (d3 * mix_3_r);

        // Resonance
        _delayline[w_index] += d1 * res;

        w_index++;
        if (w_index >= _delay_length) w_index = 0;
      }

      _w_index = w_index;
      _phase = phase;
      _hpf = hpf;
      _lpf1 = lpf1;
      _lpf2 = lpf2;

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 0);
      AudioStream_F32::release(blockL);
      AudioStream_F32::release(blockR);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    float _sample_rate_Hz;

    // mode
    Type _ct = Default;
    uint16_t _num_delay = 3;
    float _rate = 0.5f;
    float _depth = 0.5f;

    // lfo
    static constexpr float _oneThird = 1.0f / 3.0f;
    static constexpr float _twoThirds = 2.0f / 3.0f;
    static constexpr float _twoPi = 6.2831853f;
    float _phase;
    float _phaseIncrement;
    static constexpr float _maxPhase = 2.0f;
    static constexpr float _halfMaxPhase = _maxPhase / 2.0f;
    static constexpr float _oneOverHalfMaxPhase = 1.0f / _halfMaxPhase;

    // delay
    float *_delayline;  // pointer for the circular buffer
    uint16_t _w_index;   // current write pointer of the circular buffer
    uint16_t _delay_length; // calculated number of samples of the delay
    uint16_t _max_delay_length;

    // filter
    float _hpf = 0.0f;
    float _hpa = 0.0f;
    float _lpf1 = 0.0f;
    float _lpf2 = 0.0f;
    float _lpa = 1.0f;

    // modulation
    float _dp1 = 0.0f;
    float _dp2 = 0.0f;
    float _dp3 = 0.0f;

    // mixer
    float _mix_dry = 1.0f;
    float _mix_1_l = 0.0f;
    float _mix_2_l = 0.0f;
    float _mix_3_l = 0.0f;
    float _mix_1_r = 0.0f;
    float _mix_2_r = 0.0f;
    float _mix_3_r = 0.0f;
    float _res = 0.0f;

    inline float interpolate(int index, float depth, float phase)
    {
      while (phase >= _maxPhase) phase -= _maxPhase;
      float triangle = fabsf(phase - _halfMaxPhase) * _oneOverHalfMaxPhase;

      float mod_index = triangle * depth * (float)_delay_length;
      float mod_number;
      float mod_fraction = modff(mod_index, &mod_number);

      index -= (int)mod_number;
      if (index < 0) index += _delay_length;

      int next = index + 1;
      if (next >= _delay_length) next = 0;

      return _delayline[next] + (mod_fraction * (_delayline[index] - _delayline[next]));
    }

    float omega(float f)
    {
      return 1.0f - expf(-_twoPi * f / _sample_rate_Hz);
    }

    float mapRate(float min, float max, float x = 2.0f)
    {
      float freq = (powf(_rate, x) * (max - min)) + min;
      return freq * _maxPhase / _sample_rate_Hz;
    }

    float mapDepth(float min, float max, float x = 2.0f)
    {
      return (powf(_depth, x) * (max - min)) + min;
    }

    void apply()
    {
      switch (_ct)
      {
        case Default:
          _num_delay = 3;
          _phaseIncrement = mapRate(0.2f, 3.4f);
          _hpa = omega(150.0f);
          _lpa = omega(13000.0f);
          _dp1 = mapDepth(0.01f, 0.12f);
          _dp2 = mapDepth(0.01f, 0.20f);
          _dp3 = mapDepth(0.01f, 0.12f);
          _mix_dry = 0.8f;
          _mix_1_l = 0.8f;
          _mix_2_l = 0.6f;
          _mix_3_l = 0.0f;
          _mix_1_r = 0.0f;
          _mix_2_r = -0.6f;
          _mix_3_r = 0.8f;
          _res = 0.0f;
          break;

        case CE2:
          _num_delay = 1;
          _phaseIncrement = mapRate(0.3f, 3.5f);
          _hpa = omega(250.0f);
          _lpa = omega(6900.0f);
          _dp1 = mapDepth(0.025f, 0.55f);
          _dp2 = 0.0f;
          _dp3 = 0.0f;
          _mix_dry = 0.8f;
          _mix_1_l = 0.8f;
          _mix_2_l = 0.0f;
          _mix_3_l = 0.0f;
          _mix_1_r = -0.8f;
          _mix_2_r = 0.0f;
          _mix_3_r = 0.0f;
          _res = 0.0f;
          break;

        case TSC:
          _num_delay = 3;
          _phaseIncrement = mapRate(0.1f, 5.0f, 4.0f);
          _hpa = omega(180.0f);
          _lpa = omega(11000.0f);
          _dp1 = mapDepth(0.1f, 0.2f, 0.3f);
          _dp2 = mapDepth(0.1f, 0.2f, 0.3f);
          _dp3 = mapDepth(0.1f, 0.2f, 0.3f);
          _mix_dry = 0.8f;
          _mix_1_l = 0.8f;
          _mix_2_l = -0.6f;
          _mix_3_l = 0.0f;
          _mix_1_r = 0.0f;
          _mix_2_r = 0.6f;
          _mix_3_r = 0.8f;
          _res = 0.2f;
          break;

        default: // bypass
          _num_delay = 0;
          _phaseIncrement = 0.0f;
          _hpa = omega(150.0f);
          _lpa = omega(13000.0f);
          _dp1 = 0.0f;
          _dp2 = 0.0f;
          _dp3 = 0.0f;
          _mix_dry = 1.0f;
          _mix_1_l = 0.0f;
          _mix_2_l = 0.0f;
          _mix_3_l = 0.0f;
          _mix_1_r = 0.0f;
          _mix_2_r = 0.0f;
          _mix_3_r = 0.0f;
          _res = 0.0f;
          break;
      }
    }
};

#endif
