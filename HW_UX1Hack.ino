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


void HW_Loop() {

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
