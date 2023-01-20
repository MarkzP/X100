<Query Kind="Program">
  <Namespace>LINQPad.Controls</Namespace>
</Query>

class SliderGroup
{
	abstract class SliderDef
	{
		public string name { get; protected set; }
		public Span span { get; protected set; }
		public Control control { get; protected set; }
		
		public abstract void Reset();

		public void Update()
		{
			span.Text = $"{name} {this}";
		}
	}

	class SliderFloat : SliderDef
	{
		const float mult = 1000.0f;

		RangeControl range;
		int def;

		public SliderFloat(SliderGroup group, string name, float min, float max, float def)
		{
			base.name = name;
			this.range = new RangeControl((int)(min * mult), (int)(max * mult), (int)(def * mult));
			this.def = range.Value;
			this.range.Tag = this;
			this.range.Width = "90%";
			this.range.ValueInput += group.QueueValues;

			span = new Span($"{name} {this}");
			control = range;
		}

		public override string ToString() => ((float)range.Value / mult).ToString("F4", System.Globalization.CultureInfo.InvariantCulture);

		public override void Reset()
		{
			range.Value = def;
			Update();
		}
	}

	class SliderFreq : SliderDef
	{
		const int rmax = 1000;
		const double flogscale = 10.0;
		const double fscales = 3 - 0.3;
		double exp;

		RangeControl range;
		int def;

		public SliderFreq(SliderGroup group, string name, float min, float max, float def)
		{
			exp = Math.Pow(200, 0.0001);
			
			base.name = name;
			this.range = new RangeControl(0, 10000, (int)Math.Round(Math.Log(0.75 + (def / 100.0), exp)));
			this.def = range.Value;
			this.range.Tag = this;
			this.range.Width = "90%";
			this.range.ValueInput += group.QueueValues;

			span = new Span($"{name} {this}");
			control = range;
		}

		public override string ToString() => (25 * (int)(Math.Round(4.0 * Math.Pow(exp, (double)range.Value)) - 3)).ToString("F4", System.Globalization.CultureInfo.InvariantCulture);

		public override void Reset()
		{
			range.Value = def;
			Update();
		}
	}


	class SliderDbToUnit : SliderDef
	{
		RangeControl range;
		int def;

		public SliderDbToUnit(SliderGroup group, string name, float def)
		{
			base.name = name;
			this.range = new RangeControl(-126, 0, (int)(6.02 * Math.Log(def, 2.0)));
			this.def = range.Value;
			this.range.Tag = this;
			this.range.Width = "90%";
			this.range.ValueInput += group.QueueValues;

			span = new Span($"{name} {this}");
			control = range;
		}

		public override string ToString() => (Math.Pow(2.0, (double)range.Value * 0.16612)).ToString("F6", System.Globalization.CultureInfo.InvariantCulture);

		public override void Reset()
		{
			range.Value = def;
			Update();
		}
	}


	class SliderInt : SliderDef
	{
		RangeControl range;
		int def;

		public SliderInt(SliderGroup group, string name, int min, int max, int def)
		{
			base.name = name;
			this.def = def;
			this.range = new RangeControl(min, max, def);
			this.range.Tag = this;
			this.range.Width = "90%";
			this.range.ValueInput += group.QueueValues;

			span = new Span($"{name} {def}");
			control = range;
		}

		public override string ToString() => range.Value.ToString();

		public override void Reset()
		{
			range.Value = def;
			Update();
		}
	}

	static Dictionary<string, string> lastSend = new Dictionary<string, string>();
	static Dictionary<string, string> sendQueue = new Dictionary<string, string>();
	static object queueLock = new object();
	static object portLock = new object();
	static Timer sendTimer = new Timer(processQueue, null, Timeout.Infinite, Timeout.Infinite);

	static void processQueue(object state)
	{
		var msgs = new List<string>();
		lock (queueLock)
		{
			foreach (var kvp in sendQueue)
			{
				if (!lastSend.TryGetValue(kvp.Key, out string value) || value != kvp.Value)
				{
					lastSend[kvp.Key] = kvp.Value;
					msgs.Add($"{kvp.Key}({kvp.Value});");
				}
			}
			sendQueue.Clear();
		}

		if (!msgs.Any()) return;

		//msgs.ForEach(m => Console.WriteLine(m));

		try
		{
			lock (portLock)
			{
				using (var port = new System.IO.Ports.SerialPort("COM10"))
				{
					port.Open();
					port.ReadTimeout = 15;
					msgs.ForEach(m =>
					{
						port.WriteLine(m);
						Thread.Sleep(5);
						//var resp = port.ReadLine();
						//if (!string.IsNullOrEmpty(resp)) Console.WriteLine(resp);
					});
				}
			}
		}
		catch {}			
	}
	
