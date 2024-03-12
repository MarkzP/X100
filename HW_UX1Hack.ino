#include <WS2812Serial.h>



#define PIN_I2S_DIN     7
#define PIN_I2S_DOUT    8
#define PIN_AK4528_VT   9
//#define PIN_WS2812_DATA 6
#define PIN_I2S_LRCLK   20
#define PIN_I2S_BCLK    21
#define PIN_AK4528_PDN  22
#define PIN_I2S_MCLK    23


/*
int clip = 0;
float clipLevel = 0.999999f; // -0.01db
const int numled = 1;
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED
WS2812Serial leds(numled, displayMemory, drawingMemory, PIN_WS2812_DATA, WS2812_RGB);
*/
void HW_Setup() {

  //leds.begin();
  //leds.setBrightness(200);

  //pinMode(LED_BUILTIN, OUTPUT);

  CORE_PIN9_PADCONFIG  = IOMUXC_PAD_DSE(1);
  CORE_PIN11_PADCONFIG = IOMUXC_PAD_DSE(1);
  CORE_PIN21_PADCONFIG = IOMUXC_PAD_DSE(1);
  CORE_PIN23_PADCONFIG = IOMUXC_PAD_DSE(1);
  
  pinMode(PIN_AK4528_PDN, OUTPUT);
  digitalWriteFast(PIN_AK4528_PDN, LOW);
  delay(50);
  pinMode(PIN_AK4528_VT, OUTPUT);
  digitalWriteFast(PIN_AK4528_VT, HIGH);
  delay(200);
  digitalWriteFast(PIN_AK4528_PDN, HIGH);
//  delay(25);
}

float lin = 0.0f;

void HW_Loop()
{
/*
  if (lin != peak)
  {
    lin = peak;

    if (lin >= clipLevel) clip = 255;

    int blue = map(lin, 0.0f, 1.0f, 0.0f, 200.0f);
    int red = 0;
    int green = 0;
    
    if (clip > 0) {
      red = blue; //((clip & 16) > 0 ? clip : 0);
      blue = 0;      
      clip--;
    }

    //leds.setPixel(0, leds.Color(green, red, blue));
    //leds.show();
    */

    //Serial.printf("%.3f\t%.3f\t%.3f\r\n", chorusModDelayL.get_mod_delay_ms(), chorusModDelayC.get_mod_delay_ms(), chorusModDelayR.get_mod_delay_ms());

    /*
    digitalWriteFast(10, LOW);
    SPI.transfer(digitalReadFast(5) == LOW ? 0xff : 0x7f);
    SPI.transfer(digitalReadFast(4) == LOW ? 0xff : 0x7f);
    SPI.transfer(digitalReadFast(19) == LOW ? 0xff : 0x7f);
    SPI.transfer(digitalReadFast(0) == LOW ? 0xff : 0x7f);
    SPI.transfer(digitalReadFast(1) == LOW ? 0xff : 0x7f);
    SPI.transfer(digitalReadFast(16) == LOW ? 0xff : 0x7f);
    SPI.transfer(digitalReadFast(15) == LOW ? 0xff : 0x7f);
    SPI.transfer(digitalReadFast(18) == LOW ? 0xff : 0x7f);
    digitalWriteFast(10, HIGH);
    */
  //}
}
