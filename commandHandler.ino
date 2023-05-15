#define xstr(a) #a
#define CMD0(_n_)  else if (strncmp(xstr(_n_), com_instance, maxLen) == 0) _n_();
#define CMD1(_n_)  else if (strncmp(xstr(_n_), com_instance, maxLen) == 0) \
     { if (com_pi == 0) _n_(); else if (com_pi >= 1) _n_(com_params[0]); }
#define CMD2(_n_)  else if (strncmp(xstr(_n_), com_instance, maxLen) == 0) \
     { if (com_pi == 0) _n_(); else if (com_pi == 1) _n_(com_params[0]); \
        else if (com_pi >= 2) _n_(com_params[0],com_params[1]); }
#define CMD3(_n_)  else if (strncmp(xstr(_n_), com_instance, maxLen) == 0) \
     { if (com_pi == 0) _n_(); else if (com_pi == 1) _n_(com_params[0]); \
        else if (com_pi == 2) _n_(com_params[0],com_params[1]); \
        else if (com_pi >= 3) _n_(com_params[0],com_params[1],com_params[2]); }
#define CMD4(_n_)  else if (strncmp(xstr(_n_), com_instance, maxLen) == 0) \
     { if (com_pi == 0) _n_(); else if (com_pi == 1) _n_(com_params[0]); \
        else if (com_pi == 2) _n_(com_params[0],com_params[1]); \
        else if (com_pi == 3) _n_(com_params[0],com_params[1],com_params[2]); \
        else if (com_pi >= 4) _n_(com_params[0],com_params[1],com_params[2],com_params[3]); }
#define CMD5(_n_)  else if (strncmp(xstr(_n_), com_instance, maxLen) == 0) \
     { if (com_pi == 0) _n_(); else if (com_pi == 1) _n_(com_params[0]); \
        else if (com_pi == 2) _n_(com_params[0],com_params[1]); \
        else if (com_pi == 3) _n_(com_params[0],com_params[1],com_params[2]); \
        else if (com_pi == 4) _n_(com_params[0],com_params[1],com_params[2],com_params[3]); \
        else if (com_pi >= 5) _n_(com_params[0],com_params[1],com_params[2],com_params[3],com_params[4]); }
#define CMD6(_n_)  else if (strncmp(xstr(_n_), com_instance, maxLen) == 0) \
     { if (com_pi == 0) _n_(); else if (com_pi == 1) _n_(com_params[0]); \
        else if (com_pi == 2) _n_(com_params[0],com_params[1]); \
        else if (com_pi == 3) _n_(com_params[0],com_params[1],com_params[2]); \
        else if (com_pi == 4) _n_(com_params[0],com_params[1],com_params[2],com_params[3]); \
        else if (com_pi == 5) _n_(com_params[0],com_params[1],com_params[2],com_params[3],com_params[4]); \
        else if (com_pi >= 6) _n_(com_params[0],com_params[1],com_params[2],com_params[3],com_params[4],com_params[5]); }        

void handleCommand() {
  if (strncmp("AudioProcessorUsage", com_instance, maxLen) == 0) Serial.println(AudioInterrupts());
  else if (strncmp("AudioProcessorUsageMax", com_instance, maxLen) == 0) Serial.println(AudioInterrupts());
  else if (strncmp("AudioMemoryUsage_F32", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsage_F32());
  else if (strncmp("AudioMemoryUsageMax_F32", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsageMax_F32());
  else if (strncmp("levelIn", com_instance, maxLen) == 0) Serial.println(lastLevelIn, 6);
  else if (strncmp("testNoise", com_instance, maxLen) == 0) testNoise.amplitude(com_params[0]);
  CMD2(doTestTone)
  CMD2(setInputMixer)
  CMD6(setPreampEq)
  CMD5(setGate)
  CMD6(setCompression)
  CMD2(setLimiter)
  CMD4(setAutoWah)
  CMD4(setDistortion)
  CMD5(setToneStack)
  CMD3(setTremolo)
  CMD6(setChorus)
  CMD1(setDimension)
  CMD4(setTriStereoChorus)
  CMD5(setReverb)
  CMD5(setDelay)
  CMD3(setCabSim)
  CMD1(setVolume)
}
