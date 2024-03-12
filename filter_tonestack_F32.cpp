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
#include "filter_tonestack_F32.h"


AudioFilterToneStack_F32 :: AudioFilterToneStack_F32() : AudioStream_F32(1, inputQueueArray_f32)
{
	gain = 1.0f;
	bp = false;

#define k *1e3
#define M *1e6
#define nF *1e-9
#define pF *1e-12

/*
  float32_t R1 = 220 k;
  float32_t R2 = 1 M;
  float32_t R3 = 25 k;
  float32_t R4 = 33 k;
  float32_t C1 = 470 pF;
  float32_t C2 = 22 nF;
  float32_t C3 = 22 nF;
*/

  float32_t R1 = 200 k;   // RT
  float32_t R2 = 820 k;   // RB
  float32_t R3 = 22 k;    // RM
  float32_t R4 = 27 k;    // R1
  float32_t C1 = 400 pF;  // 
  float32_t C2 = 18 nF;
  float32_t C3 = 22 nF;
  
#undef k
#undef M
#undef nF
#undef pF

  b1t = C1 * R1;
  b1m = C3 * R3;
  b1l = C1 * R2 + C2 * R2;
  b1d = C1 * R3 + C2 * R3;
  b2t = C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4;
  b2m2 = -(C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3);
  b2m = C1 * C3 * R1 * R3 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3;
  b2l = C1 * C2 * R1 * R2 + C1 * C2 * R2 * R4 + C1 * C3 * R2 * R4;
  b2lm = C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3;
  b2d = C1 * C2 * R1 * R3 + C1 * C2 * R3 * R4 + C1 * C3 * R3 * R4;
  b3lm = C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4;
  b3m2 = -(C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4);
  b3m = C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4;
  b3t = C1 * C2 * C3 * R1 * R3 * R4;
  b3tm = -b3t;
  b3tl = C1 * C2 * C3 * R1 * R2 * R4;
  a0 = 1.0f;
  a1d = C1 * R1 + C1 * R3 + C2 * R3 + C2 * R4 + C3 * R4;
  a1m = C3 * R3;
  a1l = C1 * R2 + C2 * R2;
  a2m = C1 * C3 * R1 * R3 - C2 * C3 * R3 * R4 + C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3;
  a2lm = C1 * C3 * R2 * R3 + C2 * C3 * R2 * R3;
  a2m2 = -(C1 * C3 * R3 * R3 + C2 * C3 * R3 * R3);
  a2l = C1 * C2 * R2 * R4 + C1 * C2 * R1 * R2 + C1 * C3 * R2 * R4 + C2 * C3 * R2 * R4;
  a2d = C1 * C2 * R1 * R4 + C1 * C3 * R1 * R4 + C1 * C2 * R3 * R4 + C1 * C2 * R1 * R3 + C1 * C3 * R3 * R4 + C2 * C3 * R3 * R4;
  a3lm = C1 * C2 * C3 * R1 * R2 * R3 + C1 * C2 * C3 * R2 * R3 * R4;
  a3m2 = -(C1 * C2 * C3 * R1 * R3 * R3 + C1 * C2 * C3 * R3 * R3 * R4);
  a3m = C1 * C2 * C3 * R3 * R3 * R4 + C1 * C2 * C3 * R1 * R3 * R3 - C1 * C2 * C3 * R1 * R3 * R4;
  a3l = C1 * C2 * C3 * R1 * R2 * R4;
  a3d = C1 * C2 * C3 * R1 * R3 * R4;

  filter.reset();
}

void AudioFilterToneStack_F32::update()
{
#if defined(__ARM_ARCH_7EM__)

	audio_block_f32_t* block = AudioStream_F32::receiveWritable_f32(0);

  if (!block) return;
  
  if (!bp)
  {
    filter.process(block->data, block->data, block->length);
    if (gain != 1.0f)
    {
      arm_scale_f32(block->data, gain, block->data, block->length);
    }    
  }

  AudioStream_F32::transmit(block, 0);
  AudioStream_F32::release(block);
#endif
}
