#include <windows.h>
#include "Vec2.h"
#pragma once
class Fractal
{
	Vec2 computeNext(Vec2 current, Vec2 constant);
	float mod2(Vec2 z);
	int computeIterations(Vec2 z0, Vec2 constant, int max_iterations);
public:
	int generatePixel(float x, float y, float cx, float cy);
};