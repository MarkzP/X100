<Query Kind="Program">
  <Namespace>LINQPad.Controls</Namespace>
  <Namespace>LINQPad.Controls.Core</Namespace>
</Query>

class SliderGroup
{
	abstract class SliderDef
	{
		public string name { get; protected set; }
		public Span span { get; protected set; }
		public Control control { get; protected set; }
		public SliderGroup group { get; protected set; }
		
		protected SliderDef(SliderGroup g)
		{
			group = g;
		}
		
		public abstract void Reset();

		public void Update()
		{
			span.Text = $"{name} {this}";
		}

		protected void queueValue(object sender, EventArgs e)
		{
			this.Update();
			lock (queueLock)
			{
				sendQueue[$"{group.channel}.{this.name}"] = this.ToString();
				sendTimer.Change(50, Timeout.Infinite);
			}
		}
	}

	class SliderFloat : SliderDef
	{
		const float mult = 1000.0f;

		RangeControl range;
		int def;
		float min;
		float max;
		float step;
		
		float stepToVal(int s) => min + ((float)s * step);
		int valToStep(float v) => (int)Math.Round((v - min) / step);

		public SliderFloat(SliderGroup group, string name, float min, float max, float val, float def, float step): base(group)
		{
			base.name = name;
			this.min = min;
			this.max = max;
			this.step = step;
			
			this.range = new RangeControl(valToStep(min), valToStep(max), valToStep(val));
			this.def = valToStep(def);
			this.range.Tag = this;
			this.range.Width = "90%";
			this.range.ValueInput += queueValue;			

			span = new Span($"{name} {this}");
			control = range;
		}

		public override string ToString() => stepToVal(range.Value).ToString("F4", System.Globalization.CultureInfo.InvariantCulture);

		public override void Reset()
		{
			range.ValueInput -= queueValue;	
			range.Value = def;
			Update();
			range.ValueInput += queueValue;	
		}
	}

	class SliderFreq : SliderDef
	{
		private float[] fbands = { 50.0f,   56.0f,   63.0f,   71.0f,   80.0f,   89.0f,   100.0f,   112.0f,   125.0f,   140.0f,   158.0f,   175.0f,   200.0f,   225.0f,   250.0f,   280.0f,   315.0f,   400.0f,   440.0f,
						 500.0f,  560.0f,  630.0f,  710.0f,  800.0f,  890.0f,  1000.0f,  1120.0f,  1250.0f,  1400.0f,  1580.0f,  1750.0f,  2000.0f,  2250.0f,  2500.0f,  2800.0f,  3150.0f,  4000.0f,  4400.0f,
						 5000.0f, 5600.0f, 6300.0f, 7100.0f, 8000.0f, 8900.0f, 10000.0f, 11200.0f, 12500.0f, 14000.0f, 15800.0f, 17500.0f, 19000.0f
					   };
					   
		private int freqToBand(float freq)
		{
			int band = 0;
			for (; band < fbands.Length; band++)
			{
				if (fbands[band] >= freq) break;
			}
			return band;
		}

		private float bandToFreq(int band) => fbands[band];

		RangeControl range;
		int def;

		public SliderFreq(SliderGroup group, string name, float min, float max, float value, float def): base(group)
		{
			base.name = name;
			this.range = new RangeControl(freqToBand(min), freqToBand(max), freqToBand(def));
			this.def = range.Value;
			this.range.Value = freqToBand(value);
			this.range.Tag = this;
			this.range.Width = "90%";
			this.range.ValueInput += queueValue;		

			span = new Span($"{name} {this}");
			control = range;
		}

		public override string ToString() => bandToFreq(range.Value).ToString("F1", System.Globalization.CultureInfo.InvariantCulture);

		public override void Reset()
		{
			range.ValueInput -= queueValue;
			range.Value = def;
			Update();
			range.ValueInput += queueValue;
		}
	}

	class SliderInt : SliderDef
	{
		RangeControl range;
		int def;

		public SliderInt(SliderGroup group, string name, int min, int max, int val, int def): base(group)
		{
			base.name = name;
			this.def = def;
			this.range = new RangeControl(min, max, val);
			this.range.Tag = this;
			this.range.Width = "90%";
			this.range.ValueInput += queueValue;

			span = new Span($"{name} {def}");
			control = range;
		}

		public override string ToString() => range.Value.ToString();

		public override void Reset()
		{
			range.ValueInput -= queueValue;
			range.Value = def;
			Update();
			range.ValueInput += queueValue;
		}
	}

