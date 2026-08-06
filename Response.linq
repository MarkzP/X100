<Query Kind="Program">
  <Reference>&lt;RuntimeDirectory&gt;\System.Windows.Forms.DataVisualization.dll</Reference>
  <Namespace>System.Drawing</Namespace>
</Query>

void Main()
{
	var bins = new int[0];
    var freqs = new float[0];
	var levelIn = new float[0];

	var amplitude = -3.0f;//-0.615f;
	bins = Enumerable.Range(0, 151).ToArray();
	freqs = bins.Select(i => logFreq(i)).ToArray();
	levelIn = Enumerable.Range(0, freqs.Length).Select(i => amplitude).ToArray();
	
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
			port.ReadTimeout = 500;
			port.Write("stats();");
			Thread.Sleep(100);
			var report = port.ReadExisting();
			if (report.Contains("32="))
			{
				//port.Dump();
				port.ReadTimeout = 5000;
				port.Write("setVolume(0);");
				port.Write("printLevels(0);printTuner(0);");
				Thread.Sleep(100);
				port.ReadExisting();

				for (int i = 0; i < levelIn.Length && i < freqs.Length; i++)
				{
					levelOutsL[i] = -96.0f;
					levelOutsR[i] = -96.0f;
					var msg = $"doTestTone({freqs[i].ToString(System.Globalization.CultureInfo.InvariantCulture)},{levelIn[i].ToString("F2", System.Globalization.CultureInfo.InvariantCulture)});";
					//msg.Dump();
					for (int j = 0; j < 1; j++)
					{
						port.Write(msg);

						var resp = port.ReadLine();
						
						if (i == 0)
						{
							port.Write(msg);
							resp = port.ReadLine();
						}
						//resp.Dump();
						try
						{
							var parts = resp.Replace("nan", "NaN").Replace("-NaN", "NaN").Split(',').Select(s => s.Trim());
							//parts.Dump();
							levelOutsL[i] = Math.Max(levelOutsL[i], float.Parse(parts.First(), System.Globalization.CultureInfo.InvariantCulture) - levelIn[i]);
							levelOutsR[i] = Math.Max(levelOutsR[i], float.Parse(parts.Skip(1).First(), System.Globalization.CultureInfo.InvariantCulture) - levelIn[i]);
						}
						catch
						{
							resp.Dump();
							throw;
						}
					}
					Console.WriteLine($"i={i}: f={freqs[i]:F1}, dB={levelIn[i]:F1} => {levelOutsL[i]:F1}, {levelOutsR[i]:F1}");
				}
				Thread.Sleep(500);
				port.Write("setVolume(0);");
			}
		}
	}

	var chart = Enumerable.Range(0, freqs.Length).Chart(x => freqs[x])
		.AddYSeries(x => levelOutsL[x], LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => levelOutsR[x], LINQPad.Util.SeriesType.Spline)
		.ToWindowsChart();
		
	var area = chart.ChartAreas.First();
	var xaxis = area.AxisX;

	xaxis.Minimum = 10.0;
	xaxis.Maximum = 22050.0;
	xaxis.RoundAxisValues();
	
	xaxis.IsLogarithmic = true;
	xaxis.MajorGrid.Enabled = true;
	xaxis.MajorGrid.LineColor = Color.Gray;
	xaxis.MinorGrid.Enabled = true;
	xaxis.MinorGrid.LineColor = Color.LightGray;
	xaxis.MinorGrid.Interval = 1.0;
	
	var yaxis = area.AxisY;
	yaxis.Minimum = -72.0;
	yaxis.Maximum = 12.0;
	yaxis.Interval = 6.0;
	
	area.AxisX2.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
	area.AxisY2.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
	
	chart.Dump();

}

// Define other methods and classes here
float dbToUnit(float db) => (float)Math.Pow(10.0f, db / 20.0f);

float unitToDb(float unit) => unit < 5.011872E-07f ? -126.0f : 20.0f * (float)Math.Log10(unit);


float logFreq(int i) => i == 0 ? 10.0f : (float)(24.0 * Math.Pow(1.0499429, (double)(i - 11)));