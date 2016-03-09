#include "WindowFunctions.h"
#include <memory>
#include <wchar.h>
#include <chrono>
#include "resource.h"
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")

/*
Gezien de kleinschaligheid van dit programma gebruik ik hier global's. 
Normaal gesproken zou ik een window class gemaakt hebben en WndProc als instance member implementen
Dit door een pointer naar de instance te verbergen in de extra bytes van de window struct.
En op basis van deze pointer weet ik dus de instance en call ik de bijhorende WndProc instance member functie.
De WndProc van de windowclass (De Windows window class, niet de c++ class) verwijst naar een static WndProc welke de verborgen pointer uit leest.
Dit ivm de verborgen this pointer van een instance variabele, welke incompitabel is met windows' zijn WndProc functie pointer vereiste. (Wat een standaar C functie is)
*/


HWND CommandField, AcceptButton, ViewList, CommandTextLabel,PathButton;
vector<HexNode*> oldPath;
HexGrid* g_hexGrid;
int LeftOffset = 60;
int TopOffset = 80;
int HexGrootte = 30;

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

static HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); //black

auto CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
	switch (msg)
	{
		case WM_CREATE:
		{
			CommandTextLabel = CreateWindowEx(NULL, L"STATIC", L"Commando: ", WS_VISIBLE | WS_CHILD, 10, 520, 80, 20, hwnd, NULL, NULL, NULL);
			CommandField = CreateWindowEx(NULL, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 90, 520, 400, 20, hwnd, NULL, NULL, NULL);
			AcceptButton = CreateWindowEx(NULL, L"BUTTON", L"Accepteer", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 500, 518, 80, 24, hwnd, (HMENU)1, NULL, NULL);
			PathButton = CreateWindowEx(NULL, L"BUTTON", L"Pad", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 500, 494, 80, 24, hwnd, (HMENU)2, NULL, NULL);
			ViewList = CreateWindowEx(NULL, L"EDIT", L"Welkom bij H3X: The Game! \n", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_CLIPCHILDREN | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL, 600, 20, 380, 529, hwnd, (HMENU)2, NULL, NULL);
		}
			break;
		case WM_CTLCOLORSTATIC:
			SetBkColor(GetWindowDC(CommandTextLabel),TRANSPARENT);
			return (INT_PTR)hBrush;
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				if (MessageBox(NULL, L"Weet je zeker dat je wilt stoppen?", L"Meent u dit nou? :(", MB_YESNO | MB_ICONQUESTION) == IDYES) 
				{
					DestroyWindow(hwnd);
					PostQuitMessage(0);
				}
			}
			if (wParam == VK_RETURN)
			{
				SendMessage(hwnd, WM_COMMAND, 1, 0);
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
				{
					/*
					//Commando's processen
					int length = GetWindowTextLength(ViewList) + 1;
					std::unique_ptr<WCHAR> TextBuffer(new WCHAR[length]);
					GetWindowText(ViewList, &(*TextBuffer), length);

					int length2 = GetWindowTextLength(CommandField) + 1;
					std::unique_ptr<WCHAR> TextBuffer2(new WCHAR[length2]);
					GetWindowText(CommandField, &(*TextBuffer2), length2);
					ProcessCommands(wstring(&*TextBuffer2), wstring(&*TextBuffer));
					*/
					
					/*
					HDC hdc = GetDC(hwnd);
					UpdateHexes(hdc,*g_hexGrid);
					auto t1 = std::chrono::high_resolution_clock::now();
					//g_hexGrid->LeftNode->m_SetState(State::NONE);
					//g_hexGrid->RightNode->m_SetState(State::NONE);
					oldPath = g_hexGrid->FindPath(g_hexGrid->LeftNode, &(*g_hexGrid)(10,10));
					auto t2 = std::chrono::high_resolution_clock::now();
					auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
					MessageBox(NULL, std::to_wstring(duration).c_str(), L"TIME RAN FOR SEARCH:", MB_OK);
					if (oldPath.size() == 0)
					{
						MessageBox(NULL, L"EMPTY", L"EMPTY", MB_OK);
						break;
					}
					for (auto it = oldPath.begin(); it != oldPath.end(); it++)
					{
						FillHexColor(hdc, *g_hexGrid, (*it)->m_GetX(), (*it)->m_GetY(), RGB(255,0,255));
					}
					*/
					
					if (g_hexGrid != nullptr)
					{
						if (g_hexGrid->GetVictorious() != State::NONE)
						{
							wstring t;
							if (g_hexGrid->GetVictorious() == State::RED)
								t = L"Rood";
							else
								t = L"Blauw";

							MessageBox(NULL, t.c_str(), L"En de winnaar is:", MB_OK);
							break;
						}
						Move m = g_hexGrid->ComputeBestMove();
						State OppositeState = g_hexGrid->HumanPlayer == State::RED ? State::BLUE : State::RED;
						(*g_hexGrid)(m.x, m.y).m_SetState(OppositeState);
						HDC dc = GetDC(hwnd);
						UpdateHexes(dc, *g_hexGrid);
						if (g_hexGrid->GetVictorious() != State::NONE)
						{
							wstring t;
							if (g_hexGrid->GetVictorious() == State::RED)
								t = L"Rood";
							else
								t = L"Blauw";

							MessageBox(NULL, t.c_str(), L"En de winnaar is:", MB_OK);
						}
					}
					
				}
				break;
			case 2:
			{
				if (g_hexGrid != nullptr)
				{
					HDC hdc = GetDC(hwnd);
					if (g_hexGrid->IsVisible)
					{
						//Turn off
						UpdateHexes(hdc, *g_hexGrid);
					}
					else
					{
						//Turn on
						for (auto it = g_hexGrid->PotPath.begin(); it != g_hexGrid->PotPath.end(); it++)
						{
							FillHexColor(hdc, *g_hexGrid, (*it)->m_GetX(), (*it)->m_GetY(), RGB(255, 0, 255));
						}
					}
					g_hexGrid->IsVisible = !g_hexGrid->IsVisible;
				}
			}
			break;


			}
			break;
		case WM_LBUTTONDOWN:
		{
			POINT Pos;
			GetCursorPos(&Pos);
			if (ScreenToClient(hwnd, &Pos) != 0) 
			{
				//Er is geklikt, mogelijk op een hexagon
				Pos.y = abs(Pos.y);
				Pos.x = abs(Pos.x);

				HexNode* cNode = GetHexNodeByCoords(Pos.x, Pos.y, *g_hexGrid);
				if (cNode != nullptr) 
				{
					//Node has been hit as a bitch!
					cNode->m_SetState(State::RED);
					HDC hdc = GetDC(hwnd);
					FillHexRed(hdc, *g_hexGrid, cNode->m_GetX(), cNode->m_GetY());
				}
			}
		}
		break;
		case WM_RBUTTONDOWN:
			POINT Pos;
			GetCursorPos(&Pos);
			if (ScreenToClient(hwnd, &Pos) != 0)
			{
				//Er is geklikt, mogelijk op een hexagon
				Pos.y = abs(Pos.y);
				Pos.x = abs(Pos.x);

				HexNode* cNode = GetHexNodeByCoords(Pos.x, Pos.y, *g_hexGrid);
				if (cNode != nullptr)
				{
					//Node has been hit as a bitch!
					cNode->m_SetState(State::BLUE);
					HDC hdc = GetDC(hwnd);
					FillHexBlue(hdc, *g_hexGrid, cNode->m_GetX(), cNode->m_GetY());

				}
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
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WindowClassName.c_str();
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

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

auto ProcessCommands(wstring Command,wstring Contents) -> void
{
	if (Command == L"help") 
	{
		wstring Text = Contents;
		Text += L"\r\n Help: \r\n";
		Text += L"- Commands \r\n";
		Text += L" h	Print this help menu \r\n";
		Text += L" o	Set options \r\n";
		Text += L" p	Apply pie rule \r\n";
		Text += L" n	Start new game \r\n";
		Text += L" u	undo a move \r\n";
		Text += L" q	Quit H3X \r\n";
		Text += L"\r\nOr enter a move such as: F5, \r\n";
		Text += L"to place a token in column F row 5 \r\n";
		SetWindowText(ViewList, Text.c_str());
	}
	else
	{
		Contents += L"\r\nOnbekend commando!";
		SetWindowText(ViewList, Contents.c_str());
	}
}

auto UpdateHexes(HDC & hdc, HexGrid & hexGrid) -> void
{
	HBRUSH NoneBrush = CreateSolidBrush(RGB(240, 240, 240));
	HBRUSH RedBrush = CreateSolidBrush(RGB(255,0,0));
	HBRUSH BlueBrush = CreateSolidBrush(RGB(0,0,255));
	SelectObject(hdc,NoneBrush);
	for (unsigned int x = 0; x < hexGrid.get_Size(); ++x)
	{
		for (unsigned int y = 0; y < hexGrid.get_Size(); ++y)
		{		
			switch (hexGrid(x,y).m_GetState())
			{
			case State::BLUE:
				FillHexBlue(hdc, hexGrid, x, y);
				break;
			case State::RED:
				FillHexRed(hdc, hexGrid, x, y);
				break;
			case State::NONE:
				FillHexGrey(hdc, hexGrid, x, y);
				break;
			default: break;
			}
			int hexMiddleX = x*HexGrootte + LeftOffset + y*(HexGrootte / 2);
			int hexMiddleY = y*HexGrootte + TopOffset;
		}
	}
	DeleteObject(NoneBrush);
	DeleteObject(RedBrush);
	DeleteObject(BlueBrush);
}

auto DrawHexes(HDC& hdc, HexGrid& hexGrid) -> void
{
	int s = HexGrootte / 2;
	int a = static_cast<int>(sqrt( (pow(s,2)) - pow((s/2),2)));
	//Representeert de directe 90 graden lijn van punt 6 naar de helft van de verticale as door het centrum.

	const wchar_t letters[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	HBRUSH NoneBrush = CreateSolidBrush(RGB(240, 240, 240));
	SelectObject(hdc,NoneBrush);

	for (unsigned int x = 0; x < hexGrid.get_Size(); ++x) 
	{
		for (unsigned int y = 0; y < hexGrid.get_Size(); ++y)
		{	
			int hexMiddleX = x*HexGrootte + LeftOffset + y*(HexGrootte / 2);
			int hexMiddleY = y*HexGrootte + TopOffset;

			wchar_t Xletter[1] = {letters[x]};
			wstring Yletters = std::to_wstring(y);

			//TopText
			if (y == 0)
			{
				TextOut(hdc, hexMiddleX - HexGrootte/4, hexMiddleY - HexGrootte - HexGrootte/8,Xletter, 1);
			}
			//BottomText
			if (y == hexGrid.get_Size()-1)
			{
				TextOut(hdc, hexMiddleX - HexGrootte / 8, hexMiddleY + HexGrootte / 2 + HexGrootte/8, Xletter, 1);
			}
			//RightText
			if (x == hexGrid.get_Size() - 1)
			{
				TextOut(hdc, hexMiddleX + HexGrootte - HexGrootte/4, hexMiddleY - HexGrootte/4, Yletters.c_str(), static_cast<int>(Yletters.length()));
			}
			//LeftText
			if (x == 0)
			{
				TextOut(hdc, hexMiddleX - HexGrootte - HexGrootte/8, hexMiddleY - HexGrootte / 4, Yletters.c_str(), static_cast<int>(Yletters.length()));
			}
	
			std::unique_ptr<POINT> pArray(new POINT[6]);

			(&*pArray)[0] = { hexMiddleX, hexMiddleY - s };
			(&*pArray)[1] = { hexMiddleX + a, hexMiddleY - s/2 };
			(&*pArray)[2] = { hexMiddleX + a, hexMiddleY + s/2 };
			(&*pArray)[3] = { hexMiddleX,hexMiddleY + s };
			(&*pArray)[4] = { hexMiddleX - a, hexMiddleY + s/2 };
			(&*pArray)[5] = { hexMiddleX - a, hexMiddleY - s/2 };
			
			Polygon(hdc, &*pArray, 6);
		}
	}
	DeleteObject(NoneBrush);
	int size = hexGrid.get_Size();
	HPEN BPen = CreatePen(PS_SOLID, 5, RGB(0,0,255));
	SelectObject(hdc,BPen);
	MoveToEx(hdc, LeftOffset - HexGrootte - HexGrootte/2, TopOffset, NULL);
	LineTo(hdc, LeftOffset + (size-1) * (HexGrootte / 2) - HexGrootte - HexGrootte/2, (size-1) * HexGrootte + TopOffset);
	MoveToEx(hdc, LeftOffset + (size-1) * HexGrootte + HexGrootte + HexGrootte / 2, TopOffset, NULL);
	LineTo(hdc, LeftOffset + (size-1) * HexGrootte + (size - 1) * (HexGrootte / 2) + HexGrootte + HexGrootte / 2, (size - 1) * HexGrootte + TopOffset);
	DeleteObject(BPen);
	HPEN RPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
	SelectObject(hdc,RPen);
	MoveToEx(hdc, LeftOffset, TopOffset - HexGrootte - HexGrootte/2, NULL);
	LineTo(hdc, LeftOffset + HexGrootte * (size-1), TopOffset - HexGrootte - HexGrootte/2);
	MoveToEx(hdc, LeftOffset + (size-1) * (HexGrootte/2),(size-1) * HexGrootte + TopOffset + HexGrootte + HexGrootte / 2, NULL);
	LineTo(hdc, LeftOffset + HexGrootte * (size - 1) + (size - 1) * (HexGrootte / 2), (size - 1) * HexGrootte + TopOffset + HexGrootte + HexGrootte / 2);
	DeleteObject(RPen);
}

auto FillHexColor(HDC & hdc, HexGrid & hexGrid, int x, int y,COLORREF color) -> void
{
	int s = HexGrootte / 2;
	int a = static_cast<int>(sqrt((pow(s, 2)) - pow((s / 2), 2)));

	std::unique_ptr<POINT> pArray(new POINT[6]);

	HBRUSH newBrush = CreateSolidBrush(color);
	SelectObject(hdc, newBrush);

	int hexMiddleX = x*HexGrootte + LeftOffset + y*(HexGrootte / 2);
	int hexMiddleY = y*HexGrootte + TopOffset;

	(&*pArray)[0] = { hexMiddleX, hexMiddleY - s };
	(&*pArray)[1] = { hexMiddleX + a, hexMiddleY - s / 2 };
	(&*pArray)[2] = { hexMiddleX + a, hexMiddleY + s / 2 };
	(&*pArray)[3] = { hexMiddleX,hexMiddleY + s };
	(&*pArray)[4] = { hexMiddleX - a, hexMiddleY + s / 2 };
	(&*pArray)[5] = { hexMiddleX - a, hexMiddleY - s / 2 };

	Polygon(hdc, &*pArray, 6);

	DeleteObject(newBrush);
}

auto GetHexNodeByCoords(int x, int y, HexGrid & hexGrid) -> HexNode* //Check voor nullptr!
{
	int GridSize = static_cast<int>(hexGrid.get_Size());
	int XCoord = -1;
	int YCoord = -1;
	int Distance = HexGrootte / 2;

	for (unsigned int xGrid = 0; xGrid < hexGrid.get_Size(); ++xGrid)
	{
		for (unsigned int yGrid = 0; yGrid < hexGrid.get_Size(); ++yGrid)
		{
			int hexMiddleX = xGrid*HexGrootte + LeftOffset + yGrid*(HexGrootte / 2);
			int hexMiddleY = yGrid*HexGrootte + TopOffset;

			//Middelpunt van hex, dus als binnen Hexgrootte/2 is...

			if (x >= hexMiddleX - Distance && x <= hexMiddleX + Distance && y >= hexMiddleY - Distance && y <= hexMiddleY + Distance)
			{
				//Found coords
				XCoord = xGrid;
				YCoord = yGrid;
				goto End_Loop;
			}
		}
	}
	return nullptr;
	End_Loop:
	return &(hexGrid(XCoord,YCoord));
}
