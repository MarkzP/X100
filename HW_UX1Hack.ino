#ifdef UX1

#include <SPI.h>
#include "control_AK4528_F32.h"

//#define PIN_WS2812_DATA 6

#define PIN_BTN_0       3
#define PIN_BTN_1       2
#define PIN_BTN_2       1
#define PIN_BTN_3       0
//#define PIN_BTN_4       6

#define PIN_POT_0      14
#define PIN_POT_1      15
#define PIN_POT_2      16
//#define PIN_POT_3      17
//#define PIN_POT_4      18
//#define PIN_POT_5      19

#define PIN_ENC_A      5
#define PIN_ENC_B      4
#define PIN_ENC_C      12

#define PIN_SPI_CS      10
/*
  #define PIN_SPI_MOSI    11
  #define PIN_SPI_MISO    12
  #define PIN_SPI_SCK     13
*/

AudioControlAK4528_F32 codec;

/*
  P G F E D C B A
  A
  --
  F|__|B
  E|G |C
  --   .P
  D
  1000 1000
*/

const byte led_chars[] = {
  //  0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // 0
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // 1
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xbf, 0x7f, 0xff, // 2
  0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xA0, 0x83, 0xa7, 0xa1, 0x86, 0x8e, // 3
  0x80, 0x88, 0x83, 0xa7, 0xa1, 0x86, 0x8e, 0xc2, 0x8b, 0xef, 0xe1, 0x89, 0xc7, 0xaa, 0xab, 0xa3, // 4
  0x8c, 0x98, 0xaf, 0x9b, 0x87, 0xe3, 0xc1, 0xd5, 0xb6, 0x91, 0xad, 0xc6, 0xff, 0xf0, 0xff, 0xf7, // 5
  0x80, 0xa0, 0x83, 0xa7, 0xa1, 0x86, 0x8e, 0xc2, 0x8b, 0xef, 0xe1, 0x89, 0xc7, 0xaa, 0xab, 0xa3, // 6
  0x8c, 0x98, 0xaf, 0x9b, 0x87, 0xe3, 0xc1, 0xd5, 0xb6, 0x91, 0xad, 0xff, 0xff, 0xff, 0xff, 0xff, // 7
};



FLASHMEM void print(const char *s)
{
  byte buf[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

  int i = 0;
  for (int p = 0; i < 8 && s[p] != 0; i++)
  {
    buf[i] = led_chars[s[p++] & 0x7f];
    if (s[p] == '.' || s[p] == '_') {
      buf[i] &= 0x7f;
      p++;
    }
  }

  digitalWriteFast(PIN_SPI_CS, LOW);
  SPI.transfer32(*((uint32_t*)&buf[4]));
  SPI.transfer32(*((uint32_t*)&buf[0]));
  digitalWriteFast(PIN_SPI_CS, HIGH);
}

FLASHMEM void displayTunerHW(float freq, int note, int semitone, float cents)
{
  char buf[] = "        ";

  int i = 1;
  switch (note)
  {
    case 0:  buf[i] = 'A'; break;
    case 1:  buf[i] = 'A'; buf[i + 1] = '#'; break;
    case 2:  buf[i] = 'B'; break;
    case 3:  buf[i] = 'C'; break;
    case 4:  buf[i] = 'C'; buf[i + 1] = '#'; break;
    case 5:  buf[i] = 'D'; break;
    case 6:  buf[i] = 'D'; buf[i + 1] = '#'; break;
    case 7:  buf[i] = 'E'; break;
    case 8:  buf[i] = 'F'; break;
    case 9:  buf[i] = 'F'; buf[i + 1] = '#'; break;
    case 10: buf[i] = 'G'; break;
    case 11: buf[i] = 'G'; buf[i + 1] = '#'; break;
  }
  
  int qc = round(cents * 0.25f);
  switch (qc)
  {
    case -7:
    case -6:
    case -5:
    case -4:  buf[0] = '['; break;
    case -3:  buf[1] = ']'; break;
    case -2:  buf[2] = ']'; break;
    case -1:  buf[3] = ']'; break;
    case 0:   buf[0] = '-'; buf[3] = ']'; buf[4] = '['; buf[7] = '-'; break;
    case 1:   buf[4] = '['; break;
    case 2:   buf[5] = '['; break;
    case 3:   buf[6] = '['; break;
    case 4:   buf[7] = '['; break;
    case 5:
    case 6:
    case 7:   buf[7] = ']'; break;
  }
  
  print(buf);
}

FLASHMEM void HW_Setup()
{
  codec.enable();

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  pinMode(PIN_SPI_CS, OUTPUT);
  digitalWriteFast(PIN_SPI_CS, HIGH);

  print("        ");
}

void HW_Loop()
{
}

#endif
