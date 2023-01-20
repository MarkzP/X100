void handleCommand() {
  if (strncmp("AudioProcessorUsage", com_instance, maxLen) == 0) Serial.println(AudioInterrupts());
  else if (strncmp("AudioProcessorUsageMax", com_instance, maxLen) == 0) Serial.println(AudioInterrupts());
  else if (strncmp("AudioProcessorUsageMaxReset", com_instance, maxLen) == 0) AudioProcessorUsageMaxReset();
  else if (strncmp("AudioMemoryUsage_F32", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsage_F32());
  else if (strncmp("AudioMemoryUsageMax_F32", com_instance, maxLen) == 0) Serial.println(AudioMemoryUsageMax_F32());
  else if (strncmp("AudioMemoryUsageMaxReset_F32", com_instance, maxLen) == 0) AudioMemoryUsageMaxReset_F32();
  else if (strncmp("levelIn", com_instance, maxLen) == 0 && levelIn.available()) Serial.println(levelIn.read(), 6);
  else if (strncmp("levelOut", com_instance, maxLen) == 0 && levelOut.available()) Serial.println(levelOut.read(), 6);
  else if (strncmp("testNoise", com_instance, maxLen) == 0) testNoise.amplitude(com_params[0]);
  else if (strncmp("testTone", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    testTone.frequency(com_params[0]);
    testTone.amplitude(com_params[1]);
    AudioInterrupts();
  }
#ifdef LOOPER_EXTMEM  
  else if (strncmp("stop", com_instance, maxLen) == 0) looper.stop();
  else if (strncmp("play", com_instance, maxLen) == 0) looper.play();
  else if (strncmp("rec", com_instance, maxLen) == 0) looper.rec();
  else if (strncmp("dub", com_instance, maxLen) == 0) looper.dub();
  else if (strncmp("signature", com_instance, maxLen) == 0) looper.signature(com_params[0], com_params[1], com_params[2]);
  else if (strncmp("loopMix", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    chorusMixer.gain(1, com_params[0]);
    mixerL.gain(3, com_params[1]);
    mixerR.gain(3, com_params[1]); 
    AudioInterrupts();
  }
#endif
  else if (strncmp("setChannel", com_instance, maxLen) == 0) setChannel((X100_Channels)com_params[0]);
  else if (strncmp("setInstrument", com_instance, maxLen) == 0) setInstrument(com_params[0] >= 0.5f);
  else if (strncmp("setChorus", com_instance, maxLen) == 0) setChorus(com_params[0] >= 0.5f);
  else if (strncmp("setReverb", com_instance, maxLen) == 0) setReverb(com_params[0] >= 0.5f);
  else if (strncmp("setDelay", com_instance, maxLen) == 0) setDelay(com_params[0] >= 0.5f);
  else if (strncmp("setWah", com_instance, maxLen) == 0) setWah(com_params[0] >= 0.5f);
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
  else if (strncmp("detector", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    scFilter.setHighpass(0, com_params[0], com_params[1]);
    scFilter.begin();
    dynamics.detector((AudioEffectDynamics_F32::DetectorTypes)com_params[2], com_params[3], com_params[4]);
    AudioInterrupts();
  }
  else if (strncmp("gate", com_instance, maxLen) == 0) dynamics.gate(com_params[0], com_params[1], com_params[2], com_params[3], com_params[4]);
  else if (strncmp("compression", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    dynamics.compression(com_params[0], com_params[1], com_params[2], com_params[3], com_params[4]);
    if (com_params[5] < 0.0) dynamics.autoMakeupGain(com_params[5]); else dynamics.makeupGain(com_params[5]);
    AudioInterrupts();
  }
  else if (strncmp("limit", com_instance, maxLen) == 0) dynamics.limit(com_params[0], com_params[1], com_params[2]);
  else if (strncmp("wahFilter", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    wahModMixer.gain(0, com_params[0]);
    wahModMixer.gain(1, 1.0f);
    wahLfo.begin(com_params[1], com_params[2], WAVEFORM_SINE);
    wahFilter.frequency(com_params[3]);
    wahFilter.resonance(com_params[4]);
    wahFilter.octaveControl(com_params[5]);
    wahFilter.controlSmoothing(com_params[6]);
    wahMixer.gain(0, com_params[7]);
    wahMixer.gain(1, com_params[8]);
    wahMixer.gain(2, com_params[9]);
    wahMixer.gain(3, com_params[10]);
    AudioInterrupts();
  }
  else if (strncmp("distortion", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    distortion.gain(com_params[0]);
    distortion.curve(com_params[1]);
    distortion.clip(com_params[2]);
    distortion.level(com_params[3]);
    distortion.tone(com_params[4]);
    AudioInterrupts();
  }
  else if (strncmp("postEq", com_instance, maxLen) == 0 && com_pi <= posteqnf) {
    AudioNoInterrupts();
    for (int i = 0; i < com_pi && i < posteqnf; i++) posteqg[i] = com_params[i];
    postEq.equalizerNew(posteqnf, &posteqf[0], &posteqg[0], posteqnc, &posteqc[0], posteqsl);
    AudioInterrupts();
  }
  else if (strncmp("chorus", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    chorusMixer.gain(0, 1.0f);
    chorusMixer.gain(1, com_params[0]);
    chorusModDelay.delay(com_params[1]);
    chorusLfo.begin(com_params[2], com_params[3], (com_params[4] < 0.5 ? WAVEFORM_SINE : WAVEFORM_TRIANGLE));
    chorusPostFilter.setNotch(0, com_params[5], com_params[6]);
    chorusPostFilter.setLowpass(1, com_params[7], com_params[8]);
    chorusPostFilter.begin();
    chorusPostDelay.delay(com_params[9]);
    effectMixerL.gain(0, com_params[10]);
    effectMixerR.gain(0, com_params[10]);
    effectMixerL.gain(1, com_params[11]);
    effectMixerR.gain(1, (com_params[12] < 0.5f ? com_params[11] : -com_params[11]));
    AudioInterrupts();
  }
  else if (strncmp("delay", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    delayMixer.gain(1, com_params[0]);
    delayLine.delay(com_params[1]);
    delayFilter.setHighpass(0, com_params[2], com_params[3]);
    delayFilter.setLowpass(1, com_params[4], com_params[5]);
    delayFilter.begin();
    delayPostDelay.delay(com_params[6]);
    effectMixerL.gain(2, com_params[7]);
    effectMixerR.gain(2, com_params[7]);
    AudioInterrupts();
  }
  else if (strncmp("reverb", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    reverbMixer.gain(0, com_params[0] * 0.5f);
    reverbMixer.gain(1, com_params[0] * 0.5f);
    reverbPreDelay.delay(com_params[1]);
    reverb.roomsize(com_params[2]);
    reverb.damping(com_params[3]);
    mixerL.gain(0, com_params[4]);
    mixerR.gain(0, com_params[4]); 
    mixerL.gain(1, com_params[5]);
    mixerR.gain(1, com_params[5]);
    AudioInterrupts();
  }
  else if (strncmp("tremolo", com_instance, maxLen) == 0) {
    AudioNoInterrupts();
    tremoloLfo.begin(com_params[0], com_params[1], (short)com_params[2]);
    tremoloVca.controlSmoothing(com_params[3]);
    tremoloVca.offset(1.0f - (com_params[0] * 0.5f));
    AudioInterrupts();
  }  
}
