<Query Kind="Program" />

void Main()
{
	var bins = new int[0];
    var freqs = new int[0];
	var levelIn = new float[0];

	if (1 == 1)
	{
		var amplitude = -0.615f;
		bins = Enumerable.Range(20, 78).ToArray();
		freqs = bins.Select(i => logFreq(i)).ToArray();
		levelIn = Enumerable.Range(0, bins.Length).Select(i => amplitude).ToArray();
	}
	else
	{
		var freq = 1500;
		var ampMin = -76.0f;
		var ampMax = -3.0f;
		var range = 80;
		bins = Enumerable.Range(-range, range + 1).ToArray();
		freqs = bins.Select(e => freq).ToArray();
		levelIn = bins.Select(i => ((i + 5) / 20) % 2 == 0 ? ampMin : ampMax).ToArray();
	}
	
	
	var levelOutsL = new float[levelIn.Length];
	var levelOutsR = new float[levelIn.Length];
	
	
	using (var port = new System.IO.Ports.SerialPort("COM10"))
	{
		port.Open();
		port.ReadTimeout = 500;
		
		port.Write("setVolume(0);setInputMixer(0,0);");
		
		for (int i = 0; i < levelIn.Length && i < freqs.Length; i++)
		{
			if (i == 0)
			{
				levelOutsL[i] = -80.0f;
				levelOutsR[i] = -80.0f;
				continue;
			}
			
			//Thread.Sleep(5);
			var msg = $"doTestTone({freqs[i]},{dbToUnit(levelIn[i]).ToString(System.Globalization.CultureInfo.InvariantCulture)});";
			port.Write(msg);
			var resp = port.ReadLine();
			var parts = resp.Replace("nan", "NaN").Split(',').Select(s => s.Trim());
			levelOutsL[i] = unitToDb(float.Parse(parts.First(), System.Globalization.CultureInfo.InvariantCulture));
			levelOutsR[i] = unitToDb(float.Parse(parts.Last(), System.Globalization.CultureInfo.InvariantCulture));

			Console.WriteLine($"f={freqs[i]},i={dbToUnit(levelIn[i])} => {levelOutsL[i]}, {levelOutsR[i]}");
		}
		
		port.Write("setVolume();setInputMixer();");
	}

	bins.Chart()
		.AddYSeries(levelIn, Util.SeriesType.Line, "db In")
		.AddYSeries(levelOutsL, Util.SeriesType.Line, "db L")
		.AddYSeries(levelOutsR, Util.SeriesType.Line, "db R")
		.AddYSeries(freqs, Util.SeriesType.Line, "Freq", true)
		.Dump();
}

// Define other methods and classes here
float dbToUnit(float db) => (float)Math.Pow(10.0f, db / 20.0f);

float unitToDb(float unit) => unit < 5.011872E-07f ? -126.0f : 20.0f * (float)Math.Log10(unit);

int logFreq(int i) => (int)Math.Round(10.0 * Math.Pow(1.079, (double)i));