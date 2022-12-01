void handleCommand() {
  if (strncmp("AudioNoInterrupts", com_instance, maxLen) == 0) AudioNoInterrupts();
  else if (strncmp("AudioInterrupts", com_instance, maxLen) == 0) AudioInterrupts();
  else if (strncmp("setChannel", com_instance, maxLen) == 0) setChannel((X100_Channels)com_params[0]);
  else if (strncmp("distortion", com_instance, maxLen) == 0) {
    distortion.gain(com_params[0], com_params[2]);
    distortion.curve(com_params[1]);
  }
  else if (strncmp("mixerLR", com_instance, maxLen) == 0) {
    for (int i = 0; i < com_pi && i < 8; i++) {
      mixerL.gain(i, com_params[i]); mixerR.gain(i, com_params[i]); 
    }
  }
  else if (strncmp("postEq", com_instance, maxLen) == 0 && com_pi <= posteqnf) {
    for (int i = 0; i < com_pi; i++) posteqg[i] = com_params[i];
    postEq.equalizerNew(posteqnf, &posteqf[0], &posteqg[0], posteqnc, &posteqc[0], posteqsl);
  }
// BEGIN AUTO GENERATED
  else if (strncmp("preEq", com_instance, maxLen) == 0) {
      if (strncmp("setLowpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        preEq.setLowpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setHighpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        preEq.setHighpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setBandpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        preEq.setBandpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setNotch", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        preEq.setNotch(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setLowShelf", com_function, maxLen) == 0 && com_pi == 4) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_gain = (float)com_params[2];
        float _c_slope = (float)com_params[3];
        preEq.setLowShelf(_c_stage, _c_frequency, _c_gain, _c_slope);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setHighShelf", com_function, maxLen) == 0 && com_pi == 4) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_gain = (float)com_params[2];
        float _c_slope = (float)com_params[3];
        preEq.setHighShelf(_c_stage, _c_frequency, _c_gain, _c_slope);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("begin", com_function, maxLen) == 0 && com_pi == 0) {
        preEq.begin();
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("inputMixer", com_instance, maxLen) == 0) {
      if (strncmp("gain", com_function, maxLen) == 0 && com_pi == 2) {
        int _c_channel = (int)com_params[0];
        float _c_gain = (float)com_params[1];
        inputMixer.gain(_c_channel, _c_gain);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("dynamics", com_instance, maxLen) == 0) {
      if (strncmp("detector", com_function, maxLen) == 0 && com_pi == 3) {
        AudioEffectDynamics_F32::DetectorTypes _c_detectorType = (AudioEffectDynamics_F32::DetectorTypes)com_params[0];
        float _c_time = (float)com_params[1];
        float _c_voltage = (float)com_params[2];
        dynamics.detector(_c_detectorType, _c_time, _c_voltage);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("gate", com_function, maxLen) == 0 && com_pi == 5) {
        float _c_threshold = (float)com_params[0];
        float _c_attack = (float)com_params[1];
        float _c_release = (float)com_params[2];
        float _c_hysterisis = (float)com_params[3];
        float _c_attenuation = (float)com_params[4];
        dynamics.gate(_c_threshold, _c_attack, _c_release, _c_hysterisis, _c_attenuation);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("compression", com_function, maxLen) == 0 && com_pi == 5) {
        float _c_threshold = (float)com_params[0];
        float _c_attack = (float)com_params[1];
        float _c_release = (float)com_params[2];
        float _c_ratio = (float)com_params[3];
        float _c_kneeWidth = (float)com_params[4];
        dynamics.compression(_c_threshold, _c_attack, _c_release, _c_ratio, _c_kneeWidth);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("limit", com_function, maxLen) == 0 && com_pi == 3) {
        float _c_threshold = (float)com_params[0];
        float _c_attack = (float)com_params[1];
        float _c_release = (float)com_params[2];
        dynamics.limit(_c_threshold, _c_attack, _c_release);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("autoMakeupGain", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_headroom = (float)com_params[0];
        dynamics.autoMakeupGain(_c_headroom);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("makeupGain", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_gain = (float)com_params[0];
        dynamics.makeupGain(_c_gain);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("distortion", com_instance, maxLen) == 0) {
      if (strncmp("gain", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_gain = (float)com_params[0];
        distortion.gain(_c_gain);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("curve", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_curve = (float)com_params[0];
        distortion.curve(_c_curve);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("chorusLfo", com_instance, maxLen) == 0) {
      if (strncmp("begin", com_function, maxLen) == 0 && com_pi == 1) {
        short _c_waveform = (short)com_params[0];
        chorusLfo.begin(_c_waveform);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("begin", com_function, maxLen) == 0 && com_pi == 3) {
        short _c_waveform = (short)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_amplitude = (float)com_params[2];
        chorusLfo.begin(_c_waveform, _c_frequency, _c_amplitude);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("frequency", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_frequency = (float)com_params[0];
        chorusLfo.frequency(_c_frequency);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("amplitude", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_amplitude = (float)com_params[0];
        chorusLfo.amplitude(_c_amplitude);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("delayMixer", com_instance, maxLen) == 0) {
      if (strncmp("gain", com_function, maxLen) == 0 && com_pi == 2) {
        int _c_channel = (int)com_params[0];
        float _c_gain = (float)com_params[1];
        delayMixer.gain(_c_channel, _c_gain);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("chorusPostFilter", com_instance, maxLen) == 0) {
      if (strncmp("setLowpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        chorusPostFilter.setLowpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setHighpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        chorusPostFilter.setHighpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setBandpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        chorusPostFilter.setBandpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setNotch", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        chorusPostFilter.setNotch(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setLowShelf", com_function, maxLen) == 0 && com_pi == 4) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_gain = (float)com_params[2];
        float _c_slope = (float)com_params[3];
        chorusPostFilter.setLowShelf(_c_stage, _c_frequency, _c_gain, _c_slope);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setHighShelf", com_function, maxLen) == 0 && com_pi == 4) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_gain = (float)com_params[2];
        float _c_slope = (float)com_params[3];
        chorusPostFilter.setHighShelf(_c_stage, _c_frequency, _c_gain, _c_slope);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("begin", com_function, maxLen) == 0 && com_pi == 0) {
        chorusPostFilter.begin();
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("reverbPreDelay", com_instance, maxLen) == 0) {
      if (strncmp("delay", com_function, maxLen) == 0 && com_pi == 2) {
        int _c_channel = (int)com_params[0];
        float _c_delay = (float)com_params[1];
        reverbPreDelay.delay(_c_channel, _c_delay);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("disable", com_function, maxLen) == 0 && com_pi == 1) {
        int _c_channel = (int)com_params[0];
        reverbPreDelay.disable(_c_channel);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("delayFilter", com_instance, maxLen) == 0) {
      if (strncmp("setLowpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        delayFilter.setLowpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setHighpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        delayFilter.setHighpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setBandpass", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        delayFilter.setBandpass(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setNotch", com_function, maxLen) == 0 && com_pi == 3) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_Q = (float)com_params[2];
        delayFilter.setNotch(_c_stage, _c_frequency, _c_Q);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setLowShelf", com_function, maxLen) == 0 && com_pi == 4) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_gain = (float)com_params[2];
        float _c_slope = (float)com_params[3];
        delayFilter.setLowShelf(_c_stage, _c_frequency, _c_gain, _c_slope);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("setHighShelf", com_function, maxLen) == 0 && com_pi == 4) {
        int _c_stage = (int)com_params[0];
        float _c_frequency = (float)com_params[1];
        float _c_gain = (float)com_params[2];
        float _c_slope = (float)com_params[3];
        delayFilter.setHighShelf(_c_stage, _c_frequency, _c_gain, _c_slope);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("begin", com_function, maxLen) == 0 && com_pi == 0) {
        delayFilter.begin();
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("reverb", com_instance, maxLen) == 0) {
      if (strncmp("roomsize", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_roomsize = (float)com_params[0];
        reverb.roomsize(_c_roomsize);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("damping", com_function, maxLen) == 0 && com_pi == 1) {
        float _c_damping = (float)com_params[0];
        reverb.damping(_c_damping);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("delayLine", com_instance, maxLen) == 0) {
      if (strncmp("delay", com_function, maxLen) == 0 && com_pi == 2) {
        int _c_channel = (int)com_params[0];
        float _c_delay = (float)com_params[1];
        delayLine.delay(_c_channel, _c_delay);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("disable", com_function, maxLen) == 0 && com_pi == 1) {
        int _c_channel = (int)com_params[0];
        delayLine.disable(_c_channel);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("chorusPostDelay", com_instance, maxLen) == 0) {
      if (strncmp("delay", com_function, maxLen) == 0 && com_pi == 2) {
        int _c_channel = (int)com_params[0];
        float _c_delay = (float)com_params[1];
        chorusPostDelay.delay(_c_channel, _c_delay);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
      else if (strncmp("disable", com_function, maxLen) == 0 && com_pi == 1) {
        int _c_channel = (int)com_params[0];
        chorusPostDelay.disable(_c_channel);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("mixerL", com_instance, maxLen) == 0) {
      if (strncmp("gain", com_function, maxLen) == 0 && com_pi == 2) {
        int _c_channel = (int)com_params[0];
        float _c_gain = (float)com_params[1];
        mixerL.gain(_c_channel, _c_gain);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
  else if (strncmp("mixerR", com_instance, maxLen) == 0) {
      if (strncmp("gain", com_function, maxLen) == 0 && com_pi == 2) {
        int _c_channel = (int)com_params[0];
        float _c_gain = (float)com_params[1];
        mixerR.gain(_c_channel, _c_gain);
      }
      // else { Serial.print("?Unknown function: "); Serial.print(com_instance); Serial.print("."); Serial.println(com_function); }
  }
// END AUTO GENERATED
}
