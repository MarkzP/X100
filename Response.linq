<Query Kind="Program">
  <Reference>&lt;RuntimeDirectory&gt;\System.Windows.Forms.DataVisualization.dll</Reference>
  <Namespace>System.Drawing</Namespace>
</Query>

void Main()
{
	var bins = new int[0];
    var freqs = new int[0];
	var levelIn = new float[0];

	var amplitude = 0.0f;//-0.615f;
	bins = Enumerable.Range(26, 175).ToArray();
	freqs = bins.Select(i => logFreq(i)).ToArray();
	levelIn = Enumerable.Range(0, bins.Length).Select(i => amplitude).ToArray();
	
	var levelOutsL = new float[levelIn.Length];
	var levelOutsR = new float[levelIn.Length];

	foreach (var testPort in System.IO.Ports.SerialPort.GetPortNames())
	{
		using (var port = new System.IO.Ports.SerialPort(testPort))
		{
			try
			{
				port.Open();
			}
			catch (IOException)
			{
				continue;
			}
			catch (UnauthorizedAccessException)
			{
				continue;
			}
			port.ReadTimeout = 1000;
			port.Write("stats();");
			Thread.Sleep(20);
			var report = port.ReadExisting();
			if (report.Contains("Proc"))
			{
				port.Write("setVolume(0);setInput(0);printLevels(0);printTuner(0);");
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

					var msg = $"doTestTone({freqs[i]},{dbToUnit(levelIn[i]).ToString(System.Globalization.CultureInfo.InvariantCulture)});";
					port.Write(msg);

					var resp = port.ReadLine();					
					try
					{
						var parts = resp.Replace("nan", "NaN").Replace("-NaN", "NaN").Split(',').Select(s => s.Trim());
						levelOutsL[i] = unitToDb(float.Parse(parts.First(), System.Globalization.CultureInfo.InvariantCulture));
						levelOutsR[i] = unitToDb(float.Parse(parts.Last(), System.Globalization.CultureInfo.InvariantCulture));
					}
					catch
					{
						resp.Dump();
						throw;
					}
					Console.WriteLine($"f={freqs[i]},i={levelIn[i]} => {levelOutsL[i]}, {levelOutsR[i]}");
				}

				port.Write("setVolume(1);setInput(1);");
			}
		}
	}

	var chart = Enumerable.Range(0, bins.Length).Chart(x => freqs[x])
		.AddYSeries(x => levelOutsL[x], LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => levelOutsR[x], LINQPad.Util.SeriesType.Spline)
		.ToWindowsChart();
		
	var area = chart.ChartAreas.First();
	var xaxis = area.AxisX;

	xaxis.Minimum = 100.0;
	xaxis.Maximum = 20000.0;
	xaxis.RoundAxisValues();
	
	xaxis.IsLogarithmic = true;
	xaxis.MajorGrid.Enabled = true;
	xaxis.MajorGrid.LineColor = Color.Gray;
	xaxis.MinorGrid.Enabled = true;
	xaxis.MinorGrid.LineColor = Color.LightGray;
	xaxis.MinorGrid.Interval = 1.0;

	var yaxis = area.AxisY;
	yaxis.Minimum = -72.0;
	yaxis.Maximum = 15.0;
	yaxis.Interval = 6.0;
	
	area.AxisY2.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
	
	chart.Dump();

}

// Define other methods and classes here
float dbToUnit(float db) => (float)Math.Pow(10.0f, db / 20.0f);

float unitToDb(float unit) => unit < 5.011872E-07f ? -126.0f : 20.0f * (float)Math.Log10(unit);

int logFreq(int i) => (int)Math.Round(19.61526778504022638394310269766 * Math.Pow(1.0352649238413775043477881942112, (double)i));