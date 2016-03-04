#include <Windows.h>
#include "lifegame.h"

#pragma warning (disable: 4996)

static int win_height = 980, win_width = 980, map_size = 64;
lifegame lif(map_size);
bool timer = false;

LRESULT CALLBACK Procedure(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	HWND hwnd;
	MSG messages;
	WNDCLASSEX win_main;

	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);

	HICON hICON = NULL;
	win_main.hInstance = hInstance;
	win_main.style = CS_HREDRAW | CS_VREDRAW;
	win_main.cbSize = sizeof(WNDCLASSEX);
	win_main.cbClsExtra = 0;
	win_main.cbWndExtra = 0;
	win_main.lpszClassName = "LifeGame";
	win_main.lpszMenuName = NULL;
	win_main.lpfnWndProc = Procedure;
	win_main.hIcon = hICON;
	win_main.hIconSm = hICON;
	win_main.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_main.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClassEx(&win_main))
		return 0;

	hwnd = CreateWindowEx(0, "LifeGame", "LifeGame", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT,
		CW_USEDEFAULT, win_width+16, win_height+39, HWND_DESKTOP, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);

	ZeroMemory(&messages, sizeof(messages));
	while (messages.message != WM_QUIT)
	{
		if (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messages);
			DispatchMessage(&messages);
		}
	}
	return messages.wParam;
}
LRESULT CALLBACK Procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			break;
		}
		case WM_RBUTTONDOWN:
		{
			if (timer)
				KillTimer(hwnd, IDT_TIMER_GEN);
			else
				SetTimer(hwnd, IDT_TIMER_GEN, TIMER_GEN_INTERVAL, NULL);
			timer = !timer;
			break;
		}
		case WM_LBUTTONDOWN:
		{
			POINT ptr = { lParam & 0xffff, lParam >> 16 };
			double p = win_width / (double)map_size;
			POINT idx = { ptr.x / (win_width / (double)map_size), ptr.y / (win_height / (double)map_size) };
			if (idx.x >= map_size)
				idx.x = map_size - 1;
			if (idx.y >= map_size)
				idx.y = map_size - 1;
			if(!timer)
				lif.ele[idx.x][idx.y] = !lif.ele[idx.x][idx.y];
			return 0;
		}
		case WM_LBUTTONUP:				
		case WM_RBUTTONUP:
		{
			InvalidateRect(hwnd, NULL, false);
			return 0;
		}
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case 'C':
					lif.clear();
				default:
					break;
			}
			InvalidateRect(hwnd, NULL, false);
			return 0;
		}
		case WM_TIMER:
		{
			int ret;
			switch (wParam)
			{
				case IDT_TIMER_GEN:
					ret = lif.next();
					if (!ret)
					{
						KillTimer(hwnd, IDT_TIMER_GEN);
						timer = false;
					}

					InvalidateRect(hwnd, NULL, false);
				default:
					return 0;
			}
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			RECT mRC;
			HDC hdc, mDC;
			HBITMAP mBitmap, mOldBitmap;

			hdc = BeginPaint(hwnd, &ps);

			GetClientRect(hwnd, &mRC);
			mDC = CreateCompatibleDC(hdc);
			mBitmap = CreateCompatibleBitmap(hdc, mRC.right, mRC.bottom);
			mOldBitmap = (HBITMAP)SelectObject(mDC, mBitmap);

			PatBlt(mDC, 0, 0, mRC.right, mRC.bottom, WHITENESS);
			SetBkMode(mDC, TRANSPARENT);
			SetStretchBltMode(mDC, COLORONCOLOR);

				HPEN hOldPen = (HPEN)SelectObject(mDC, CreatePen(0, 1, RGB(0, 0, 0)));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(mDC, CreateSolidBrush(RGB(0, 121, 255)));

				for (int i = 0; i < map_size; i++)
					for (int j = 0; j < map_size; j++)
						if(lif.ele[i][j])
							Rectangle(mDC, i * win_width/map_size, j * win_height/map_size, (i+1) * win_width / map_size, (j+1) * win_height / map_size);
				
				DeleteObject(SelectObject(mDC, hOldPen));
				DeleteObject(SelectObject(mDC, hOldBrush));

				hOldPen = (HPEN)SelectObject(mDC, CreatePen(0, 1, RGB(0, 0, 0)));
				hOldBrush = (HBRUSH)SelectObject(mDC, CreateSolidBrush(RGB(255, 255, 255)));

				for (int i = 0; i < map_size; i++)
					for (int j = 0; j < map_size; j++)
						if (!lif.ele[i][j])
							Rectangle(mDC, i * win_width / map_size, j * win_height / map_size, (i + 1) * win_width / map_size, (j + 1) * win_height / map_size);

				DeleteObject(SelectObject(mDC, hOldPen));
				DeleteObject(SelectObject(mDC, hOldBrush));

				RECT rt;
				SetRect(&rt, win_width - 40, 0, win_width, 20);
				char gen[32];
				itoa(lif.gen, gen, 10);
				DrawText(mDC, gen, -1, &rt, DT_WORDBREAK);


				SetRect(&rt, 0, 0, 40, 20);
				itoa(lif.count, gen, 10);
				DrawText(mDC, gen, -1, &rt, DT_WORDBREAK);

			BitBlt(hdc, 0, 0, mRC.right, mRC.bottom, mDC, 0, 0, SRCCOPY);

			SelectObject(mDC, mOldBitmap);
			DeleteObject(mBitmap);
			DeleteDC(mDC);

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	return 0;
}