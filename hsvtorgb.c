// math.h is needed for the floor function
#include <math.h>

static void HSVToRGB(float rgb[3], float h, float s, float v)
{
	float r, g, b;

	int i = floor(h * 6);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch(i % 6)
	{
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
	}

	rgb[0] = r;
	rgb[1] = b;
	rgb[2] = g;
}

