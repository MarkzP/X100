#ifdef TGA

#include "BAAudioControlWM8731.h"
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

#define PIN_ENC_A      15
#define PIN_ENC_B      16
#define PIN_ENC_C      14

#define PIN_ENC_VOL_A      3
#define PIN_ENC_VOL_B      4
#define PIN_ENC_VOL_C      14

Encoder volKnob(PIN_ENC_VOL_A, PIN_ENC_VOL_B);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
BALibrary::BAAudioControlWM8731 codec;

long volumePos = 0;

FLASHMEM void clear()
{
  lcd.clear();
}

FLASHMEM void print(const char *s, int line = 0)
{
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
  lcd.print(s);
}

FLASHMEM void displayTunerHW(float freq, int note, int semitone, float cents)
{
  int c = round(cents * 0.25f);
  if (c < -4)       print("[       ");
  else if (c == -4) print("]       ");
  else if (c == -3) print(" ]      ");
  else if (c == -2) print("  ]     ");
  else if (c == -1) print("   ]    ");
  else if (c == 0)  print("-  ][  -");
  else if (c == 1)  print("    [   ");
  else if (c == 2)  print("     [  ");
  else if (c == 3)  print("      [ ");
  else if (c == 4)  print("       [");
  else if (c > 4)   print("       ]");
}

FLASHMEM void HW_Setup()
{
  pinMode(PIN_ENC_VOL_A, INPUT_PULLUP);
  pinMode(PIN_ENC_VOL_B, INPUT_PULLUP);
  pinMode(PIN_ENC_VOL_C, INPUT_PULLUP);

  volKnob.write(80 * 4);
  
  lcd.init();
  lcd.backlight();
  print("Codec init...");

  delay(500);
  codec.enable();
  delay(10);
  codec.setRightInputGain(0);
  delay(10);
  codec.setLeftInputGain(0);
  delay(10);
  print("Calibrating...", 1);
  delay(10);
  codec.recalibrateDcOffset();
  delay(10);
  codec.setRightInMute(true);  
  delay(10);
  codec.setRightInputGain(0);
  delay(10);
  codec.setLeftInputGain(26);
  delay(10);
  lcd.clear();
}

FLASHMEM void HW_Loop()
{
  long newVolumePos = (volKnob.read() + 2) / 4;
  if (newVolumePos != volumePos)
  {
    codec.setHeadphoneVolume((float)newVolumePos / 100.0f);
    volumePos = newVolumePos;
  }
}

#endif
