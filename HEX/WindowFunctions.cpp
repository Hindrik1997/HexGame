#include "WindowFunctions.h"

auto CheckMessage() -> bool
{
	MSG Msg;
	if (PeekMessage(&Msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if (Msg.message == WM_QUIT)
			return false;
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return true;
}

auto CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	switch (msg)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				if (MessageBox(NULL, L"Are you sure you want to exit?", L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES) 
				{
					DestroyWindow(hwnd);
					PostQuitMessage(0);
				}
			}
			return 0;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
			default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

auto InitializeWindow(HINSTANCE hInstance, wstring WindowClassName, wstring WindowTitle, int Width, int Height) -> HWND
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WindowClassName.c_str();
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window registratie gefaald!", L"Fout!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	HWND hwnd = CreateWindowEx(
		NULL,
		WindowClassName.c_str(),
		WindowTitle.c_str(),
		WS_SYSMENU, //Enkel sluit button, verder default
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window creatie gefaald!", L"Fout!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	return hwnd;
}
