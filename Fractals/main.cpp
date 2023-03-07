#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <Shlwapi.h>
#include <fstream>
#include <vector>
#include <string>
#include "json/json.h"
#include "Fractal.h"
#include "opencl.h"
#include "CL/cl.h"

LRESULT CALLBACK WindowsProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool redraw = true;
float cx, cy, px, py, z, ox, oy;
std::vector<float> fx, fy;
int currentRound = 0;
HWND paramx, paramy, offsetx, offsety;

int main() {
	std::ifstream fractals("fractals.json", std::ifstream::binary);
	Json::Value fracs;
	fractals >> fracs;
	std::cout << fracs["x"] << "\n" << fracs["y"];
	for (int i = 0; i < fracs["x"].size(); i++)
	{
		if (fracs["x"][i].asString()[1] == '-') {
			fx.push_back(stof(fracs["x"][i].asString()) * -1);	
		}
		else {
			fx.push_back(stof(fracs["x"][i].asString()));
		}
	}
	for (int i = 0; i < fracs["y"].size(); i++)
	{
		if (fracs["y"][i].asString()[1] == '-') {
			fy.push_back(stof(fracs["y"][i].asString()) * -1);
		}
		else {
			fy.push_back(stof(fracs["y"][i].asString()));
		}
	}

	const wchar_t CLASS_NAME[] = L"Fractal";

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	WNDCLASS wc = {};

	wc.lpfnWndProc		= WindowsProc;
	wc.lpszClassName	= CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowExW(
		0,
		CLASS_NAME,
		L"",
		WS_VISIBLE |WS_POPUP,
		desktop.left,
		desktop.top,
		desktop.right,
		desktop.bottom,
		NULL,
		NULL,
		NULL,
		NULL
	);

	paramx = CreateWindowW(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE, 56, 10, 50, 18, hwnd, 0, 0, 0);
	
	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, SW_MAXIMIZE);

	MSG msg = {};
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowsProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT:
	{
		if (redraw) {
			HDC hdc = GetDC(hwnd);
			Fractal frac;
			RECT desktop;
			const HWND hDesktop = GetDesktopWindow();
			GetWindowRect(hDesktop, &desktop);
			cx = -0.835;	// x-Parameter aus dem Mandelbrot-Set. Intervall [-2; 1]
			cy = -0.2321;	// y-Parameter aus dem Mandelbrot-Set. Intervall [-1; 1]
			z = 0.5;		// Zoom factor of the fractal. Whole fractal is visible with parameter .5 or .25
			ox = 0;			// Offset for x-Axis
			oy = 0;			// Offset for y-Axis
			for (int x = -desktop.right / 2; x < desktop.right / 2; ++x)
			{
				for (int y = -desktop.bottom / 2; y < desktop.bottom / 2; ++y) {
					int color = frac.generatePixel((float)x / (desktop.bottom * z) + ox, (float)y / (desktop.bottom * z) + oy, cx, cy);
					SetPixel(hdc, x + desktop.right / 2, y + desktop.bottom / 2, RGB(color, color, 0));
				}
			}
			redraw = false;
		}
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
