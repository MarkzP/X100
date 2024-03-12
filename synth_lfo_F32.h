#ifndef synth_lfo_F32_h
#define synth_lfo_F32_h

#include <arm_math.h>
#include <AudioStream_F32.h>

class AudioSynthLfo_F32 : public AudioStream_F32
{
  //GUI: inputs:0, outputs:1  //this line used for automatic generation of GUI node
  public:
    AudioSynthLfo_F32(const AudioSettings_F32 &settings) : AudioStream_F32(1, inputQueueArray)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
    }
    
    AudioSynthLfo_F32(void) : AudioStream_F32(1, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    }

    void frequency(float freq)
    {
        float nyquist = _sample_rate_Hz/2.f;

        if (freq < 0.0) freq = 0.0;
        else if (freq > nyquist) freq = nyquist;

        _frequency = freq;

        _phaseIncrement = _frequency * _twoPi / _sample_rate_Hz;
    }

    void amplitude(float n)
    {
        if (n < 0) n = 0;
        _magnitude = n;
    }
    
    void phase(float p)
    {
      _phase = _twoPi * p / 360.0f;
      while (_phase >= _twoPi) _phase -= _twoPi;
    }

    void begin(short t_type)
    {
        _phase = 0;
        _waveform_type = t_type;
    }

    void begin(float t_amp, float t_freq, short t_type)
    {
        amplitude(t_amp);
        frequency(t_freq);
        begin(t_type);
    }

    
    void update(void)
    {
      if (_magnitude == 0.0f) return;
    
      audio_block_f32_t *block = allocate_f32();
      if (!block) return;
    
      switch (_waveform_type)
      {
        case WAVEFORM_SINE:
            for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
            {
              block->data[i] = arm_sin_f32(_phase);
              
              _phase += _phaseIncrement;
              while (_phase >= _twoPi) _phase -= _twoPi;
            }
            break;
        case WAVEFORM_SAWTOOTH:
            for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
            {
              block->data[i] = 1.0f - (_phase * _oneOverPi);
              
              _phase += _phaseIncrement;
              while (_phase >= _twoPi) _phase -= _twoPi;
            }
            break;
        case WAVEFORM_SQUARE:
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
          {
            if (_phase <= _pi) block->data[i] = 1.0f;
            else block->data[i] = -1.0f;
    
            _phase += _phaseIncrement;
            while (_phase >= _twoPi) _phase -= _twoPi;
          }
          break;
        case WAVEFORM_TRIANGLE:
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
          {
            float value = -1.0f + (_phase * _oneOverPi);
            block->data[i] = 2.0f * (fabsf(value) - 0.5f);
            
            _phase += _phaseIncrement;
            while (_phase >= _twoPi) _phase -= _twoPi;
          }
          break;
        case WAVEFORM_SAWTOOTH_REVERSE:
          for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
          {
            block->data[i] = (_phase * _oneOverPi) - 1.0f;
            
            _phase += _phaseIncrement;
            while (_phase >= _twoPi) _phase -= _twoPi;
          }
          break;            
        default:
          break;
      }      
    
      if (_magnitude != 1.0f) arm_scale_f32(block->data, _magnitude, block->data, AUDIO_BLOCK_SAMPLES);
    
      AudioStream_F32::transmit(block);
      AudioStream_F32::release(block);      
    }
    
  private:
    audio_block_f32_t *inputQueueArray[1];
    short _waveform_type;
    static constexpr float _pi = (float)PI;
    static constexpr float _twoPi = 2.0f * _pi;
    static constexpr float _oneOverPi = 1.0f / _pi;
    float _sample_rate_Hz;
    float _frequency;
    float _phase;
    float _phaseIncrement;
    float _magnitude;
};

#endif
