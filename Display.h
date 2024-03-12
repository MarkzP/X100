#ifndef _display_h
#define _display_h

#include <SPI.h>

#define PIN_SPI_CS      10
/*
#define PIN_SPI_MOSI    11
#define PIN_SPI_MISO    12
#define PIN_SPI_SCK     13
*/

/*
P G F E D C B A
  A
  --
F|__|B
E|G |C
  --   .P
  D
1100 0001
*/

const byte led_chars[]={
//  0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // 0
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // 1
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x7f, 0xff, // 2
  0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xA0, 0x83, 0xa7, 0xa1, 0x86, 0x8e, // 3
  0x80, 0xa0, 0x83, 0xa7, 0xa1, 0x86, 0x8e, 0xc2, 0x8b, 0xef, 0xe1, 0x89, 0xc7, 0xaa, 0xab, 0xa3, // 4
  0x8c, 0x98, 0xaf, 0x9b, 0x87, 0xe3, 0xc1, 0xd5, 0xb6, 0x91, 0xad, 0xc6, 0xff, 0xf0, 0xff, 0xf7, // 5
  0x80, 0xa0, 0x83, 0xa7, 0xa1, 0x86, 0x8e, 0xc2, 0x8b, 0xef, 0xe1, 0x89, 0xc7, 0xaa, 0xab, 0xa3, // 6
  0x8c, 0x98, 0xaf, 0x9b, 0x87, 0xe3, 0xc1, 0xd5, 0xb6, 0x91, 0xad, 0xff, 0xff, 0xff, 0xff, 0xff, // 7
};

void print(const char *s)
{
  byte buf[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  
  for (int i = 0, p = 0; i < 8 && s[p] != 0; i++)
  {
    buf[i] = led_chars[s[p++] & 0x7f];
    if (s[p] == '.' || s[p] == '_') { buf[i] &= 0x7f; p++; }
  }

  digitalWriteFast(PIN_SPI_CS, LOW);
  SPI.transfer32(*((uint32_t*)&buf[4]));
  SPI.transfer32(*((uint32_t*)&buf[0]));
  digitalWriteFast(PIN_SPI_CS, HIGH);
}

void Display_Setup()
{
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setDataMode(SPI_MODE0);
  pinMode(PIN_SPI_CS, OUTPUT);
  digitalWriteFast(PIN_SPI_CS, HIGH);
}

#endif