	string name;
	string channel;
	StackPanel sp;
	
	IEnumerable<SliderDef> sliders => sp.Children
			.Where(o => o is Control c && c.Tag is SliderDef)
			.Select(o => (o as Control).Tag as SliderDef);
	
	void QueueValues(object sender, EventArgs e)
	{
		var values = sliders
				.Select(idf =>
				{
					idf.Update();
					return idf.ToString();
				});

		lock (queueLock)
		{
			sendQueue[channel] = string.Join(",", values);
			sendTimer.Change(20, Timeout.Infinite);
		}
	}
	
	public SliderGroup(string name, string channel = null, bool horizontal = false)
	{
		this.name = name;
		this.channel = channel;
		sp = new StackPanel(false, ".1em");
		sp.Horizontal = horizontal;

		if (!string.IsNullOrEmpty(channel))
		{
			var resetBtn = new Button("Reset");
			resetBtn.Click += (o, e) =>
			{
				var values = sliders.Select(s => { s.Reset(); return s.ToString(); });
				lock (queueLock)
				{
					sendQueue[channel] = string.Join(",", values);
					sendTimer.Change(20, Timeout.Infinite);
				}
			};
			sp.Children.Add(resetBtn);
		}
		
	}
	
	public SliderGroup AddButton(string name, string command, string parms = null)
	{
		var btn = new Button(name);
		btn.Click += (o, e) =>
		{
			lock (queueLock)
			{
				sendQueue[command] = parms ?? DateTime.Now.Millisecond.ToString();
				sendTimer.Change(20, Timeout.Infinite);
			}
		};
		sp.Children.Add(btn);
		return this;
	}
	
	public SliderGroup AddSlider(string name, float min, float max, float value)
	{
		var def = new SliderFloat(this, name, min, max, value);

		sp.Children.Add(def.span);
		sp.Children.Add(def.control);
		return this;
	}

	public SliderGroup AddSlider(string name, int min, int max, int value)
	{
		var def = new SliderInt(this, name, min, max, value);

		sp.Children.Add(def.span);
		sp.Children.Add(def.control);
		return this;
	}

	public SliderGroup AddFreqSlider(string name, float value, float min = 20.0f, float max = 20000.0f)
	{
		var def = new SliderFreq(this, name, min, max, value);

		sp.Children.Add(def.span);
		sp.Children.Add(def.control);
		return this;
	}
	
	public SliderGroup AddDbToUnitSlider(string name, float value)
	{
		var def = new SliderDbToUnit(this, name, value);

		sp.Children.Add(def.span);
		sp.Children.Add(def.control);
		return this;
	}
	
	
	public SliderGroup AddDbSlider(string name, float value, float min = -96.0f, float max = 96.0f) => AddSlider(name, min, max, value);
	public SliderGroup AddEqSlider(float freq, float value, float min = -36.0f, float max = 12.0f) => AddSlider($"{freq} Hz", min, max, value);
	public SliderGroup AddGainSlider(string name, float value, float min = 0.0f, float max = 1.0f) => AddSlider(name, min, max, value);
	public SliderGroup AddQSlider(string name, float value = 0.7071f) => AddSlider(name, 0.5f, 2.5f, value);
	public SliderGroup AddSlopeSlider(string name, float value) => AddSlider(name, 0.001f, 2.0f, value);
	public SliderGroup AddTimeSlider(string name, float value, float max = 10.0f) => AddSlider(name, 0.0f, max, value);	
	public SliderGroup AddWaveformSlider(string name, int value) => AddSlider(name, 0.0f, 1.0f, (float)value);
	
	public FieldSet Dump() => new FieldSet(name, sp).Dump();
}

