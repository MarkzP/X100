#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code

AudioSynthWaveform_F32   waveform1;      //xy=97,113
AudioInputI2S_F32        audioInI2S1;    //xy=97,168
AudioOutputI2S_F32       audioOutI2S1;   //xy=365,115
AudioConnection_F32          patchCord1(waveform1, 0, audioOutI2S1, 0);
AudioConnection_F32          patchCord2(audioInI2S1, 0, audioOutI2S1, 1);
// GUItool: end automatically generated code

#include "MPAudioControlWM8731.h"
MPAudioControlWM8731 codec;

void setup() {
  delay(1000); // allow the WM7831 extra time to power up
  codec.enable();

  AudioMemory(15);
  AudioMemory_F32(15);

  waveform1.begin(WAVEFORM_SINE);
  waveform1.frequency(12500);
  waveform1.amplitude(0.99f);
}

void loop() {
  // put your main code here, to run repeatedly:

}
