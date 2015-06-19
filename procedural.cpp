#include <math.h>

#define	PI 3.14159265358979323846

static float Min(float a, float b)
{
	return (a < b ? a : b);
}

static float Max(float a, float b)
{
	return (a > b ? a : b);
}

static float Clamp(float x, float min, float max)
{
	if(x < min)
		return min;
	if(x > max)
		return max;

	return x;
}

static float Step(float x)
{
	return (x < 0.0f ? 0.0f : 1.0f);
}

static float Ramp(float x)
{
	return x;
}

static float SmoothStep(float x)
{
	return x * x * (3 - 2 * x);
}

static float SmoothStepX2(float x)
{
	return SmoothStep(SmoothStep(x));
}

static float SmoothStepX4(float x)
{
	return SmoothStepX2(SmoothStepX2(x));
}

static float SmootherStep(float x)
{
	return x * x * x * (x * ( x * 6 - 15) + 10);
}

static float CosineStep(float x)
{
	return 0.5f - 0.5f * cos(PI * t);
}

// cosine window of width 'w'
static float CosWindow(float x, float w)
{
	x  = PI * (1.0f / w) * x;

	x = Clamp(x, -PI, PI);

	return cosf(x);
}

static float SinPulse(float x, float width)
{
	x  = PI * (1.0f / width) * x;

	x = Clamp(x, 0, PI);

	return sinf(x);
}

static float Lerp(float t, float a, float b)
{
	return ((1 - t) * a) + (t * b);
}

static float Cerp(float t, float a, float b)
{
	t = CosineStep(t);
	return Lerp(t, a, b);
}

static float Serp(float t, float a, float b)
{
	t = SmoothStep(t);
	return Lerp(t, a, b);
}

// generate an impulse that decays toward 0. 'k' controls the width, the maximum occurs at 1/k
static float Impulse( float k, float x )
{
    float h = k * x;
    return h * expf(1.0 - h);
}

// power curve maps a, b to the range 0, 1
float PowerCurve( float x, float a, float b )
{
    float k = pow(a+b,a+b) / (pow(a,a)*pow(b,b));
    return k * pow( x, a ) * pow( 1.0-x, b );
}
