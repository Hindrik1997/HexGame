#include "WindowFunctions.h"
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")

HWND CommandField, AcceptButton, ViewList, CommandTextLabel;

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

static HBRUSH hBrush=CreateSolidBrush(RGB(255, 255, 255)); //black

auto CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	switch (msg)
	{
		case WM_CREATE:

			CommandTextLabel = CreateWindowEx(NULL, L"STATIC",L"Commando: ", WS_VISIBLE | WS_CHILD, 10, 520, 80, 20,hwnd,NULL,NULL,NULL);
			CommandField = CreateWindowEx(NULL, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 90, 520, 400, 20, hwnd, NULL,NULL, NULL);
			AcceptButton = CreateWindowEx(NULL, L"BUTTON", L"Accepteer", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON , 500, 518, 80, 24, hwnd, (HMENU)1, NULL, NULL);
			ViewList = CreateWindowEx(NULL, L"STATIC", L"Some cool text bae", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 600, 20, 160, 529, hwnd, NULL, NULL, NULL);
			break;
		case WM_CTLCOLORSTATIC:
			SetBkColor(GetWindowDC(CommandTextLabel),TRANSPARENT);
			return (INT_PTR)hBrush;
			break;
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
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case 1:
				MessageBox(NULL, L"Yo Swa", L"Good morning!", MB_OK);
				break;
			}

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
	RECT r = {0,0,Width,Height};
	AdjustWindowRect(&r, WS_SYSMENU, FALSE);



	HWND hwnd = CreateWindowEx(
		NULL,
		WindowClassName.c_str(),
		WindowTitle.c_str(),
		WS_SYSMENU, //Enkel sluit button, verder default
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window creatie gefaald!", L"Fout!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	return hwnd;
}
