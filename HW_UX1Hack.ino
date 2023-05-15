#ifdef HW_UX1Hack

#include <WS2812Serial.h>


#define PIN_I2S_DIN     7
#define PIN_I2S_DOUT    8
#define PIN_AK4528_VT   9
#define PIN_WS2812_DATA 17
#define PIN_I2S_LRCLK   20
#define PIN_I2S_BCLK    21
#define PIN_AK4528_PDN  22
#define PIN_I2S_MCLK    23

/*
#define PIN_SPI_CS      10
#define PIN_SPI_MOSI    11
#define PIN_SPI_MISO    12
#define PIN_SPI_SCK     13
*/

int clip = 0;
float clipLevel = 0.944061f; // -0.5db
const int numled = 1;
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED
WS2812Serial leds(numled, displayMemory, drawingMemory, PIN_WS2812_DATA, WS2812_RGB);


void HW_Setup() {

  leds.begin();
  leds.setBrightness(200);

  pinMode(LED_BUILTIN, OUTPUT);
  
  CORE_PIN9_PADCONFIG  = IOMUXC_PAD_DSE(1);
  CORE_PIN11_PADCONFIG = IOMUXC_PAD_DSE(1);
  CORE_PIN21_PADCONFIG = IOMUXC_PAD_DSE(1);
  CORE_PIN23_PADCONFIG = IOMUXC_PAD_DSE(1);
  
  pinMode(PIN_AK4528_PDN, OUTPUT);
  digitalWriteFast(PIN_AK4528_PDN, LOW);
  delay(100);
  pinMode(PIN_AK4528_VT, OUTPUT);
  digitalWriteFast(PIN_AK4528_VT, HIGH);
  delay(500);
  digitalWriteFast(PIN_AK4528_PDN, HIGH);
  delay(100);

  inputMixer.gain(0, 0.0f);
  inputMixer.gain(1, 1.0f);
}

float lin = 0.0f;

void HW_Loop() {
  if (lin != lastLevelIn)
  {
    lin = lastLevelIn;

    if (lin > clipLevel) clip = 255;

    int blue = map(lin, 0.0f, 1.0f, 0.0f, 200.0f);
    int red = 0;
    int green = 0;
    
    if (clip > 0) {
      red = blue; //((clip & 16) > 0 ? clip : 0);
      blue = 0;      
      clip--;
    }

    leds.setPixel(0, leds.Color(green, red, blue));
    leds.show();
  }
}

#endif
