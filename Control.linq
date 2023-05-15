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

	const string portName = "COM10";
	static Dictionary<string, string> sendQueue = new Dictionary<string, string>();
	static object queueLock = new object();
	static object portLock = new object();
	static Timer sendTimer = new Timer(processQueue, null, Timeout.Infinite, Timeout.Infinite);
	static Timer levelTimer = new Timer(getLevels, null, Timeout.Infinite, Timeout.Infinite);

	static RangeControl leftOut = new RangeControl(-126, 0);
	static RangeControl rightOut = new RangeControl(-126, 0);
	static Span leftSpan = new Span("Left");
	static Span rightSpan = new Span("Right");
	static Button pauseBtn = new Button("Resume");
	
	static float unitToDb(float unit) => unit < 5.011872E-07f ? -126.0f : 20.0f * (float)Math.Log10(unit);

	static void processQueue(object state)
	{
		var msgs = string.Empty;
		lock (queueLock)
		{
			foreach (var kvp in sendQueue)
			{
				msgs += $"{kvp.Key}({kvp.Value});";
			}
			sendQueue.Clear();
		}
		
		if (string.IsNullOrEmpty(msgs)) return;
		//msgs.Dump();

		try
		{
			lock (portLock)
			{
				using (var port = new System.IO.Ports.SerialPort(portName))
				{
					port.Open();
					port.ReadTimeout = 15;
					port.Write(msgs);
					Thread.Sleep(10);
					port.ReadExisting();
				}
			}
		}
		catch {}			
	}
	
	static void getLevels(object state)
	{
		try
		{
			lock (portLock)
			{
				using (var port = new System.IO.Ports.SerialPort(portName))
				{
					port.Open();
					port.ReadTimeout = 25;
					port.ReadExisting();
					port.Write("levelOut();");

					var resp = string.Empty;
					do { Thread.Sleep(25); resp = port.ReadExisting(); } while (string.IsNullOrEmpty(resp));
					
					var parts = resp.ToLower().Replace("nan", "-126.0").Replace("inf", "12.0").Split(',').Select(s => s.Trim());
					var dbL = unitToDb(float.Parse(parts.First(), System.Globalization.CultureInfo.InvariantCulture) / 0.7071f);
					var dbR = unitToDb(float.Parse(parts.Last(), System.Globalization.CultureInfo.InvariantCulture) / 0.7071f);

					leftSpan.Text = $"Left   {dbL}db";
					rightSpan.Text = $"Right {dbR}db";
					
					leftOut.Value = (int)dbL;
					rightOut.Value = (int)dbR;
				}
			}
		}
		catch { }

	}
	
	public static void Init()
	{
		var metersSp = new StackPanel(false, ".1em");
		metersSp.Horizontal = false;

		pauseBtn.Click += (o, e) =>
		{
			if (pauseBtn.Text == "Pause")
			{
				pauseBtn.Text = "Resume";
				levelTimer.Change(Timeout.Infinite, Timeout.Infinite);
			}
			else
			{
				pauseBtn.Text = "Pause";
				levelTimer.Change(500, 500);
			}
		};
		metersSp.Children.Add(pauseBtn);
		
		leftOut.Width = "90%";
		leftOut.Enabled = false;
		metersSp.Children.Add(leftSpan);
		metersSp.Children.Add(leftOut);

		rightOut.Width = "90%";
		rightOut.Enabled = false;
		metersSp.Children.Add(rightSpan);
		metersSp.Children.Add(rightOut);

		new FieldSet("Meters", metersSp).Dump();

		//levelTimer.Change(100, 500);
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
			sendTimer.Change(50, Timeout.Infinite);
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

	public SliderGroup AddButton(string name, params (string, string)[] commands)
	{
		var btn = new Button(name);
		btn.Click += (o, e) =>
		{
			lock (queueLock)
			{
				foreach (var command in commands)
				{
					sendQueue[command.Item1] = command.Item2 ?? string.Empty;
				}
				sendTimer.Change(50, Timeout.Infinite);
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
	public SliderGroup AddEqSlider(float freq, float value, float min = -30.0f, float max = 15.0f) => AddSlider($"{freq} Hz", min, max, value);
	public SliderGroup AddGainSlider(string name, float value, float min = 0.0f, float max = 1.0f) => AddSlider(name, min, max, value);
	public SliderGroup AddQSlider(string name, float value = 0.7071f) => AddSlider(name, 0.5f, 2.5f, value);
	public SliderGroup AddSlopeSlider(string name, float value) => AddSlider(name, 0.001f, 2.0f, value);
	public SliderGroup AddTimeSlider(string name, float value, float max = 10.0f) => AddSlider(name, 0.0f, max, value);	
	public SliderGroup AddWaveformSlider(string name, int value) => AddSlider(name, 0.0f, 1.0f, (float)value);
		
	public FieldSet Dump() => new FieldSet(name, sp).Dump();
}


void Main()
{
	SliderGroup.Init();

	new SliderGroup("Test Noise", "testNoise")
		.AddGainSlider("Amplitude", 0.0f)
		.Dump();

	new SliderGroup("Channel Presets", horizontal: true)
		.AddButton("Default",
			("setInputMixer", "1"),
			("setPreampEq", "1"),
			("setGate", "1"),
			("setCompression", "1"),
			("setMakeupGain", "6"),
			("setLimiter", "1"),
			("setAutoWah", "0"),
			("setDistortion", "0"),
			("setToneStack", "1"),
			("setTremolo", "0"),
			("setChorus", "1"),
			("setReverb", "1"),
			("setDelay", "0"),
			("setCabSim", "1"),
			("setVolume", "1")
		)

		.AddButton("Flat", 
			("setPreampEq", "0"),
			("setGate", "0"),
			("setCompression", "0"),
			("setMakeupGain", "0"),
			("setLimiter", "0"),
			("setAutoWah", "0"),
			("setDistortion", "0"),
			("setToneStack", "0"),
			("setTremolo", "0"),
			("setChorus", "0"),
			("setReverb", "0"),
			("setDelay", "0"),
			("setCabSim", "0")
		)
		.Dump();
		
	new SliderGroup("Effects Presets", horizontal: true)
		.AddButton("Compressor On", ("setCompressor", "1"))
		.AddButton("Compressor Off", ("setCompressor", "0"))
		.AddButton("Distortion On", ("setDistortion", "1"))
		.AddButton("Distortion Off", ("setDistortion", "0"))
		.AddButton("Chorus On", ("setChorus", "1"))
		.AddButton("Chorus Off", ("setChorus", "0"))
		.AddButton("Delay On", ("setDelay", "1"))
		.AddButton("Delay Off", ("setDelay", "0"))
		.AddButton("Reverb On", ("setReverb", "1"))
		.AddButton("Reverb Off", ("setReverb", "0"))
		.AddButton("Cab Sim On", ("setCabSim", "1"))
		.AddButton("Cab Sim Off", ("setCabSim", "0"))
		.Dump();

		new SliderGroup("Input Mixer", "setInputMixer")
				.AddGainSlider("Instrument", 1.0f, 0.0f, 2.0f)
				.AddGainSlider("Microphone", 0.0f, 0.0f, 2.0f)
				.Dump();

		new SliderGroup("Preamp", "setPreampEq")
				.AddSlider("Enable", 0, 1, 1)
				.AddFreqSlider("Highpass freq", 100.0f)
				.AddFreqSlider("Lowshelf freq", 1000.0f)
				.AddDbSlider("Lowshelf gain", -6.0f)
				.AddFreqSlider("LowPass freq", 8500.0f)
				.AddQSlider("Low Pass Q")
				.Dump();

		new SliderGroup("Dynamics: Gate", "setGate")
				.AddSlider("Enable", 0, 1, 1)
				.AddDbSlider("Threshold", -75.0f, -126.0f, 0.0f)
				.AddTimeSlider("Attack", 0.0001f)
				.AddTimeSlider("Release", 3.0f)
				.AddDbSlider("Attenuation", -14.0f, -126.0f, 0.0f)			
				.Dump();

		new SliderGroup("Dynamics: Compression", "setCompression")
				.AddSlider("Enable", 0, 1, 1)
				.AddDbSlider("Threshold", -50.0f, -126.0f, 0.0f)
				.AddTimeSlider("Attack", 0.001f, 0.5f)
				.AddTimeSlider("Release", 2.5f)
				.AddSlider("Ratio", 1.0f, 20.0f, 1.5f)
				.AddDbSlider("Manual Makeup Gain", 6.0f)
				.Dump();

		new SliderGroup("Dynamics: Limiter", "setLimiter")
				.AddSlider("Enable", 0, 1, 1)
				.AddDbSlider("Threshold", 0.0f, -126.0f, 0.0f)
				.Dump();

		new SliderGroup("VCF (Wah/Filter)", "setAutoWah")
				.AddSlider("Enable", 0, 1, 0)
				.AddSlider("Sensitivity", -50.0f, 50.0f, 9.0f)
				.AddFreqSlider("Center Frequency", 250.0f)
				.AddSlider("Resonance", 0.5f, 5.0f, 3.5f)
				.Dump();

		new SliderGroup("Distortion", "setDistortion")
			.AddSlider("Enable", 0, 1, 0)
			.AddSlider("Gain", 0.0f, 500.0f, 1.0f)
			.AddGainSlider("Curve", 0.0f)
			.AddGainSlider("Level", 1.0f)			
			.Dump();

		new SliderGroup("Tone Stack", "setToneStack")
			.AddSlider("Enable", 0, 1, 1)
			.AddDbSlider("Bass", 0.0f, -12.0f, 6.0f)
			.AddDbSlider("Middle", 0.0f, -12.0f, 6.0f)
			.AddDbSlider("Treble", 0.0f, -12.0f, 6.0f)
			.AddFreqSlider("Lowpass freq", 8000.0f)
			.Dump();

		new SliderGroup("VCA (Tremolo)", "setTremolo")
			.AddSlider("Enable", 0, 1, 0)
			.AddSlider("Rate", 0.1f, 100.0f, 3.5f)
			.AddGainSlider("Depth", 0.3f, 0.0f, 1.0f)
			.Dump();

		new SliderGroup("Chorus", "setChorus")
			.AddSlider("Enable", 0, 1, 1)
			.AddSlider("Rate", 0.2f, 8.0f, 1.7f)
			.AddGainSlider("Depth", 0.032f, 0.0f, 0.5f)
			.AddGainSlider("Mix", 0.6f)
			.AddGainSlider("Resonance", 0.5f, 0.0f, 0.9f)
			.AddSlider("Wave (triangle/sine)", 0, 1, 0)
			.Dump();

		new SliderGroup("Reverb", "setReverb")
			.AddSlider("Enable", 0, 1, 1)
			.AddSlider("Pre delay (msec)", 0.0f, 100.0f, 90.0f)
			.AddSlider("Room size", 0.0f, 1.0f, 0.75f)
			.AddSlider("Damping", 0.0f, 1.0f, 0.12f)
			.AddGainSlider("Mix", 0.04f)
			.Dump();

		new SliderGroup("Delay", "setDelay")
			.AddSlider("Enable", 0, 1, 0)
			.AddSlider("Delay (msec)", 0.0f, 800.0f, 375.0f)
			.AddGainSlider("Feedback", 0.25f)
			.AddFreqSlider("Low Pass freq", 6500.0f)
			.AddGainSlider("Mix", 0.05f)
			.Dump();

		new SliderGroup("Cab Simulator", "setCabSim")
			.AddSlider("Enable", 0, 1, 1)
			.AddFreqSlider("Scoop freq", 1300.0f)
			.AddDbSlider("Scoop gain", -15.0f, -24.0f, 6.0f)
			.Dump();
			
		new SliderGroup("Volume", "setVolume")
			.AddGainSlider("Volume", 1.0f, 0.0f, 2.0f)
			.Dump();

/*
	Environment.CurrentDirectory = @"C:\Users\T948384\Documents\Arduino\X100";
	var mainInoLines = File.ReadAllLines("x100.ino").ToList();
	var audioObjects = mainInoLines.Where(l => l.Contains("//xy=") && !l.Contains("AudioConnection"))
							.Select(l => l.Split(' ').Where(c => !string.IsNullOrEmpty(c)).Skip(1).First().TrimEnd(';'))
							.ToList();

	var defaults = audioObjects.ToDictionary(o => o, o => mainInoLines.Where(l => l.Contains(o + ".")).Select(c => c.Trim().Substring(o.Length + 1))).Where(kvp => kvp.Value.Any()) ;
	
	
	
	var currentHandler = File.ReadAllLines("commandHandler.ino").ToList();
	
	SliderGroup sg = null;
	foreach (var line in currentHandler)
	{
		if (line.Contains("strncmp"))
		{
			if (sg != null) sg.Dump();
			
			var function = line.Substring(line.IndexOf("strncmp")).Split('"')[1];
			sg = new SliderGroup(function, function);
		}
	}
	*/

}