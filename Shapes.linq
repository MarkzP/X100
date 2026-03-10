<Query Kind="Program">
  <Reference>&lt;RuntimeDirectory&gt;\System.Windows.Forms.DataVisualization.dll</Reference>
</Query>

void Main()
{
	var input = Enumerable.Range(0, 240).Select(i => ((float)i * 0.01f) - 1.2f);
	
	var chart = input.Chart(x => x)
		.AddYSeries(x => hard(x), LINQPad.Util.SeriesType.Spline)
		//.AddYSeries(x => saturation(x), LINQPad.Util.SeriesType.Spline)
		//.AddYSeries(x => cubic(x), LINQPad.Util.SeriesType.Spline)
		//.AddYSeries(x => fifth(x), LINQPad.Util.SeriesType.Spline)
		//.AddYSeries(x => nonlinear(x, 0.0f, 1.0f), LINQPad.Util.SeriesType.Spline)
		.AddYSeries(x => delayDrive(x, 0.05f), LINQPad.Util.SeriesType.Spline)
		.ToWindowsChart();
		
	var area = chart.ChartAreas.First();
	var yaxis = area.AxisY;
	//yaxis.MinorGrid.Enabled = true;
	//yaxis.MinorGrid.LineColor = Color.LightGray;
	//yaxis.Minimum = -1.5;
	//yaxis.Maximum = 1.5;
	
	var xaxis = area.AxisX;
	xaxis.MajorGrid.Enabled = true;
	xaxis.Minimum = -1.2;
	xaxis.Maximum = 1.2;
	xaxis.MajorGrid.Interval = 0.1;
		
	chart.Dump();
}

float gain = 1.0f;

float clip(float x) => x < -1.0f ? -1.0f : x > 1.0f ? 1.0f: x;
float powf(float x, float n) => (float)Math.Pow(x, n);
float fabsf(float x) => (float)Math.Abs(x);
float sinf(float x) => (float)Math.Sin(x);
float arm_sin_f32(float x) => sinf(x);
float square(float x) => x * x;
float cube(float x) { double d = x; return (float)(d * d * d); }
const float _oneThird = 1.0f / 3.0f;
const float _twoThirds = 2.0f / 3.0f;
const float _threeHalfs = 3.0f / 2.0f;

float hard(float sample)
{
	sample *= gain;
	sample = clip(sample);
	return sample;
}

float bilinear(float sample)
{
	float c1 = 2.0f;
	float c2 = 2.0f;
	
	sample *= 0.005f;
	sample = (c1 + 1.0f) * sample / (1.0f + fabsf(c1 * sample));
	sample *= 150.0f;
	sample = (c2 + 1.0f) * sample / (1.0f + fabsf(c2 * sample));
	return sample;
}

float nonlinear(float sample, float color, float s)
{
	sample *= gain;

	float _pcurve = color < 0.0f ? 2.0f : color > 0.5f ? 0.0f : 2.0f - (color * 4.0f);
	float _ncurve = color < 0.5f ? 2.0f : color > 1.0f ? 0.0f : 2.0f - ((color - 0.5f) * 4.0f);
	float _pcomp = 1.0f / (_pcurve + 1.0f);
	float _ncomp = 1.0f / (_ncurve + 1.0f);
	float _skew = s < 0.0f ? 1.0f : s > 1.0f ? 0.0f : 1.0f - s;


	float pcomp = _pcomp;
	float ncomp = _ncomp;
	float pcurve = _pcurve;
	float pcurvep1 = pcurve + 1.0f;
	float ncurve = _ncurve;
	float ncurvep1 = ncurve + 1.0f;
	float skew = _skew;
	float skewm1 = _skew - 1.0f;

        if (sample > 0.0f)
        {
          sample *= pcomp;
          sample = pcurvep1 * sample / (1.0f + (pcurve * sample));
        }
        else
        {
          sample *= ncomp;
          sample = ncurvep1 * sample / (1.0f - (ncurve * sample));
        }

        sample *= _twoThirds;
        sample = clip(sample);
        sample = (sample - (cube(sample) * _oneThird)) * _threeHalfs;

        sample *= (fabsf(sample) + skew) / (square(sample) + skewm1 * fabsf(sample) + 1.0f);
	
	return sample;
}

float smoothstep(float sample, float curve)
{
	sample *= gain * 0.5f;
	sample += 0.5f;
	float x = sample < 0.0f ? 0.0f : sample > 1.0f ? 1.0f : sample;
	//float st = x * x * x * (x * (6.0f * x - 15.0f) + 10.0f);
	float st = x * x * (3.0f - 2.0f * x);
	return (st - 0.5f) * 2.0f;
}

float cubic(float sample)
{
	//sample *= gain * (2.0f / 3.0f);
	sample = clip(sample);
	sample = (sample - ((sample * sample * sample) * (1.0f / 3.0f))) * (3.0f / 2.0f);
	return sample;
}


float fifth(float sample)
{
	sample *= gain * 0.8f;
	sample = clip(sample);
	
    // Fifth
    float s2 = sample * sample;
    float s5 = s2 * s2 * sample;
    sample = (sample - (s5 * (1.0f / 5.0f))) * (5.0f / 4.0f);

	return sample;
}

float waveshaper(float sample, float a)
{
	sample *= gain;// * 0.85f;
	sample = clip(sample);
	sample = sample * (fabsf(sample) + a) / (sample * sample + (a-1.0f)*fabsf(sample) + 1.0f);
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

float saturation(float sample)
{
	sample *= gain * (2.0f / 3.0f);
	sample = clip(sample);
	sample = sinf(sample * 1.57079632f);
	return sample;
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

float delayDrive(float sample, float drive)
{
    drive = drive < 0.0f ? 0.0f : drive > 1.0f ? 1.0f : drive;
    drive = (powf(drive, 2.0f) * 10.0f) + 0.5f;
	float attn = 0.5f / drive;

	sample = (sample + sample) * drive;
    sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
    double s1 = (double)sample;
	double s2 = s1 * s1;
	double s3 = s2 * s1;
	double s5 = s2 * s3;
    
    //sample = (sample - ((float)s3 * (1.0f / 3.0f))) * (3.0f / 2.0f);
	sample = (sample - ((float)s5 * (1.0f / 5.0f))) * (5.0f / 4.0f);
	
    sample *= attn;
	
	return sample;
}