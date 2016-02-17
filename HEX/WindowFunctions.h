#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include "HexGrid.h"

using std::string;
using std::wstring;

extern HexGrid* g_hexGrid;

bool CheckMessage();
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND InitializeWindow(HINSTANCE hInstance, wstring WindowClassName, wstring WindowTitle, int Width, int Height);
void ProcessCommands(wstring Command,wstring Contents);
void UpdateHexes(HDC& hdc, HexGrid& hexGrid);
void DrawHexes(HDC& hdc, HexGrid& hexGrid);
void FillHexColor(HDC& hdc, HexGrid& hexGrid, int x, int y, COLORREF color);
HexNode* GetHexNodeByCoords(int x, int y, HexGrid& hexGrid);


inline void FillHexRed(HDC& hdc, HexGrid& hexGrid, int x, int y) { FillHexColor(hdc, hexGrid, x, y, RGB(255,0,0)); }
inline void FillHexBlue(HDC& hdc, HexGrid& hexGrid, int x, int y) { FillHexColor(hdc, hexGrid, x, y, RGB(0, 0, 255));  }
inline void FillHexGrey(HDC& hdc, HexGrid& hexGrid, int x, int y) { FillHexColor(hdc,hexGrid, x, y, RGB(240,240,240)); }
