#include "Fractal.h"

Vec2 Fractal::computeNext(Vec2 current, Vec2 constant)
{
	const float zr = current.x * current.x - current.y * current.y;
	const float zi = 2.0 * current.x * current.y;

	return Vec2(zr + constant.x, zi + constant.y);
}

float Fractal::mod2(Vec2 z)
{
	return z.x * z.x + z.y * z.y;
}

int Fractal::computeIterations(Vec2 z0, Vec2 constant, int max_iterations)
{
	Vec2 zn = z0;
	int iteration = 0;
	while (mod2(zn) < 4.0 && iteration < max_iterations) {
		zn = computeNext(zn, constant);
		iteration++;
	}
	return iteration;
}

int Fractal::generatePixel(float x, float y, float cx, float cy)
{
	return computeIterations(Vec2(x, y), Vec2(cx, cy), 255);
}
