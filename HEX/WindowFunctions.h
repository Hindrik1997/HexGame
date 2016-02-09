#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

using std::string;

std::wstring StringToWideString(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND InitializeWindow(string WindowClassName, string WindowTitle, HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);