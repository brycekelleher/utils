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

static float Lerp(float t, float a, float b)
{
	return ((1 - t) * a) + (t * b);
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

static float CosWindow(float x, float width)
{
	x  = PI * (1.0f / width) * x;

	x = Clamp(x, -PI, PI);

	return cosf(x);
}

static float SinPulse(float x, float width)
{
	x  = PI * (1.0f / width) * x;

	x = Clamp(x, 0, PI);

	return sinf(x);
}

