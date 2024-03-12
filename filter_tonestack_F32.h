/*
	ToneStack.h
	
	Copyright 2006-7
		David Yeh <dtyeh@ccrma.stanford.edu> 
	2006-14
		Tim Goetze <tim@quitte.de> (cosmetics)
	
	Tone Stack emulation for Teensy 4.x
	
	Ported for 32bit float version for OpenAudio_ArduinoLibrary:
	https://github.com/chipaudette/OpenAudio_ArduinoLibrary

	12.2023 Piotr Zapart www.hexefx.com
*/
/*
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA or point your web browser to http://www.gnu.org.
*/

#ifndef _FILTER_TONESTACK_F32_H_
#define _FILTER_TONESTACK_F32_H_

#include <Arduino.h>
#include <Audio.h>
#include "AudioStream_F32.h"
#include "filter_tdf2.h"
#include "arm_math.h"


template <int N>
class _AudioFilterTDF2
{
public:
  float32_t a[N + 1];
  float32_t b[N + 1];
  float32_t h[N + 1];

  void reset()
  {
    for (int i = 0; i <= N; ++i)
      h[i] = 0; // zero state
  }

  void init()
  {
    reset();
    clear();
  }

  void clear()
  {
    for (int i = 0; i <= N; i++)
      a[i] = b[i] = 0;
    b[0] = 1;
  }

  void process(float32_t *src, float32_t *dst, uint32_t blockSize)
  {
    for (uint16_t i = 0; i<blockSize; i++)
    {
      float32_t in = *src++;
      float32_t y = h[0] + b[0] * in;

      for (uint16_t j = 1; j < N; ++j)
        h[j - 1] = h[j] + b[j] * in- a[j] * y;

      h[N - 1] = b[N] * in - a[N] * y;
      *dst++ = y;
    }
  }
};


class AudioFilterToneStack_F32 : public AudioStream_F32
{
public:
	AudioFilterToneStack_F32();
	~AudioFilterToneStack_F32(){};
	virtual void update(void);

		/**
	 * @brief set all 3 parameters at once
	 * 
	 * @param b bass setting
	 * @param m middle setting
	 * @param t treble setting
	 */
	void setTone(float32_t b, float32_t m, float32_t t)
	{
		b = constrain(b, 0.0f, 1.0f); bass = b;
		m = constrain(m, 0.0f, 1.0f); mid = m;
		t = constrain(t, 0.0f, 1.0f); treble = t;
		struct
		{
			float32_t a1, a2, a3;
			float32_t b1, b2, b3;
		} acoef; // analog coefficients

		// digital coefficients
		float32_t dcoef_a[4];
		float32_t dcoef_b[4];

    if (b < 0.5f) b *= 0.2f;
    else b = b * 1.8f - 0.8f;
    
		acoef.a1 = a1d + m * a1m + b * a1l;
		acoef.a2 = m * a2m + b * m * a2lm + m * m * a2m2 + b * a2l + a2d;
		acoef.a3 = b * m * a3lm + m * m * a3m2 + m * a3m + b * a3l + a3d;
		dcoef_a[0] = -1.0f - acoef.a1 * c - acoef.a2 * c * c - acoef.a3 * c * c * c; // sets scale
		dcoef_a[1] = -3.0f - acoef.a1 * c + acoef.a2 * c * c + 3.0f * acoef.a3 * c * c * c;
		dcoef_a[2] = -3.0f + acoef.a1 * c + acoef.a2 * c * c - 3.0f * acoef.a3 * c * c * c;
		dcoef_a[3] = -1.0f + acoef.a1 * c - acoef.a2 * c * c + acoef.a3 * c * c * c;

		acoef.b1 = t * b1t + m * b1m + b * b1l + b1d;
		acoef.b2 = t * b2t + m * m * b2m2 + m * b2m + b * b2l + b * m * b2lm + b2d;
		acoef.b3 = b * m * b3lm + m * m * b3m2 + m * b3m + t * b3t + t * m * b3tm + t * b * b3tl;
		dcoef_b[0] = -acoef.b1 * c - acoef.b2 * c * c - acoef.b3 * c * c * c;
		dcoef_b[1] = -acoef.b1 * c + acoef.b2 * c * c + 3.0f * acoef.b3 * c * c * c;
		dcoef_b[2] = acoef.b1 * c + acoef.b2 * c * c - 3.0f * acoef.b3 * c * c * c;
		dcoef_b[3] = acoef.b1 * c - acoef.b2 * c * c + acoef.b3 * c * c * c;

		__disable_irq();
		for (int i = 1; i <= order; ++i)
		{
			filter.a[i] = dcoef_a[i] / dcoef_a[0];
		}
		for (int i = 0; i <= order; ++i)
		{
			filter.b[i] = dcoef_b[i] / dcoef_a[0];
		}
		 __enable_irq();
	}
	
	/**
	 * @brief Master volume setting
	 * 
	 * @param g gain value
	 */
	void setGain(float32_t g) {	gain = g;}

private:
	static const uint8_t order = 3;
	_AudioFilterTDF2<order> filter;
	audio_block_f32_t *inputQueueArray_f32[1];
	bool bp = false;		// bypass
	uint8_t currentModel;
	float32_t c = 2.0f * AUDIO_SAMPLE_RATE_EXACT;
	float32_t b1t, b1m, b1l, b1d,
		b2t, b2m2, b2m, b2l, b2lm, b2d,
		b3lm, b3m2, b3m, b3t, b3tm, b3tl,
		a0, a1d, a1m, a1l, a2m, a2lm, a2m2, a2l, a2d,
		a3lm, a3m2, a3m, a3l, a3d; // intermediate calculations
	float32_t bass, mid, treble, gain;
};

#endif // _FILTER_TONESTACK_F32_H_
