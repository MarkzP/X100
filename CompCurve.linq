<Query Kind="Program">
  <Reference>&lt;RuntimeDirectory&gt;\System.Windows.Forms.DataVisualization.dll</Reference>
  <Namespace>System.Drawing</Namespace>
</Query>

void Main()
{
    var freq = 1500.0f;
	var time = 0.1f;
	var levelIn = Enumerable.Range(-53, 59).Select(x => (float)x * 1.5f).ToArray();
	
	var levelOutsL = new float[levelIn.Length];
	var levelOutsR = new float[levelIn.Length];

	foreach (var testPort in System.IO.Ports.SerialPort.GetPortNames())
	{
		using (var port = new System.IO.Ports.SerialPort(testPort))
		{
			port.Open();
			port.ReadTimeout = 500 + (int)(time * 1000.0f);
			port.Write("stats();");
			Thread.Sleep(20);
			var report = port.ReadExisting();
			if (report.Contains("Proc"))
			{
				port.Write("setVolume(0);setInput(0);");
				Thread.Sleep(20);
				port.ReadExisting();

				for (int i = 0; i < levelIn.Length; i++)
				{
					var msg = $"doTestTone({freq.ToString(System.Globalization.CultureInfo.InvariantCulture)},{dbToUnit(levelIn[i]).ToString(System.Globalization.CultureInfo.InvariantCulture)},{time.ToString(System.Globalization.CultureInfo.InvariantCulture)});";
					port.Write(msg);
					var resp = port.ReadLine();
					var parts = resp.Replace("nan", "NaN").Split(',').Select(s => s.Trim());
					//parts.Dump();
					levelOutsL[i] = unitToDb(float.Parse(parts.First(), System.Globalization.CultureInfo.InvariantCulture));
					levelOutsR[i] = unitToDb(float.Parse(parts.Last(), System.Globalization.CultureInfo.InvariantCulture));

					Console.WriteLine($"f={freq},i={levelIn[i]} => {levelOutsL[i]}, {levelOutsR[i]}");
				}

				Thread.Sleep(200);
				port.Write("setVolume(1);setInput(1);");
			}
		}
	}

	var chart = Enumerable.Range(0, levelIn.Length).Chart(x => levelIn[x])
		.AddYSeries(x => levelIn[x], LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => levelOutsL[x], LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => levelOutsR[x], LINQPad.Util.SeriesType.Spline)
		.ToWindowsChart();
		
	var area = chart.ChartAreas.First();
	var xaxis = area.AxisX;
	xaxis.MinorGrid.Enabled = true;
	xaxis.MinorGrid.LineColor = Color.LightGray;
	xaxis.Minimum = -80.0;
	xaxis.Maximum = 6.0;
	xaxis.Interval = 6.0;

	var yaxis = area.AxisY;
	yaxis.Minimum = -80.0;
	yaxis.Maximum = 6.0;
	yaxis.Interval = 6.0;
	
	area.AxisY2.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
	
	chart.Dump();

}


float dbToUnit(float db) => (float)Math.Pow(10.0f, db / 20.0f);

float unitToDb(float unit) => unit < 5.011872E-07f ? -126.0f : 20.0f * (float)Math.Log10(unit);