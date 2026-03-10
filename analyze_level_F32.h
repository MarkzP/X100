#ifndef _analyze_level_F32_h_
#define _analyze_level_F32_h_

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

#include "components_F32.h"

class AudioAnalyzeLevel_F32 :
  public AudioStream_F32
{
    //GUI: inputs:1, outputs:0  //this line used for automatic generation of GUI node
    //GUI: shortName:preamp
  public:
    AudioAnalyzeLevel_F32(void):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    AudioAnalyzeLevel_F32(const AudioSettings_F32 &settings):
      AudioStream_F32(1, inputQueueArray)
    {
    }

    void begin()
    {
    }

    bool available()
    {
      return _count > 0;
    }

    uint32_t getCount()
    {
      return _count;
    }

    float read()
    {
      __disable_irq();
      float level = _max - _min;
      _max = 0.0f;
      _min = 0.0f;
      _count = 0.0f;
      __enable_irq();

      return level * 0.5f;
    }

    virtual void update(void)
    {
      audio_block_f32_t *block = AudioStream_F32::receiveReadOnly_f32(0);
      if (!block) return;

      float min = _min;
      float max = _max;

      float *p = block->data;
      float *end = p + block->length;
      do
      {
        float sample = *p++;
        if (sample > max) max = sample;
        if (sample < min) min = sample;
      }
      while (p < end);

      _count++;
      _min = min;
      _max = max;

      AudioStream_F32::release(block);
    }

  private:
    audio_block_f32_t *inputQueueArray[1];
    uint32_t _count = 0;
    float _min = 0.0f;
    float _max = 0.0f;

};

#endif
