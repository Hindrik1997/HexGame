#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

using std::string;
using std::wstring;

bool CheckMessage();
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND InitializeWindow(HINSTANCE hInstance, wstring WindowClassName, wstring WindowTitle, int Width, int Height);