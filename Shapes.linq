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
		.AddYSeries(x => nonlinear(x, 0.7f, 0.0f), LINQPad.Util.SeriesType.Spline, "p3_0_0")
		.AddYSeries(x => nonlinear5(x, 0.7f, 0.0f), LINQPad.Util.SeriesType.Spline, "p5_0_0")
		//.AddYSeries(x => nonlinear(x, 1.0f, 0.0f), LINQPad.Util.SeriesType.Spline, "p3_1_0")
		//.AddYSeries(x => nonlinear5(x, 1.0f, 0.0f), LINQPad.Util.SeriesType.Spline, "p5_1_0")
		//.AddYSeries(x => delayDrive(x, 1.00f), LINQPad.Util.SeriesType.Spline)
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

float gain = 5.0f;

float clip(float x) => x < -1.0f ? -1.0f : x > 1.0f ? 1.0f: x;
float powf(float x, float n) => (float)Math.Pow(x, n);
float fabsf(float x) => (float)Math.Abs(x);
double abs(double x) => Math.Abs(x);
float sinf(float x) => (float)Math.Sin(x);
float arm_sin_f32(float x) => sinf(x);
float square(float x)
{
  if (fabsf(x) < 1.0e-9f) return 0.0f;
  double d = x;
  return (float)(d * d);
}

float cube(float x)
{
  if (fabsf(x) < 1.0e-6f) return 0.0f;
  double d = x;
  return (float)(d * d * d);
}
float p3(float x)
{
  if (fabsf(x) < 1.0e-6f) return 0.0f;
  double d = x;
  return (float)(d * d * d);
}

float p5(float x)
{
  if (fabsf(x) < 2.5118864e-4f) return 0.0f;
  double s1 = (double)x;
  double s2 = s1 * s1;
  double s3 = s2 * s1;
  return (float)(s2 * s3);
}
const float _oneThird = 1.0f / 3.0f;
const float _twoThirds = 2.0f / 3.0f;
const float _threeHalfs = 3.0f / 2.0f;
const float _oneFifth = 1.0f / 5.0f;
const float _threeFourths = 3.0f / 4.0f;
const float _fiveFourths = 5.0f / 4.0f;


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


        if (sample < 0.0f)
        {
          sample *= ncomp;
          sample = ncurvep1 * sample / (1.0f - (ncurve * sample));
		  sample *= _twoThirds;
		  sample = sample < -1.0f ? -1.0f : sample;
		  sample -= (p3(sample) * _oneThird);
		  sample *= _threeHalfs;
		  sample *= (-sample + skew) / (square(sample) + (skewm1 * -sample) + 1.0f);
        }
		else
		{
          sample *= pcomp;
          sample = pcurvep1 * sample / (1.0f + (pcurve * sample));
		  sample *= _twoThirds;
		  sample = sample > 1.0f ? 1.0f : sample;
		  sample -= (p3(sample) * _oneThird);
		  sample *= _threeHalfs;
		  sample *= (sample + skew) / (square(sample) + (skewm1 * sample) + 1.0f);
        }

	
	return sample;
}

float nonlinear5(float sample, float color, float s)
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


        if (sample < 0.0f)
        {
          sample *= ncomp;
          sample = ncurvep1 * sample / (1.0f - (ncurve * sample));
		  sample *= _threeFourths;
		  sample = sample < -1.0f ? -1.0f : sample;
		  sample -= (p5(sample) * _oneFifth);
		  sample *= _fiveFourths;
		  sample *= (-sample + skew) / (square(sample) + (skewm1 * -sample) + 1.0f);
        }
		else
		{
          sample *= pcomp;
          sample = pcurvep1 * sample / (1.0f + (pcurve * sample));
		  sample *= _threeFourths;
		  sample = sample > 1.0f ? 1.0f : sample;
		  sample -= (p5(sample) * _oneFifth);
		  sample *= _fiveFourths;
		  sample *= (sample + skew) / (square(sample) + (skewm1 * sample) + 1.0f);
        }

	
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
	  drive = powf(drive, 2.0f);
      float attn = 0.1f / (drive + 0.1f);
      drive = (drive * 50.0f) + (4.0f / 5.0f);
	  
	  sample *= drive;

	sample = sample < -1.0f ? -1.0f : sample > 1.0f ? 1.0f : sample;
      double s1 = (double)sample;
	  if (abs(s1) < 2.5118864e-4f) s1 = 0.0;
      double s2 = s1 * s1;
      double s3 = s2 * s1;
      double s5 = s2 * s3;
      sample -= (float)(s5 * (1.0 / 5.0));
	  sample *= (5.0f / 4.0f);
	
	return sample * attn;
}