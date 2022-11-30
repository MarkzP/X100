<Query Kind="Program">
  <Namespace>LINQPad.Controls</Namespace>
</Query>

class SliderGroup
{
	class SliderDef
	{
		public string name { get; }
		public float value { get; }
		public Span span { get; }
		public bool log { get; }

		public void Update(double value)
		{
			span.Text = $"{name} {value:F4}";
		}

		public SliderDef(string name, float value)
		{
			this.name = name;
			this.value = value;
			this.log = false;
			span = new Span($"{name} {value:F4}");
		}	
	}
	
	static Dictionary<string, string> lastSend = new Dictionary<string, string>();
	static Dictionary<string, string> sendQueue = new Dictionary<string, string>();
	static object queueLock = new object();
	static object portLock = new object();
	static Timer sendTimer = new Timer(processQueue, null, Timeout.Infinite, Timeout.Infinite);

	const int rmax = 290;
	const double flogscale = 10.0;
	const double fscales = 3 - 0.3;
	const double fmax = 16000.0;
	static double fd = (double)rmax / fscales;
	static double f0 = fmax / Math.Pow(flogscale, fscales);
	static double fa = Math.Pow(flogscale, (1.0 / fd));

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
				using (var port = new System.IO.Ports.SerialPort("COM6"))
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
	const float mult = 10000.0f;

	public SliderGroup(string name, string channel)
	{
		var resetBtn = new Button("Reset");
		resetBtn.Click += (o, e) =>
		{
			var values = sp.Children.Where(c => c is RangeControl).Select(c =>
			{
				var irc = c as RangeControl;
				var idf = irc.Tag as SliderDef;
				irc.Value = (int)(idf.value * mult);
				idf.Update(idf.value);
				return idf.value.ToString(System.Globalization.CultureInfo.InvariantCulture);;
			});
			lock (queueLock)
			{
				sendQueue[channel] = string.Join(",", values);
				sendTimer.Change(20, Timeout.Infinite);
			}
		};
		
		this.name = name;
		this.channel = channel;		
		sp = new StackPanel(false, ".1em", resetBtn);
	}
	
	public SliderGroup AddSlider(string name, float min, float max, float value)
	{
		var rc = new RangeControl((int)(min * mult), (int)(max * mult), (int)(value * mult));
		var def = new SliderDef(name, value); 
		rc.Width = "90%";
		rc.Tag = def;
		rc.ValueInput += (o, e) => 
		{
			var values = sp.Children
							.Where(c => c is RangeControl)
							.Select(c =>
							{
								var irc = c as RangeControl;
								var idf = irc.Tag as SliderDef;
								var ivl = idf.log ? f0 * (Math.Pow(fa, (double)irc.Value)) : (float)irc.Value / mult;
								idf.Update(ivl);
								return ivl.ToString(System.Globalization.CultureInfo.InvariantCulture);
							});
							
			lock (queueLock)
			{
				sendQueue[channel] = string.Join(",", values);
				sendTimer.Change(20, Timeout.Infinite);
			}
		};

		sp.Children.Add(def.span);
		sp.Children.Add(rc);
		return this;
	}
	
	public SliderGroup AddFreqSlider(string name, float value, float min = 1.0f, float max = 22000.0f) => AddSlider(name, min, max, value);
	public SliderGroup AddDbSlider(string name, float value, float min = -96.0f, float max = 96.0f) => AddSlider(name, min, max, value);
	public SliderGroup AddEqSlider(float freq, float value, float min = -36.0f, float max = 12.0f) => AddSlider($"{freq} Hz", min, max, value);
	public SliderGroup AddGainSlider(string name, float value, float min = 0.0f, float max = 1.0f) => AddSlider(name, min, max, value);
	public SliderGroup AddQSlider(string name, float value = 0.7071f) => AddSlider(name, 0.001f, 15.0f, value);
	public SliderGroup AddSlopeSlider(string name, float value) => AddSlider(name, 0.001f, 15.0f, value);
	public SliderGroup AddTimeSlider(string name, float value, float max = 10.0f) => AddSlider(name, 0.0f, max, value);	
	
	public FieldSet Dump() => new FieldSet(name, sp).Dump();
}

