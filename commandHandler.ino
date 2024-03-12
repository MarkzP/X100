#include "Effect.h"

void handleCommand() {
  if (strncmp("stats", com_instance, maxLen) == 0) Serial.printf("Proc=%.1f%%/%.1f%%, Mem16=%u/%u, Mem32=%u/%u\r\n", AudioProcessorUsage(), AudioProcessorUsageMax(), AudioMemoryUsage(), AudioMemoryUsageMax(), AudioMemoryUsage_F32(), AudioMemoryUsageMax_F32());
  else if (strncmp("printCompGain", com_instance, maxLen) == 0 && com_pi >= 1) printCompGain = com_params[0] != 0.0f;
  else if (strncmp("setVolume", com_instance, maxLen) == 0 && com_pi >= 1) setVolume(com_params[0]);
  else if (strncmp("levelIn", com_instance, maxLen) == 0) Serial.println(lastLevelIn, 6);
  else if (strncmp("doTestTone", com_instance, maxLen) == 0 && com_pi >= 2) doTestTone(com_params[0], com_params[1]);
  else if (strncmp("dumpAll", com_instance, maxLen) == 0) Effect::dumpAll(&Serial);
  else if (strncmp("loadPreset", com_instance, maxLen) == 0 && com_vi > 0) loadPreset(com_value);
  else if (strncmp("savePreset", com_instance, maxLen) == 0) savePreset(com_value);
  else if (strncmp("listPresets", com_instance, maxLen) == 0) listPresets();
  else if (strncmp("dumpPreset", com_instance, maxLen) == 0) dumpPreset(com_value);  
  else if (strncmp("printPreset", com_instance, maxLen) == 0) printPreset(&Serial);
  else if (strncmp("bind", com_function, maxLen) == 0 && com_vi > 0) IBindable::bind(com_function, com_value);
  else if (strncmp("presetName", com_instance, maxLen) == 0 && com_vi > 0) presetName(com_value);
  else if (strncmp("reset", com_function, maxLen) == 0 && com_pi == 0) resetEffect(com_instance);
  else if (strncmp("update", com_function, maxLen) == 0 && com_pi > 0) updateEffect(com_instance, com_pi, com_params);
  else updateParam(com_instance, com_function, com_params[0]);
}
