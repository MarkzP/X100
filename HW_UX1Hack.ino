#include <WS2812Serial.h>

const int numled = 1;
const int pin = 17;
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED
WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_RGB);

#define PIN_AK4528_VT   9
#define PIN_AK4528_PDN  22
#define PIN_DIST        2
#define PIN_EDGE        3
#define PIN_CLN1        5
#define PIN_CLN2        4

elapsedMillis hold_Dist;
elapsedMillis hold_Edge;
elapsedMillis hold_Cln1;
elapsedMillis hold_Cln2;

void HW_Setup() {

  leds.begin();
  leds.setBrightness(200);
  
  pinMode(PIN_AK4528_PDN, OUTPUT);
  digitalWriteFast(PIN_AK4528_PDN, LOW);
  delay(100);
  pinMode(PIN_AK4528_VT, OUTPUT);
  digitalWriteFast(PIN_AK4528_VT, HIGH);
  delay(500);
  digitalWriteFast(PIN_AK4528_PDN, HIGH);
  delay(100);
  
  pinMode(PIN_DIST, INPUT_PULLUP);
  pinMode(PIN_EDGE, INPUT_PULLUP);
  pinMode(PIN_CLN1, INPUT_PULLUP);
  pinMode(PIN_CLN2, INPUT_PULLUP);
}

int clip = 0;
float clipLevel = 0.944061f; // -0.5db


void HW_Loop() {
  if (inputLevel.available())
  {
    float lin = fabsf(inputLevel.read());

    if (lin > clipLevel) clip = 255;
    
    int level = map(lin, 0.0f, 1.0f, 0.0f, 200.0f);

    int red = channel == X100_Dist || channel == X100_Cln1 ? level : 0;
    int green = channel == X100_Cln1 || channel == X100_Edge ? (level * 0.5f) : 0;
    int blue = channel == X100_Cln2 ? level : 0;

    if (clip > 0) {
      red = ((clip & 16) > 0 ? clip : 0);
      green = 0;
      blue = 0;      
      clip--;
    }
    
    leds.setPixel(0, leds.Color(green, red, blue));
    leds.show();
  }
  
  if (digitalReadFast(PIN_DIST)) 
  {
    if (hold_Dist > 50 && hold_Dist < 500) setChannel(X100_Dist);
    else if (hold_Dist > 1000) { setReverb(false); }
    hold_Dist = 0;
  }

  if (digitalReadFast(PIN_EDGE))
  {
    if (hold_Edge > 50 && hold_Edge < 500) setChannel(X100_Edge);
    else if (hold_Edge > 1000 && hold_Cln1 == 0) { setDelay(true); }
    else if (hold_Edge > 1000 && hold_Cln1 > 1000) { setInstrument(false); hold_Cln1 = 0; }
    hold_Edge = 0;
  }

  if (digitalReadFast(PIN_CLN1))
  {
    if (hold_Cln1 > 50 && hold_Cln1 < 500) setChannel(X100_Cln1);
    else if (hold_Cln1 > 1000 && hold_Edge == 0) { setInstrument(true); setDelay(false); setReverb(true); setChorus(true); }
    else if (hold_Edge > 1000 && hold_Cln1 > 1000) { setInstrument(false); hold_Edge = 0; }
    hold_Cln1 = 0;
  }

  if (digitalReadFast(PIN_CLN2))
  {
    if (hold_Cln2 > 50 && hold_Cln2 < 500) setChannel(X100_Cln2);
    else if (hold_Cln2 > 1000) { setChorus(false); }
    hold_Cln2 = 0;
  }
}
