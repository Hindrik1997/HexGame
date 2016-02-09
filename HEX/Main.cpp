#include <windows.h>
#include "WindowFunctions.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	wstring ClassName = { L"HXC" };
	wstring WindowTitle = {L"H3X: The Game"};


	HWND hwnd = InitializeWindow(hInstance, ClassName, WindowTitle, 800,600 );

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return (int)Msg.wParam;
}
