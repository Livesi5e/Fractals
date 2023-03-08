#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "json/json.h"
#include "Fractal.h"

LRESULT CALLBACK WindowsProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowsProcParam(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool redraw = false;
float cx, cy, z, ox, oy;
std::vector<float> fx, fy;
int currentRound = 0;
HWND paramx, paramy, offsetx, offsety, zoom, generate, mainW;

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
	const wchar_t CLASS_NAME2[] = L"Params";

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	WNDCLASS wc = {};
	WNDCLASS wcparam = {};

	wc.lpfnWndProc			= WindowsProc;
	wc.lpszClassName		= CLASS_NAME;
	wcparam.lpfnWndProc		= WindowsProcParam;
	wcparam.lpszClassName	= CLASS_NAME2;

	RegisterClass(&wc);
	RegisterClass(&wcparam);

	mainW = CreateWindowExW(
		0,
		CLASS_NAME,
		L"",
		WS_POPUP,
		desktop.left,
		desktop.top,
		desktop.right,
		desktop.bottom,
		NULL,
		NULL,
		NULL,
		NULL
	);

	HWND paramWin = CreateWindowExW(
		0,
		CLASS_NAME2,
		L"",
		WS_SYSMENU,
		100,
		100,
		370,
		400,
		NULL,
		NULL,
		NULL,
		NULL
	);
	
	if (mainW == NULL || paramWin == NULL) {
		return 0;
	}

	ShowWindow(mainW, SW_MAXIMIZE);
	ShowWindow(paramWin, SW_RESTORE);

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
			int r = 0;
			COLORREF* colorData1D = (COLORREF*)calloc(desktop.bottom * desktop.right, sizeof(COLORREF));

			for (int y = -desktop.bottom / 2; y < desktop.bottom / 2; ++y)
			{
				for (int x = -desktop.right / 2; x < desktop.right / 2; ++x) {
					int res = frac.generatePixel((float)x / (desktop.bottom * z) + ox, (float)y / (desktop.bottom * z) + oy, cx, cy);
					colorData1D[r++] = 0x00010000 * res + 0x00000100 * res;
				}
			}

			// Generate bitmap
			HBITMAP data = CreateBitmap(desktop.right, desktop.bottom, 1, 8*4, (void*)colorData1D);

			HDC hdcMem = CreateCompatibleDC(hdc);
			auto oldBitmap = SelectObject(hdcMem, data);
			BitBlt(hdc, 0, 0, desktop.right, desktop.bottom, hdcMem, 0, 0, SRCCOPY);

			ReleaseDC(NULL, hdcMem);
			ReleaseDC(NULL, hdc);
			redraw = false;
		}
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT WindowsProcParam(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		paramx = CreateWindowW(L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 5, 5, 105, 23, hwnd, (HMENU) 2, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0);
		paramy = CreateWindowW(L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 5, 60, 105, 23, hwnd, (HMENU) 3, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0);
		offsetx = CreateWindowW(L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 125, 5, 105, 23, hwnd, (HMENU) 4, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0);
		offsety = CreateWindowW(L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 125, 60, 105, 23, hwnd, (HMENU) 5, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0);
		zoom = CreateWindowW(L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 245, 5, 105, 23, hwnd, (HMENU) 6, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0);
		generate = CreateWindowW(L"BUTTON", L"Generate", WS_CHILD | WS_VISIBLE, 125, 115, 105, 23, hwnd, (HMENU) 1, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 0);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		HDC hdc = GetDC(hwnd);
		RECT paramXText = { 5, 28, 105, 45 };
		RECT paramYText = { 5, 88, 105, 105 };
		RECT offsetXText = { 125, 28, 230, 45 };
		RECT offsetYText = { 125, 88, 230, 105 };
		RECT zoomText = { 245, 28, 350, 45 };
		SetBkMode(hdc, TRANSPARENT);
		DrawTextW(hdc, (LPWSTR)L"Parameter for X", -1, &paramXText, DT_SINGLELINE | DT_CENTER);
		DrawTextW(hdc, (LPWSTR)L"Parameter for Y", -1, &paramYText, DT_SINGLELINE | DT_CENTER);
		DrawTextW(hdc, (LPWSTR)L"X-Offset", -1, &offsetXText, DT_SINGLELINE | DT_CENTER);
		DrawTextW(hdc, (LPWSTR)L"Y-Offset", -1, &offsetYText, DT_SINGLELINE | DT_CENTER);
		DrawTextW(hdc, (LPWSTR)L"Zoom", -1, &zoomText, DT_SINGLELINE | DT_CENTER);
		return 0;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == 1) {
			int len = GetWindowTextLengthW(paramx) + 1;
			LPWSTR cxS = (LPWSTR) new char[len];
			GetWindowTextW(paramx, cxS, len);
			len = GetWindowTextLengthW(paramy) + 1;
			LPWSTR cyS = (LPWSTR) new char[len];
			GetWindowTextW(paramy, cyS, len);
			len = GetWindowTextLengthW(offsetx) + 1;
			LPWSTR oxS = (LPWSTR) new char[len];
			GetWindowTextW(offsetx, oxS, len);
			len = GetWindowTextLengthW(offsety) + 1;
			LPWSTR oyS = (LPWSTR) new char[len];
			GetWindowTextW(offsety, oyS, len);
			len = GetWindowTextLengthW(zoom) + 1;
			LPWSTR zS = (LPWSTR) new char[len];
			GetWindowTextW(zoom, zS, len);
			cx = std::stof(cxS);
			cy = std::stof(cyS);
			ox = std::stof(oxS);
			oy = std::stof(oyS);
			z = std::stof(zS);
			SendMessageW(mainW, WM_PAINT, NULL, NULL);
			redraw = true;
		}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}