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
  codecControl.setHPFDisable(false);
  codecControl.setRightInputGain(24);
  codecControl.setLeftInputGain(24); // Switch in +0db

  //codecControl.setHeadphoneVolume(0.5f);
}

void HW_Loop() {

}
