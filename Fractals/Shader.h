#include <windows.h>
#include <iostream>
#include "Vec2.h"
#include "opencl.h"
#define MAX_SOURCE_SIZE (0x100000)
#pragma once
class Shader
{
public:
	BITMAP generateFractal(int x, int y, int cx, int cy, Vec2 size);
};