#ifdef HW_TGAPro

#include "BALibrary.h"

using namespace BALibrary;

BAAudioControlWM8731     codecControl;
BAGpio                   gpio;  // access to User LED

void HW_Setup() {

  TGA_PRO_MKII_REV1(); // Declare the version of the TGA Pro you are using.
  //TGA_PRO_REVB(x);
  //TGA_PRO_REVA(x);

  gpio.begin();

  codecControl.enable();
  delay(100);
  //codecControl.setHPFDisable(false);
  //codecControl.recalibrateDcOffset();
  codecControl.setRightInputGain(0);
  codecControl.setLeftInputGain(24); // Switch in +0db
  //codecControl.setLeftInputGain(16); // Switch in +12db

  codecControl.setHeadphoneVolume(0.9f);

  inputMixer.gain(0, 1.0f);
  inputMixer.gain(1, 0.0f);
}

void HW_Loop() {

}

#endif
