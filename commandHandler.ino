void handleCommand() {
  if (strncmp("AudioProcessorUsage", com_instance, maxLen) == 0) Serial.println(AudioInterrupts());
  else if (strncmp("AudioProcessorUsageMax", com_instance, maxLen) == 0) Serial.println(AudioInterrupts());
  else if (strncmp("AudioProcessorUsageMaxReset", com_instance, maxLen) == 0) AudioProcessorUsageMaxReset();
  else if (strncmp("AudioMemoryUsage", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsage());
  else if (strncmp("AudioMemoryUsageMax", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsageMax());
  else if (strncmp("AudioMemoryUsageMaxReset", com_instance, maxLen) == 0) AudioMemoryUsageMaxReset();
  else if (strncmp("AudioMemoryUsage_F32", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsage_F32());
  else if (strncmp("AudioMemoryUsageMax_F32", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsageMax_F32());
  else if (strncmp("AudioMemoryUsageMaxReset_F32", com_instance, maxLen) == 0) AudioMemoryUsageMaxReset_F32();
  else if (strncmp("reportInput", com_instance, maxLen) == 0) reportInput = com_params[0] != 0.0;
  else if (strncmp("setChannel", com_instance, maxLen) == 0) setChannel((X100_Channels)com_params[0]);
  else if (strncmp("inputMixer", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    inputMixer.gain(0, com_params[0]);
    inputMixer.gain(1, com_params[1]);
    AudioInterrupts();
  }
  else if (strncmp("preEq", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    preEq.setHighpass(0, com_params[0], com_params[1]);
    preEq.setLowShelf(1, com_params[2], com_params[3], com_params[4]);
    preEq.setLowpass(2, com_params[5], com_params[6]);
    preEq.begin();
    AudioInterrupts();
  }
  else if (strncmp("gate", com_instance, maxLen) == 0) {
    dynamics.gate(com_params[0], com_params[1], com_params[2], com_params[3], com_params[4]);
  }
  else if (strncmp("compression", com_instance, maxLen) == 0) {
    dynamics.compression(com_params[0], com_params[1], com_params[2], com_params[3], com_params[4]);
  }
  else if (strncmp("distortion", com_instance, maxLen) == 0) {
    distortion.gain(com_params[0], com_params[2]);
    distortion.curve(com_params[1]);
  }
  else if (strncmp("postEq", com_instance, maxLen) == 0 && com_pi <= posteqnf) {
    AudioNoInterrupts();
    for (int i = 0; i < com_pi; i++) posteqg[i] = com_params[i];
    postEq.equalizerNew(posteqnf, &posteqf[0], &posteqg[0], posteqnc, &posteqc[0], posteqsl);
    AudioInterrupts();
  }
  else if (strncmp("chorus", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    chorusLfo.begin(com_params[0], com_params[1], com_params[2] < 0.5 ? 0 : 3);
    chorusPostFilter.setNotch(0, com_params[3], com_params[4]);
    chorusPostFilter.setLowpass(1, com_params[5], com_params[6]);
    chorusPostFilter.begin();
    chorusPostDelay.delay(0, com_params[7]);
    mixerR.gain(1, com_params[8]);
    mixerL.gain(1, com_params[9]);
    AudioInterrupts();
  }
  else if (strncmp("reverb", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    reverbPreDelay.delay(0, com_params[0]);
    reverb.roomsize(com_params[1]);
    reverb.damping(com_params[2]);
    mixerR.gain(2, com_params[3]);
    mixerL.gain(2, com_params[4]);
    AudioInterrupts();
  }
  else if (strncmp("delay", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    delayMixer.gain(0, com_params[0]);
    delayMixer.gain(1, com_params[1]);
    delayLine.delay(0, com_params[2]);
    delayFilter.setHighpass(0, com_params[3], com_params[4]);
    delayFilter.setLowpass(1, com_params[5], com_params[6]);
    delayFilter.begin();
    mixerR.gain(3, com_params[7]);
    mixerL.gain(3, com_params[8]);
    AudioInterrupts();
  }
  else if (strncmp("dryMix", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    mixerR.gain(0, com_params[0]);
    mixerL.gain(0, com_params[1]); 
    AudioInterrupts();
  }
}
