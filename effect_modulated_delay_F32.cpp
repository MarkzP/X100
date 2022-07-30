/*
   Copyright (c) 2014, Pete (El Supremo)
   Copyright (c) 2019-2021 H. Wirtz

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include "effect_modulated_delay_F32.h"

/******************************************************************/

// Based on;      A u d i o E f f e c t D e l a y
// Written by Pete (El Supremo) Jan 2014
// 140529 - change to handle mono stream - change modify() to voices()
// 140219 - correct storage class (not static)
// 190527 - added modulation input (by Holger Wirtz)
// ported by Marc Paquette to the Teensy OpenAudio F32 library
//    https://github.com/chipaudette/OpenAudio_ArduinoLibrary



boolean AudioEffectModulatedDelay_F32::begin(float *delayline, uint16_t d_length)
{
  _delayline = NULL;
  _delay_length = 0;
  _cb_index = 0;
  _delay_offset = 0;

  if (delayline == NULL)
    return (false);
  if (d_length < 10)
    return (false);

  _delayline = delayline;
  _delay_length = d_length;
  memset(_delayline, 0, _delay_length * sizeof(float));
  _delay_offset = _delay_length >> 1 ;

  return (true);
}

uint16_t AudioEffectModulatedDelay_F32::get_delay_length(void)
{
  return (_delay_length);
}

void AudioEffectModulatedDelay_F32::update(void)
{
  audio_block_f32_t *block;
  audio_block_f32_t *modulation;

  if (_delayline == NULL) return;

  block = AudioStream_F32::receiveWritable_f32(0);
  modulation = AudioStream_F32::receiveReadOnly_f32(1);

  if (block && modulation)
  {
    float *bp;
    int16_t cb_mod_index_neighbor;
    float *mp;
    float mod_index;
    float mod_number;
    float mod_fraction;

    bp = block->data;
    mp = modulation->data;

    for (uint16_t i = 0; i < block->length; i++)
    {
      // write data into circular buffer (delayline)
      if (_cb_index >= _delay_length) _cb_index = 0;
      _delayline[_cb_index] = *bp;

      // calculate the modulation-index as a floating point number for interpolation
      mod_index = *mp * _delay_offset;
      mod_fraction = modff(mod_index, &mod_number); // split float of mod_index into integer (= mod_number) and fraction part

      // calculate modulation index into circular buffer
      cb_mod_index = _cb_index - (_delay_offset + mod_number);
      if (cb_mod_index < 0) // check for negative offsets and correct them
        cb_mod_index += _delay_length;

      if (cb_mod_index == _delay_length - 1)
        cb_mod_index_neighbor = 0;
      else
        cb_mod_index_neighbor = cb_mod_index + 1;

      *bp = _delayline[cb_mod_index] * mod_fraction + _delayline[cb_mod_index_neighbor] * (1.0f - mod_fraction);

      // push the pointers forward
      bp++; // next audio data
      mp++; // next modulation data
      _cb_index++; // next circular buffer index
    }
  }

  if (modulation) AudioStream_F32::release(modulation);

  if (block)
  {
    AudioStream_F32::transmit(block, 0);
    AudioStream_F32::release(block);
  }
}
