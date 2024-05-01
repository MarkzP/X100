<Query Kind="Program">
  <Reference>&lt;RuntimeDirectory&gt;\System.Windows.Forms.DataVisualization.dll</Reference>
</Query>

void Main()
{
	var input = Enumerable.Range(0, 240).Select(i => ((float)i * 0.01f) - 1.2f);
	
	var chart = input.Chart(x => x)
		//.AddYSeries(x => nonlinear(x, 0.2f), LINQPad.Util.SeriesType.Spline)
		//.AddYSeries(x => saturation(x, 0.1f), LINQPad.Util.SeriesType.Spline)
		//.AddYSeries(x => overdrive(x, 1.0f), LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => cubic(x, 0.5f), LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => foldback(x, 0.5f), LINQPad.Util.SeriesType.Spline)
		.ToWindowsChart();
		
	var area = chart.ChartAreas.First();
	var yaxis = area.AxisY;
	//yaxis.MinorGrid.Enabled = true;
	//yaxis.MinorGrid.LineColor = Color.LightGray;
	//yaxis.Minimum = -1.5;
	//yaxis.Maximum = 1.5;
		
	chart.Dump();
}

float gain = 4.0f;

float nonlinear(float sample, float curve)
{
	float c = 5.0f;
	sample *= gain;
	
	float clipped = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;

	float skewed = (c + 1.0f) * clipped / (1.0f + Math.Abs(c * clipped));
	//sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
	
	return (skewed * curve) + (clipped * (1.0f - curve));
}

float smoothstep(float sample, float curve)
{
	sample *= gain * 0.5f * (1.0f - (curve * 0.333f));
	sample += 0.5f;
	float x = sample < 0.0f ? 0.0f : sample > 1.0f ? 1.0f : sample;
	//float st = x * x * x * (x * (6.0f * x - 15.0f) + 10.0f);
	float st = x * x * (3.0f - 2.0f * x);
	float mix = (st * curve) + (x * (1.0f - curve));
	return (mix - 0.5f) * 2.0f;	
}

float cubic(float sample, float curve)
{
	sample *= gain * (1.0f - (curve * 0.333f));
	sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
	sample = (sample - ((sample * sample * sample) * (1.0f / 3.0f))) * (3.0f / 2.0f);
	return sample;
}

float waveshaper(float sample, float a)
{
	sample *= gain;
	sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
	sample = sample * (Math.Abs(sample) + a) / (sample * sample + (a-1.0f)*Math.Abs(sample) + 1.0f);
	return sample;
}

float foldback(float sample, float threshold)
{
	
  if (sample>threshold || sample<=threshold)
  {
    sample = Math.Abs(Math.Abs((sample - threshold % (threshold*4.0f))) - threshold*2) - threshold;
  }
  return sample;
}

float saturation(float sample, float curve)
{
	sample *= gain * (1.0f - (curve * 0.35f));
	float clipped = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
	float nonlinear = (float)Math.Sin((double)clipped * 0.5 * Math.PI);
	return (nonlinear * curve) + (clipped * (1.0f - curve));
}

float atan(float sample, float curve)
{
	sample *= gain;
	
	if (curve > 0.00001f)
	{
		sample = (1.0f / curve) * (float)Math.Atan(sample * curve);
	}
	
	return sample;
}

float overdrive(float sample, float curve)
{
	sample *= gain;
	
	sample = sample > 0.0f ? (float)Math.Sqrt(sample) : -(float)Math.Sqrt(-sample);
	
	return sample;
}