void Main()
{
	new SliderGroup("preEq", "preEq")
			.AddFreqSlider("High Pass freq", 200.0f)
			.AddQSlider("High Pass Q")
			.AddFreqSlider("Low Shelf freq", 1000.0f)
			.AddDbSlider("Low Shelf gain", -18.0f)
			.AddSlopeSlider("Low Shelf slope", 0.2f)
			.AddFreqSlider("Low Pass freq", 2000.0f)
			.AddQSlider("Low Pass Q", 1.7f)
			.Dump();
	
	new SliderGroup("Gate", "dynamics.gate")
			.AddDbSlider("Threshold", -75.0f, -126.0f, 0.0f)
			.AddTimeSlider("Attack", 0.0f, 0.5f)
			.AddTimeSlider("Release", 3.0f)
			.AddDbSlider("Hysterisis", 6.0f, 0.0f, 12.0f)
			.AddDbSlider("Attenuation", -14.0f, -126.0f, 0.0f)			
			.Dump();

	new SliderGroup("Compression", "dynamics.compression")
			.AddDbSlider("Threshold", -55.0f, -126.0f, 0.0f)
			.AddTimeSlider("Attack", 0.0015f, 0.5f)
			.AddTimeSlider("Release", 2.5f)
			.AddSlider("Ratio", 1.0f, 60.0f, 4.5f)
			.AddSlider("Knee width", 0.0f, 18.0f, 3.0f)
			.Dump();

	new SliderGroup("Distortion", "distortion")
			.AddSlider("Pre Gain", 0.0f, 50.0f, 1.0f)
			.AddSlider("Curve", 0.0f, 25.0f, 1.0f)
			.AddSlider("Post Gain", 0.0f, 50.0f, 1.0f)
			.Dump();

	new SliderGroup("Post Eq", "postEq")
			.AddEqSlider(100.0f,   -6.0f)
			.AddEqSlider(200.0f,   -3.0f)
			.AddEqSlider(400.0f,   -1.5f)
			.AddEqSlider(800.0f,   -9.0f)
			.AddEqSlider(1600.0f, -12.0f)
			.AddEqSlider(3200.0f, 1.5f)
			.AddEqSlider(6400.0f, -9.0f)
			.Dump();

	new SliderGroup("Chorus Modulation", "chorusMod")
			.AddGainSlider("Depth", 0.032f, 0.0f, 0.1f)
			.AddSlider("Rate", 0.1f, 10.0f, 1.7f)
			.Dump();

	new SliderGroup("Chorus Post Filter", "chorusPostFilter")
			.AddFreqSlider("Low Pass freq", 8000.0f)
			.AddQSlider("Low Pass Q", 0.5f)
			.Dump();
			
	new SliderGroup("Chorus Post Delay", "chorusPostDelay")
			.AddSlider("Millisec", 0.0f, 50.0f, 17.0f) 
			.Dump();

	new SliderGroup("Reverb Pre Delay", "chorusPostDelay")
			.AddSlider("Millisec", 0.0f, 50.0f, 17.0f)
			.Dump();

	new SliderGroup("Reverb Mixer", "reverbMixer")
			.AddGainSlider("Instrument", 1.0f)
			.AddGainSlider("Mike", 0.0f)
			.Dump();

	new SliderGroup("Reverb", "reverb")
			.AddSlider("Room size", 0.0f, 1.0f, 0.9f)
			.AddSlider("Damping", 0.0f, 1.0f, 0.001f)
			.Dump();

	new SliderGroup("Delay Mixer", "delayMixer")
			.AddGainSlider("Instrument", 0.7f)
			.AddGainSlider("Mike", 0.0f)
			.AddGainSlider("Feedback", 0.3f)
			.Dump();

	new SliderGroup("Delay", "delayLine")
			.AddSlider("Millisec", 0.0f, 500.0f, 400.0f)
			.Dump();

	new SliderGroup("Delay Filter", "delayFilter")
			.AddFreqSlider("High Pass freq", 500.0f)
			.AddQSlider("High Pass Q")
			.AddFreqSlider("Low Pass freq", 3000.0f)
			.AddQSlider("Low Pass Q")
			.Dump();

	new SliderGroup("Mixer LR", "mixerLR")
			.AddGainSlider("Instrument Mix", 1.0f)
			.AddGainSlider("Chorus Mix", 0.750f)
			.AddGainSlider("Reverb Mix", 0.018f)
			.AddGainSlider("Delay Mix", 0.0f)
			.Dump();
}