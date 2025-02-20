#ifndef _effect_stereo_delay_F32_h_
#define _effect_stereo_delay_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioEffectStereoDelay_F32 :
  public AudioStream_F32
{
    //GUI: inputs:2, outputs:2  //this line used for automatic generation of GUI node
    //GUI: shortName:delay
  public:
    AudioEffectStereoDelay_F32(void):
      AudioStream_F32(2, inputQueueArray)
    {
      _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    }

    AudioEffectStereoDelay_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(2, inputQueueArray),
      _lfo(settings.sample_rate_Hz),
      _svf(settings.sample_rate_Hz)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
    }

    bool begin(int16_t *delayline, int d_length)
    {
      _svf.mix(0.0f, 0.3f, 0.7f, 0.0f);
      _svf.resonance(1.0f);
      _svf.frequency(1000.0f);
      _samples = (float)d_length * 0.5f;
      _smoothed_samples = _samples;
      return _delay.begin(delayline, d_length);
    }

    void drive(float drive)
    {
      drive = drive < 0.0f ? 0.0f : drive > 1.0f ? 1.0f : drive;
      _drive = (powf(drive, 2.0f) * 20.0f) + 0.5f;
    }

    void time(float ms)
    {
      ms = ms < 10.0f ? 10.0f : ms;
      _samples = ms * _sample_rate_Hz * 0.001f;
    }

    void rate(float rate)
    {
      rate = rate < 0.0f ? 0.0f : rate > 1.0f ? 1.0f : rate;
      _lfo.freq(rate * 2.5f + 0.25f);
    }

    void depth(float depth)
    {
      depth = depth < 0.0f ? 0.0f : depth > 1.0f ? 1.0f : depth;
      _depth = 2.5f * depth * _sample_rate_Hz * 0.001f;
    }

    void spread(float spread)
    {
      spread = spread < 0.0f ? 0.0f : spread > 1.0f ? 1.0f : spread;
      _spread = 10.0f * spread * _sample_rate_Hz * 0.001f;
    }

    void repeat(float repeat)
    {
      _repeat = (repeat < 0.0f ? 0.0f : repeat > 1.0f ? 1.0f : repeat) * 1.0f;
    }

    void filter(float freq, float q, float dry, float low, float band, float high)
    {
      _svf.frequency(freq);
      _svf.resonance(q);
      _svf.mix(dry, low, band, high);
    }

    void wet(float wet)
    {
      _wet = wet < -1.0f ? -1.0f : wet > 1.0f ? 1.0f : wet;
    }

    void dry(float dry)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }

    virtual void update(void)
    {
      audio_block_f32_t* blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t* blockR = AudioStream_F32::receiveWritable_f32(1);
      if (!blockL || !blockR)
      {
        if (blockL) AudioStream_F32::release(blockL);
        if (blockR) AudioStream_F32::release(blockR);
        return;
      }

      float drive = _drive;
      float attn = 0.5f / drive;   
      for (int i = 0; i < blockL->length; i++)
      {
        _smoothed_samples += (_samples - _smoothed_samples) * 0.00075f;
        _smoothed_lfo += (_lfo.increment() - _smoothed_lfo) * 0.0001f;

        float sample = (blockL->data[i] + blockR->data[i] + _feedback) * drive;
        
        sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
        sample = (sample - ((sample * sample * sample) * (1.0f / 3.0f))) * (3.0f / 2.0f);
        sample *= attn;
        sample = _svf.filter(sample);

        _delay.write(sample);

        float modulation = _smoothed_lfo * _depth;

        float L = _delay.read(_smoothed_samples + modulation - _spread);
        float R = _delay.read(_smoothed_samples - modulation + _spread);

        _feedback = _delay.read(_smoothed_samples) * _repeat;

        if (_enable)
        {
          blockL->data[i] = (blockL->data[i] * _dry) + (L * _wet);
          blockR->data[i] = (blockR->data[i] * _dry) + (R * _wet);
        }
      }

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
    }

  private:
    typedef TriangleLfo Lfo;
    typedef LQDelay Delay;
    typedef StateVariableFilter<> Filter;

    audio_block_f32_t *inputQueueArray[2];
    float _sample_rate_Hz;

    Lfo _lfo;
    Filter _svf;
    Delay _delay;
    float _time = 1.0f;
    float _samples = 0.0f;
    float _maxrate = 0.25f;
    float _smoothed_samples = 0.0f;
    float _smoothed_lfo;
    float _drive = 0.5f;
    float _depth = 0.0f;
    float _spread = 0.0f;
    float _feedback = 0.0f;
    float _wet = 0.0f;
    float _dry = 1.0f;
    float _repeat = 0.0f;
    bool _enable = false;
};

#endif