	static string portName = null;
	static Dictionary<string, string> sendQueue = new Dictionary<string, string>();
	static object queueLock = new object();
	static object portLock = new object();
	static Timer sendTimer = new Timer(processQueue, null, Timeout.Infinite, Timeout.Infinite);

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

		SendReceive(msgs);			
	}
	
    enum ParameterType
    {
      PT_Float = 0,
      PT_Bool,
      PT_Freq,
	  PT_Enum,
	  PT_Coeff,
	}

	static readonly Dictionary<string, SliderGroup> Groups = new Dictionary<string, UserQuery.SliderGroup>();

	static readonly TextBox tbCom = new TextBox();
	static readonly TextBox tbPreset = new TextBox();

	public static bool DetectPort()
	{
		lock (portLock)
		{
			if (!string.IsNullOrEmpty(portName)) return true;

			foreach (var testPort in System.IO.Ports.SerialPort.GetPortNames())
			{
				using (var port = new System.IO.Ports.SerialPort(testPort))
				{
					try
					{
						port.Open();
						port.ReadTimeout = 200;
						port.ReadExisting();
						port.Write("stats();");
						Thread.Sleep(200);
						var report = port.ReadExisting();
						if (report.Contains("32="))
						{
							portName = testPort;
							return true;
						}
					}
					catch (IOException)
					{
						continue;
					}
					catch (UnauthorizedAccessException)
					{
						continue;
					}			
				}
			}
		}
		return false;
	}

	public static string SendReceive(string msg)
	{
		if (!DetectPort()) return string.Empty;
		lock (portLock)
		{
			using (var port = new System.IO.Ports.SerialPort(portName))
			{
				port.Open();
				port.WriteTimeout = 25;
				port.ReadTimeout = 25;
				port.Write(msg);
				Thread.Sleep(100);
				return port.ReadExisting();
			}
		}
	}

	public static void Init()
	{
		if (!DetectPort()) return;

		Util.ClearResults();	

		SliderGroup actions = new SliderGroup("Actions", null, true);
		actions.AddButton("printLevels(1);");
		actions.AddButton("printLevels(0);");
		actions.AddButton("reboot();");
		actions.AddButton("stats();");
		actions.AddControl(new Label(portName));
		actions.AddControl(tbCom);
		actions.Dump();
		
		
		SliderGroup tones = new SliderGroup("Input Tones", null, true);
		tones.AddButton("Mute", "tone(0,0);");
		tones.AddButton("25", "tone(25,1);");
		tones.AddButton("50", "tone(50,1);");
		tones.AddButton("100", "tone(100,1);");
		tones.AddButton("300", "tone(300,1);");
		tones.AddButton("500", "tone(500,1);");
		tones.AddButton("1.0k", "tone(1000,1);");
		tones.AddButton("2.0k", "tone(2000,1);");
		tones.AddButton("3.0k", "tone(3000,1);");
		tones.AddButton("4.9k", "tone(4900,1);");
		tones.AddButton("5.0k", "tone(5000,1);");
		tones.AddButton("5.1k", "tone(5100,1);");
		tones.AddButton("5.5k", "tone(5500,1);");
		tones.AddButton("10k", "tone(10000,1);");
		tones.AddButton("15k", "tone(15000,1);");
		tones.Dump();
		
		var listPresets = SendReceive("listPresets();");
		SliderGroup presets = new SliderGroup("Presets", null, true);
		foreach (var presetName in listPresets.Split('\n').Select(r => r.Trim()).Where(r => !string.IsNullOrWhiteSpace(r) && !r.EndsWith(".bak") && !r.StartsWith("System")))
		{
			var btn = new Button(presetName);
			btn.Click += (o, e) =>
			{
				SendReceive($"loadPreset(\"{presetName}\");");
				Init();
				tbPreset.Text = presetName;
			};
			presets.AddControl(btn);
		}
		var saveBtn = new Button("Save");
		saveBtn.Click += (o, e) =>
		{
			SendReceive($"savePreset(\"{tbPreset.Text}\");");
		};
		
		presets.AddControl(new Label("Preset name:"));			
		presets.AddControl(tbPreset);
		presets.AddControl(saveBtn);
		
		presets.Dump();
		
		var dumpAll = SendReceive("dumpAll();");
		
		SliderGroup group = null;
		foreach (var line in dumpAll.Split('\n').Select(r => r.Trim()).Where(r => !string.IsNullOrWhiteSpace(r)))
		{
			//line.Dump();

			if (line.Contains("[") && line.Contains("]"))
			{
				if (group != null) group.Dump();
				var groupName = line.TrimEnd(']').TrimStart('[');
				group = new SliderGroup(groupName, groupName);
				Groups[groupName] = group;
				
			}
			else if (line.Contains("="))
			{
				var parts = line.Split('=');
				var paramName = parts.First();
				if (paramName.Contains("."))
				{
					var groupName = paramName.Split('.').First();
					if (!Groups.TryGetValue(groupName, out group))
					{
						group = new SliderGroup(groupName, groupName);
						Groups[groupName] = group;
					}
					paramName = paramName.Split('.').Skip(1).First();
				}
				
				var paramParts = parts.Skip(1).First().TrimEnd(';', '\r', '\n', ' ').Split(',');
				
				var paramType = (ParameterType)int.Parse(paramParts.First());
				var paramValues = paramParts.Skip(1).Select(p => float.Parse(p, System.Globalization.CultureInfo.InvariantCulture)).ToArray();

				switch (paramType)
				{
					case ParameterType.PT_Float:
						group.AddFloatSlider(paramName, paramValues[2], paramValues[3], paramValues[0], paramValues[1], paramValues[4]);
						break;
					case ParameterType.PT_Coeff:
						group.AddFloatSlider(paramName, paramValues[2], paramValues[3], paramValues[0], paramValues[1], paramValues[4]);
						break;
					case ParameterType.PT_Enum:
					case ParameterType.PT_Bool:
						group.AddIntSlider(paramName, (int)paramValues[2], (int)paramValues[3], (int)paramValues[0], (int)paramValues[1]);
						break;
					case ParameterType.PT_Freq:
						group.AddFreqSlider(paramName, paramValues[0], paramValues[1], paramValues[2], paramValues[3]);
						break;
				}
			}
		}
		if (group != null) group.Dump();
	}

	string name;
	string channel;
	StackPanel sp;

	IEnumerable<SliderDef> sliders => sp.Children
			.Where(o => o is Control c && c.Tag is SliderDef)
			.Select(o => (o as Control).Tag as SliderDef);
	
	
	public SliderGroup(string name, string channel = null, bool horizontal = false)
	{
		this.name = name;
		this.channel = channel;
		sp = new StackPanel(false, ".1em");
		sp.Horizontal = horizontal;

		if (!string.IsNullOrEmpty(channel))
		{
			//var resetBtn = new Button("Reset");
			//resetBtn.Click += (o, e) =>
			//{
			//	SendReceive($"{channel}.reset");
			//	sliders.ToList().ForEach(s => s.Reset());
			//};
			//sp.Children.Add(resetBtn);
			
			//var printBtn = new Button("Print");
			//printBtn.Click += (o, e) =>
			//{
			//	foreach(var slider in sliders)
			//	{
			//		$"{slider.name}.change({slider}f);".Dump();
			//	}
			//};
			//sp.Children.Add(printBtn);
		}
		
	}
	
	public SliderGroup AddButton(string name, string command = null)
	{
		if (string.IsNullOrEmpty(name)) return this;
		
		var btn = new Button(name);
		btn.Click += (o, e) =>
		{
			tbCom.Text = SendReceive(command ?? name);
		};
		sp.Children.Add(btn);		
		
		return this;
	}

	public SliderGroup AddFloatSlider(string name, float min, float max, float value, float def, float step)
	{
		var slider = new SliderFloat(this, name, min, max, value, def, step);

		sp.Children.Add(slider.span);
		sp.Children.Add(slider.control);
		return this;
	}

	public SliderGroup AddIntSlider(string name, int min, int max, int value, int def)
	{
		var slider = new SliderInt(this, name, min, max, value, def);

		sp.Children.Add(slider.span);
		sp.Children.Add(slider.control);
		return this;
	}

	public SliderGroup AddFreqSlider(string name, float value, float def, float min = 20.0f, float max = 20000.0f)
	{
		var slider = new SliderFreq(this, name, min, max, value, def);

		sp.Children.Add(slider.span);
		sp.Children.Add(slider.control);
		return this;
	}
	
	public SliderGroup AddControl(Control c)
	{
		sp.Children.Add(c);
		return this;
	}
	
	public FieldSet Dump() => new FieldSet(name, sp).Dump();
}

void Main()
{
	SliderGroup.Init();
}