void Main()
{
	new SliderGroup("Test Tone", "testTone")
		.AddFreqSlider("Frequency", 1000.0f)
		.AddGainSlider("Amplitude", 0.0f, 0.0f, 1.5f)
		.Dump();
		
	new SliderGroup("Test Noise", "testNoise")
		.AddGainSlider("Amplitude", 0.0f)
		.Dump();
	
	new SliderGroup("Channel Presets", horizontal: true)
		.AddButton("Clean 2", "setChannel", "4")
		.AddButton("Clean 1", "setChannel", "3")
		.AddButton("Edge", "setChannel", "2")
		.AddButton("Dist", "setChannel", "1")
		.Dump();
		
	new SliderGroup("Effects Presets", horizontal: true)
		.AddButton("Chorus On", "setChorus", "1")
		.AddButton("Chorus Off", "setChorus", "0")
		.AddButton("Delay On", "setDelay", "1")
		.AddButton("Delay Off", "setDelay", "0")
		.AddButton("Reverb On", "setReverb", "1")
		.AddButton("Reverb Off", "setReverb", "0")
		.Dump();
	
	new SliderGroup("Input Mixer", "inputMixer")
			.AddGainSlider("Microphone", 0.0f, 0.0f, 5.0f)
			.AddGainSlider("Instrument", 1.0f, 0.0f, 5.0f)
			.Dump();

	new SliderGroup("preEq", "preEq")
			.AddFreqSlider("High Pass freq", 200.0f)
			.AddQSlider("High Pass Q")
			.AddFreqSlider("Low Shelf freq", 1000.0f)
			.AddDbSlider("Low Shelf gain", -16.0f, -20.0f, 6.0f)
			.AddSlopeSlider("Low Shelf slope", 0.2f)
			.AddFreqSlider("Low Pass freq", 2800.0f)
			.AddQSlider("Low Pass Q", 1.5f)
			.Dump();

	new SliderGroup("Dynamics: Detector", "detector")
			.AddFreqSlider("High Pass freq", 20.0f)
			.AddQSlider("High Pass Q")
			.AddSlider("Detector Type (Full bridge peak, Half bridge peak, RMS)", 0, 2, 0)
			.AddTimeSlider("Decay/RMS Windows", 0.002f, 0.050f)
			.AddSlider("Diodes Forward Voltage Drop", 0.0f, 0.1f, 0.0f)
			.Dump();
	
	new SliderGroup("Dynamics: Gate", "gate")
			.AddDbSlider("Threshold", -75.0f, -126.0f, 0.0f)
			.AddTimeSlider("Attack", 0.0f, 0.5f)
			.AddTimeSlider("Release", 3.0f)
			.AddDbSlider("Hysterisis", 6.0f, 0.0f, 12.0f)
			.AddDbSlider("Attenuation", -14.0f, -126.0f, 0.0f)			
			.Dump();

	new SliderGroup("Dynamics: Compression", "compression")
			.AddDbSlider("Threshold", -55.0f, -126.0f, 0.0f)
			.AddTimeSlider("Attack", 0.0015f, 0.5f)
			.AddTimeSlider("Release", 2.5f)
			.AddSlider("Ratio", 1.0f, 20.0f, 4.5f)
			.AddSlider("Knee width", 0.0f, 9.0f, 3.0f)
			.AddDbSlider("Makeup Gain", -6.0f, -60.0f, 60.0f)
			.Dump();

	new SliderGroup("Dynamics: Limiter", "limit")
			.AddDbSlider("Threshold", 0.0f, -126.0f, 0.0f)
			.AddTimeSlider("Attack", 0.0f, 0.5f)
			.AddTimeSlider("Release", 3.0f)
			.Dump();

	new SliderGroup("VCF (Wah/Filter)", "wahFilter")
			.AddSlider("Auto Gain", -50.0f, 50.0f, 0.0f)
			.AddGainSlider("LFO Depth", 0.0f, 0.0f, 1.0f)
			.AddSlider("LFO Rate", 0.1f, 10.0f, 1.7f)
			.AddFreqSlider("Center Frequency", 250.0f)
			.AddSlider("Resonance", 0.5f, 5.0f, 3.5f)
			.AddSlider("Octave control", 0.0f, 6.0f, 3.0f)
			.AddTimeSlider("Smoothing", 0.1f, 0.5f)
			.AddGainSlider("Dry", 1.0f)
			.AddGainSlider("Wet - LowPass", 0.0f)
			.AddGainSlider("Wet - BandPass", 0.0f)
			.AddGainSlider("Wet - HighPass", 0.0f)
			.Dump();

	new SliderGroup("Distortion", "distortion")
			.AddSlider("Gain", 1.0f, 100.0f, 1.0f)
			.AddSlider("Curve", 0.0f, 100.0f, 1.0f)
			.AddSlider("Clip", 0.1f, 2.0f, 1.0f)
			.AddSlider("Level", 0.0f, 1.0f, 1.0f)
			.AddSlider("Tone", 0.0f, 1.0f, 1.0f)
			.Dump();

	new SliderGroup("Post Eq", "postEq")
			.AddEqSlider(50.0f,   -30.0f)
			.AddEqSlider(100.0f,   -6.0f)
			.AddEqSlider(200.0f,   -1.5f)
			.AddEqSlider(400.0f,   -1.5f)
			.AddEqSlider(800.0f,   -9.0f)
			.AddEqSlider(1600.0f, -12.0f)
			.AddEqSlider(3200.0f, 1.5f)
			.AddEqSlider(6400.0f, -1.5f)
			.AddEqSlider(12800.0f, -30.0f)
			.Dump();

	new SliderGroup("Looper Control", horizontal: true)
		.AddButton("Stop", "stop")
		.AddButton("Play", "play")
		.AddButton("Rec", "rec")
		.AddButton("OverDub", "dub")
		.Dump();

	
	new SliderGroup("Looper Signature", "signature")
		.AddSlider("BPM", 30, 480, 100)
		.AddSlider("Beats Per Bar", 2, 16, 4)
		.AddSlider("Bars", 1, 16, 4)
		.Dump();

	new SliderGroup("Looper", "loopMix")
		.AddGainSlider("Mix", 1.0f)
		.AddGainSlider("Metronome", 0.0f)
		.Dump();

	new SliderGroup("VCA (Tremolo)", "tremolo")
			.AddGainSlider("Depth", 0.0f, 0.0f, 1.0f)
			.AddSlider("Rate", 0.1f, 10.0f, 1.7f)
			.AddSlider("Wave (Sine, Saw, Square, Triangle)", 0, 3, 0)
			.AddTimeSlider("Smoothing", 0.05f, 0.5f)
			.Dump();

	new SliderGroup("Chorus/Flanger", "chorus")
			.AddGainSlider("Resonance", 0.2f, 0.0f, 0.75f)
			.AddSlider("Delay (msec)", 0.0f, 30.0f, 23.0f)
			.AddGainSlider("Depth", 0.032f, 0.0f, 1.0f)
			.AddSlider("Rate", 0.1f, 10.0f, 1.7f)
			.AddSlider("Wave (Sine or Triangle)", 0, 1, 1)
			.AddFreqSlider("Notch freq", 6500.0f)
			.AddQSlider("Notch Q", 0.7f)
			.AddFreqSlider("Low Pass freq", 8000.0f)
			.AddQSlider("Low Pass Q", 0.5f)
			.AddSlider("Post Delay (R) (msec)", 0.0f, 30.0f, 17.0f)
			.AddGainSlider("Dry", 1.0f)
			.AddGainSlider("Mix", 0.750f)
			.AddSlider("Phase Reverse", 0, 1, 1)
			.Dump();

	new SliderGroup("Delay", "delay")
			.AddGainSlider("Feedback", 0.3f)
			.AddSlider("Delay (msec)", 0.0f, 1000.0f, 500.0f)
			.AddFreqSlider("High Pass freq", 250.0f)
			.AddQSlider("High Pass Q")
			.AddFreqSlider("Low Pass freq", 4000.0f)
			.AddQSlider("Low Pass Q")
			.AddSlider("Post Delay (L) (msec)", 0.0f, 100.0f, 17.0f)
			.AddGainSlider("Mix", 0.0f)
			.Dump();

	new SliderGroup("Reverb", "reverb")
			.AddGainSlider("Input", 1.0f)
			.AddSlider("Pre delay (msec)", 0.0f, 100.0f, 45.0f)
			.AddSlider("Room size", 0.0f, 1.0f, 0.8f)
			.AddSlider("Damping", 0.0f, 1.0f, 0.002f)
			.AddGainSlider("Dry", 1.0f)
			.AddGainSlider("Mix", 0.018f)
			.Dump();
}