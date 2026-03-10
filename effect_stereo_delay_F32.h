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
      _filterL(settings.sample_rate_Hz),
      _filterR(settings.sample_rate_Hz)
    {
      _sample_rate_Hz = settings.sample_rate_Hz;
    }

    bool begin(float *delayL, float *delayR, int d_length)
    {
      _filterL.mix(0.0f, 0.3f, 0.7f, 0.0f);
      _filterL.resonance(1.0f);
      _filterL.frequency(1000.0f);
      _filterR.mix(0.0f, 0.3f, 0.7f, 0.0f);
      _filterR.resonance(1.0f);
      _filterR.frequency(1000.0f);

      _samples = (float)d_length * 0.5f;
      _smoothed_samples = _samples;
      return _delayL.begin(delayL, d_length) && _delayR.begin(delayR, d_length);
    }

    void drive(float drive)
    {
      drive = drive < 0.0f ? 0.0f : drive > 1.0f ? 1.0f : drive;
      _drive = (powf(drive, 2.0f) * 10.0f) + 1.0f;
    }

    void time(float ms)
    {
      ms = ms < 10.0f ? 10.0f : ms;
      _samples = ms * _sample_rate_Hz * 0.001f;
    }

    void rate(float rate)
    {
      rate = rate < 0.0f ? 0.0f : rate > 1.0f ? 1.0f : rate;
      _lfo.freq(rate * 2.25f + 0.25f);
    }

    void depth(float depth)
    {
      depth = depth < 0.0f ? 0.0f : depth > 1.0f ? 1.0f : depth;
      _depth = 0.5f * depth * _sample_rate_Hz * 0.001f;
    }

    void spread(float spread)
    {
      spread = spread < 0.0f ? 0.0f : spread > 1.0f ? 1.0f : spread;
      _spread = 10.0f * spread * _sample_rate_Hz * 0.001f;
    }

    void repeat(float repeat)
    {
      _repeat = (repeat < 0.0f ? 0.0f : repeat > 1.0f ? 1.0f : repeat) * 0.999f;
    }

    void wet(float wet = 0.2f)
    {
      _wet = wet < -1.0f ? -1.0f : wet > 1.0f ? 1.0f : wet;
    }

    void dry(float dry = 1.0f)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    void enable(bool enable = false)
    {
      _enable = enable;
    }

    void filter(float freq, float q, float dry, float low, float band, float high)
    {
      _filterL.frequency(freq);
      _filterL.resonance(q);
      _filterL.mix(dry, low, band, high);
      _filterR.frequency(freq);
      _filterR.resonance(q);
      _filterR.mix(dry, low, band, high);
    }

    virtual void update(void)
    {
      audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t *blockR = AudioStream_F32::receiveWritable_f32(1);
      if (!blockL || !blockR)
      {
        if (blockL) AudioStream_F32::release(blockL);
        if (blockR) AudioStream_F32::release(blockR);
        return;
      }

      float drive = _drive;
      float attn = 0.5f / drive;
      float repeat = _repeat / drive;
      float input = _enable ? 1.0f : 0.0f;
      float dry = _enable ? _dry : 1.0f;
      float wet = _wet;
      float feedbackL = _feedbackL;
      float feedbackR = _feedbackR;
      float *pl = blockL->data;
      float *pr = blockR->data;
      float *endl = pl + blockL->length;
      do
      {
        _smoothed_samples += (_samples - _smoothed_samples) * 0.00075f;
        float modulation = _lfo.next() * _depth;

        float dryL = *pl;
        float dryR = *pr;

        float sampleL = dryL * input;
        float sampleR = dryR * input;
        
        sampleL = nonLinear(sampleL * drive);
        sampleL = _filterL.filter(sampleL);

        sampleR = nonLinear(sampleR * drive);
        sampleR = _filterR.filter(sampleR);

        _delayL.write(sampleL + feedbackL);        
        _delayR.write(sampleR + feedbackR);

        float yL = _delayL.read(_smoothed_samples + modulation - _spread) * attn;
        float yR = _delayR.read(_smoothed_samples - modulation + _spread) * attn;

        feedbackL = _delayL.read(_smoothed_samples) * repeat;
        feedbackR = _delayR.read(_smoothed_samples) * repeat;

        *pl++ = (dryL * dry) + (yL * wet);
        *pr++ = (dryR * dry) + (yR * wet);
      }
      while (pl < endl);

      _feedbackL = FilterUtils::denormFloat(feedbackL);
      _feedbackR = FilterUtils::denormFloat(feedbackR);

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
    }

    typedef SineLfo Lfo;
    typedef StateVariableFilter<4> Filter;

  private:
    audio_block_f32_t *inputQueueArray[2];
    float _sample_rate_Hz;

    Lfo _lfo;
    Filter _filterL;
    Filter _filterR;
    Delay _delayL;
    Delay _delayR;
    float _time = 1.0f;
    float _samples = 0.0f;
    float _maxrate = 0.25f;
    float _smoothed_samples = 0.0f;
    float _drive = 0.5f;
    float _depth = 0.0f;
    float _spread = 0.0f;
    float _feedbackL = 0.0f;
    float _feedbackR = 0.0f;
    float _repeat = 0.0f;
    float _wet = 1.0f;
    float _dry = 0.3f;
    bool _enable = false;

    inline float nonLinear(float sample)
    {
        sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
        double ds = (double)sample;
        float s3 = (float)(ds * ds * ds);
        return (sample - (s3 * (1.0f / 3.0f))) * (3.0f / 2.0f);      
    }
};

#endif
