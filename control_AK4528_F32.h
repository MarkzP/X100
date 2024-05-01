#ifndef _CONTROL_AK4528_H_
#define _CONTROL_AK4528_H_

#include <Arduino.h>

//#define PIN_I2S_DIN     7
//#define PIN_I2S_DOUT    8
//#define PIN_I2S_LRCLK   20
//#define PIN_I2S_BCLK    21
//#define PIN_I2S_MCLK    23

#define PIN_AK4528_VT   9
#define PIN_AK4528_PDN  22

class AudioControlAK4528_F32 //: public AudioControl
{
public:
  AudioControlAK4528_F32(void){};
  ~AudioControlAK4528_F32(void){};
  
  bool enable()
  {
    CORE_PIN9_PADCONFIG  = IOMUXC_PAD_DSE(1);
    CORE_PIN11_PADCONFIG = IOMUXC_PAD_DSE(1);
    CORE_PIN21_PADCONFIG = IOMUXC_PAD_DSE(1);
    CORE_PIN23_PADCONFIG = IOMUXC_PAD_DSE(1);
    
    pinMode(PIN_AK4528_PDN, OUTPUT);
    pinMode(PIN_AK4528_VT, OUTPUT);
    
    digitalWriteFast(PIN_AK4528_PDN, LOW);
    digitalWriteFast(PIN_AK4528_VT, LOW);
    delay(50);    
    digitalWriteFast(PIN_AK4528_VT, HIGH);
    delay(200);
    digitalWriteFast(PIN_AK4528_PDN, HIGH);

    return true;
  }
  
  bool disable(void) { return false; }
  bool volume(float n) { return true; }
  bool inputSelect(int n) { return true; }
  bool inputLevel(float n) { return true; }
  bool lineInLevel(uint8_t n) { return lineInLevel(n, n); }
  bool lineInLevel(uint8_t left, uint8_t right) { return true; }
  uint16_t lineOutLevel(uint8_t n) { return 0; }
  uint16_t lineOutLevel(uint8_t left, uint8_t right) { return 0; }
  uint16_t adcHighPassFilterEnable(void) { return 0; }
  uint16_t adcHighPassFilterFreeze(void) { return 0; }
  uint16_t adcHighPassFilterDisable(void) { return 0; }
};
#endif
