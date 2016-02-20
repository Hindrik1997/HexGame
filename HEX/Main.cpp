#include "WindowFunctions.h"
#include "HexGrid.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	wstring ClassName = { L"H3X" };
	wstring WindowTitle = {L"H3X: The Game"};
	int Width = 1000;
	int Height = 600;

	HWND hwnd = InitializeWindow(hInstance, ClassName, WindowTitle, Width,Height);
	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window handle was zero?", L"Yo Dawg, Weird Error man!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	HDC hdc = GetDC(hwnd);

	HexGrid h(11);
	g_hexGrid = &h;
	DrawHexes(hdc,*g_hexGrid);

	//Main loop
	do
	{
	} 
	while (CheckMessage() != false);
	ReleaseDC(hwnd,hdc);
	return 0;
}
