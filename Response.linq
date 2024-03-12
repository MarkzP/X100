<Query Kind="Program">
  <Reference>&lt;RuntimeDirectory&gt;\System.Windows.Forms.DataVisualization.dll</Reference>
  <Namespace>System.Drawing</Namespace>
</Query>

void Main()
{
	var bins = new int[0];
    var freqs = new int[0];
	var levelIn = new float[0];

	if (1 == 1)
	{
		var amplitude = 0.0f;//-0.615f;
		bins = Enumerable.Range(20, 80).ToArray();
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

	foreach (var testPort in System.IO.Ports.SerialPort.GetPortNames())
	{
		using (var port = new System.IO.Ports.SerialPort(testPort))
		{
			port.Open();
			port.ReadTimeout = 500;
			port.Write("stats();");
			Thread.Sleep(20);
			var report = port.ReadExisting();
			if (report.Contains("Proc"))
			{
				port.Write("setVolume(0);setInputMixer(0,0);");
				Thread.Sleep(20);
				port.ReadExisting();

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

					Console.WriteLine($"f={freqs[i]},i={levelIn[i]} => {levelOutsL[i]}, {levelOutsR[i]}");
				}

				port.Write("setVolume(1);");
			}
		}
	}

	var chart = Enumerable.Range(10, bins.Length - 10).Chart(x => freqs[x])
		.AddYSeries(x => levelOutsL[x], LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => levelOutsR[x], LINQPad.Util.SeriesType.Spline)
		.ToWindowsChart();
		
	var area = chart.ChartAreas.First();
	var xaxis = area.AxisX;
	xaxis.IsLogarithmic = true;
	xaxis.MinorGrid.Enabled = true;
	xaxis.MinorGrid.LineColor = Color.LightGray;
	xaxis.Minimum = 100.0;

	var yaxis = area.AxisY;
	//yaxis.Minimum = levelOutsL.Skip(5).Min();
	//yaxis.Maximum = 12.0;
	yaxis.Minimum = -50.0;
	yaxis.Maximum = 5.0;
	yaxis.Interval = 5.0;
	
	area.AxisY2.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
	
	chart.Dump();

}

// Define other methods and classes here
float dbToUnit(float db) => (float)Math.Pow(10.0f, db / 20.0f);

float unitToDb(float unit) => unit < 5.011872E-07f ? -126.0f : 20.0f * (float)Math.Log10(unit);

int logFreq(int i) => (int)Math.Round(10.0 * Math.Pow(1.079, (double)i));