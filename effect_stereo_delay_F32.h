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
    }

    AudioEffectStereoDelay_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(2, inputQueueArray),
      _sample_rate_Hz(settings.sample_rate_Hz),
      _lfo(settings.sample_rate_Hz),
      _hpL(settings.sample_rate_Hz),
      _hpR(settings.sample_rate_Hz),
      _lpL(settings.sample_rate_Hz),
      _lpR(settings.sample_rate_Hz)
    {
    }

    FLASHMEM void begin()
    {
      _samples = _max_delay * 0.5f;
      _smoothed_samples = _samples;
    }

    FLASHMEM void time(float ms)
    {
      ms = ms < 10.0f ? 10.0f : ms;
      _samples = DelayBase::msToSamples(ms);
    }

    FLASHMEM void rate(float rate = 0.5f)
    {
      rate = rate < 0.0f ? 0.0f : rate > 1.0f ? 1.0f : rate;
      _lfo.freq(rate * 1.8f + 0.2f);
    }

    FLASHMEM void depth(float depth = 0.5f)
    {
      depth = depth < 0.0f ? 0.0f : depth > 1.0f ? 1.0f : depth;
      _depth = DelayBase::msToSamples(depth * 0.75f);
    }

    FLASHMEM void spread(float spread = 0.5f)
    {
      spread = spread < 0.0f ? 0.0f : spread > 1.0f ? 1.0f : spread;
      _spread = DelayBase::msToSamples(spread * 15.0f);
    }

    FLASHMEM void repeat(float repeat)
    {
      _repeat = (repeat < 0.0f ? 0.0f : repeat > 1.0f ? 1.0f : repeat) * 0.999f;
    }

    FLASHMEM void wet(float wet = 0.2f)
    {
      _wet = wet < 0.0f ? 0.0f : wet > 1.0f ? 1.0f : wet;
    }

    FLASHMEM void dry(float dry = 1.0f)
    {
      _dry = dry < 0.0f ? 0.0f : dry > 1.0f ? 1.0f : dry;
    }

    FLASHMEM void highpass(float freq = 0.0f)
    {
      _hpL.setHighpassFirstOrder(freq);
      _hpR.setHighpassFirstOrder(freq);
    }

    FLASHMEM void lowpass(float freq = 15000.0f, float q = 0.7f)
    {
      _lpL.setLowpass(freq, q);
      _lpR.setLowpass(freq, q);
    }

    FLASHMEM void enable(bool enable = false)
    {
      _enable = enable;
    }

    virtual void update(void)
    {
      if (!_enable)
      {
        audio_block_f32_t *bpL = AudioStream_F32::receiveReadOnly_f32(0);
        audio_block_f32_t *bpR = AudioStream_F32::receiveReadOnly_f32(1);
        if (!bpL || !bpR)
        {
          if (bpL) AudioStream_F32::release(bpL);
          if (bpR) AudioStream_F32::release(bpR);
          return;
        }
        AudioStream_F32::transmit(bpL, 0);
        AudioStream_F32::transmit(bpR, 1);
        AudioStream_F32::release(bpR);
        AudioStream_F32::release(bpL);
        return;
      }

      audio_block_f32_t *blockL = AudioStream_F32::receiveWritable_f32(0);
      audio_block_f32_t *blockR = AudioStream_F32::receiveWritable_f32(1);
      if (!blockL || !blockR)
      {
        if (blockL) AudioStream_F32::release(blockL);
        if (blockR) AudioStream_F32::release(blockR);
        return;
      }

      float samples = _samples;
      float smoothed_samples = _smoothed_samples;
      float depth = _depth;
      float spread = _spread;
      float repeat = _repeat;
      float input = _enable ? 1.0f : 0.0f;
      float dry = _enable ? _dry : 1.0f;
      float wet = _wet;
      float feedbackL = _feedbackL;
      float feedbackR = _feedbackR;

      float modulation, dryL, dryR, sampleL, sampleR;

      float *pl = blockL->data;
      float *pr = blockR->data;
      float *endl = pl + blockL->length;
      do
      {
        smoothed_samples += (samples - smoothed_samples) * 0.00075f;
        modulation = _lfo.next() * depth;

        dryL = *pl;
        sampleL = dryL * input;
        sampleL += feedbackL;
        sampleL = _hpL.filter(sampleL);
        sampleL = _lpL.filter(sampleL);
        _delayL.write(sampleL);        
        sampleL = _delayL.read(smoothed_samples + modulation - spread);
        *pl++ = (dryL * dry) + (sampleL * wet);

        dryR = *pr;
        sampleR = dryR * input;
        sampleR += feedbackR;
        sampleR = _hpR.filter(sampleR);
        sampleR = _lpR.filter(sampleR);
        _delayR.write(sampleR);
        sampleR = _delayR.read(smoothed_samples - modulation + spread);
        *pr++ = (dryR * dry) + (sampleR * wet);

        feedbackL = sampleL * repeat;
        feedbackR = sampleR * repeat;
      }
      while (pl < endl);

      _smoothed_samples = smoothed_samples;

      _feedbackL = feedbackL;
      _feedbackR = feedbackR;

      AudioStream_F32::transmit(blockL, 0);
      AudioStream_F32::transmit(blockR, 1);

      AudioStream_F32::release(blockR);
      AudioStream_F32::release(blockL);
    }

    typedef SineLfo Lfo;
    typedef StateVariableFilter<2> Filter;

  private:
    audio_block_f32_t *inputQueueArray[2];
    const float _sample_rate_Hz = AUDIO_SAMPLE_RATE_EXACT;
    static constexpr float _max_delay = 1000.0f;
    Lfo _lfo;

    HQFirstOrder _hpL;
    HQFirstOrder _hpR;
    Biquad _lpL;
    Biquad _lpR;

    EXTMEMDelay< DelayBase::bufferSizeMs(_max_delay) > _delayL;
    EXTMEMDelay< DelayBase::bufferSizeMs(_max_delay) > _delayR;
    float _samples = 0.0f;
    float _maxrate = 0.25f;
    float _smoothed_samples = 0.0f;

    float _depth = 0.0f;
    float _spread = 0.0f;
    float _feedbackL = 0.0f;
    float _feedbackR = 0.0f;
    float _repeat = 0.0f;
    float _wet = 1.0f;
    float _dry = 0.3f;
    bool _enable = false;
};

#endif
