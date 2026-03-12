#include "Effect.h"

void handleCommand() {
  if (strncmp("stats", com_instance, maxLen) == 0) Serial.printf("%.1f%%/%.1f%%, 16=%u/%u, 32=%u/%u, G=%.4f\r\n", AudioProcessorUsage(), AudioProcessorUsageMax(), AudioMemoryUsage(), AudioMemoryUsageMax(), AudioMemoryUsage_F32(), AudioMemoryUsageMax_F32(), hdr.gain());
  else if (strncmp("printLevels", com_instance, maxLen) == 0) printLevels = (com_pi == 0 ? !printLevels : com_params[0] != 0.0f);
  else if (strncmp("printTuner", com_instance, maxLen) == 0) printTuner = (com_pi == 0 ? !printTuner : com_params[0] != 0.0f);
  else if (strncmp("setVolume", com_instance, maxLen) == 0 && com_pi >= 1) setVolume(com_params[0]);
  else if (strncmp("tone", com_instance, maxLen) == 0 && com_pi >= 2) inputTone(com_params[0], com_params[1]);
  else if (strncmp("doTestTone", com_instance, maxLen) == 0 && com_pi >= 2) doTestTone(com_params[0], com_params[1], com_pi > 2 ? com_params[2] : 0.0f);
  else if (strncmp("debug", com_instance, maxLen) == 0) debug = (com_pi == 0 ? true : com_params[0] != 0.0f);

  else if (strncmp("dumpAll", com_instance, maxLen) == 0) Effect::dumpAll(&Serial);
  else if (strncmp("resetAll", com_instance, maxLen) == 0) Effect::resetAll();
  else if (strncmp("reset", com_function, maxLen) == 0 && com_pi == 0) Effect::reset(com_instance);

  else if (strncmp("listPresets", com_instance, maxLen) == 0) listPresets();
  else if (strncmp("loadPreset", com_instance, maxLen) == 0 && com_vi > 0) loadPreset(com_value);
  else if (strncmp("savePreset", com_instance, maxLen) == 0) savePreset(com_value);
  else if (strncmp("dumpPreset", com_instance, maxLen) == 0) dumpPreset(com_value);  
  else if (strncmp("printPreset", com_instance, maxLen) == 0) printPreset(&Serial);
  else if (strncmp("presetName", com_instance, maxLen) == 0 && com_vi > 0) presetName(com_value);

  else if (strncmp("bind", com_function, maxLen) == 0) IBindable::bind(com_instance, com_value);
  else if (strncmp("unbind", com_function, maxLen) == 0) IBindable::unbind(com_instance);
  
  else updateParam(com_instance, com_function, com_params[0]);
}
