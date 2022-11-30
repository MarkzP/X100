#include "BALibrary.h"

using namespace BALibrary;

BAAudioControlWM8731     codecControl;
BAGpio                   gpio;  // access to User LED

void HW_Setup() {

  TGA_PRO_MKII_REV1(); // Declare the version of the TGA Pro you are using.
  //TGA_PRO_REVB(x);
  //TGA_PRO_REVA(x);

  gpio.begin();

  // If the codec was already powered up (due to reboot) power itd own first
  codecControl.disable();
  delay(100);
  codecControl.enable();
  delay(100);

  codecControl.setHeadphoneVolume(0.5f);
}

int clip = 0;
float clipLevel = 0.944061f; // -0.5db
elapsedMillis reportInput;
float levelAvg = 0.0f;

void HW_Loop() {
  if (reportInput > 20) {
    reportInput = 0;
    levelAvg += (dynamics.readDetector() - levelAvg) * 0.1f;
    
    Serial.print("-126,0,");
    Serial.println(levelAvg, 5);
  }
  
  if (inputLevel.available())
  {
    float lin = fabsf(inputLevel.read());

    if (lin > clipLevel) {
      gpio.setLed(); 
      clip = 255;
    }    
    
    if (clip > 0 && --clip == 0) {
      gpio.clearLed();
    }
  }
}
