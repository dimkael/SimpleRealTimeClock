#include <Windows.h>
#include <tchar.h>
#include <cmath>
#include <map>


HRGN clockRgn;
float static speed = 1;

std::map<int, LPCWSTR> hours_map = {
	{ 0, L"12" },
	{ 1, L" 1" },
	{ 2, L" 2" },
	{ 3, L" 3" },
	{ 4, L" 4" },
	{ 5, L" 5" },
	{ 6, L" 6" },
	{ 7, L" 7" },
	{ 8, L" 8" },
	{ 9, L" 9" },
	{ 10, L"10" },
	{ 11, L"11" }
};

PAINTSTRUCT ps;
HDC hdc;

HPEN hPen[3];
HPEN hOldPen = NULL;

const float m_angle = float(3.14159) * 2 / 60;
float m_x, m_y, new_x, new_y;
float m_r = 114;
int x = 230, y = 215;

const float h_angle[3] = {
	float(3.14159) * 2 / 60,
	float(3.14159) * 2 / 60,
	float(3.14159) * 2 / 12
};

POINT hand[3];
float hand_r[3] = { 100, 80, 60 };


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int sx, sy;
	static int time[3] = { 0, 0, 0 };
	SYSTEMTIME localTime;

	switch (message) {
	case WM_KEYDOWN: {
		if (wParam == 13) {
			GetLocalTime(&localTime);
			time[0] = localTime.wSecond;
			time[1] = localTime.wMinute;
			time[2] = localTime.wHour % 12;
		}
		break;
	}

	case WM_TIMER: {
		InvalidateRgn(hWnd, clockRgn, 1);

		time[0]++;
		if (time[0] % 60 == 0 && time[0] > 0) {
			time[0] = 0;
			time[1]++;
		}
		if (time[1] % 60 == 0 && time[1] > 0) {
			time[1] = 0;
			time[2]++;
		}
		if (time[2] % 12 == 0 && time[2] > 0) {
			time[2] = 0;
		}

		break;
	}

	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);

		clockRgn = CreateRoundRectRgn(x - 100, y + 100, x + 100, y - 100, 200, 200);

		MoveToEx(hdc, x, y, NULL);

		Ellipse(hdc, x - 130, y - 130, x + 130, y + 130);

		for (int i = 0; i < 360; i++) {
			MoveToEx(hdc, x, y, NULL);
			m_x = m_r * sin(m_angle * i) + x;
			m_y = -m_r * cos(m_angle * i) + y;

			if (i % 5 == 0)
				TextOut(hdc, m_x - 7, m_y - 7, hours_map[i / 5], 2);
			else {
				new_x = 3 * sin(m_angle * i) + m_x;
				new_y = -3 * cos(m_angle * i) + m_y;

				MoveToEx(hdc, m_x, m_y, NULL);
				LineTo(hdc, new_x, new_y);
			}

		}

		for (int i = 0; i < 3; i++) {
			hPen[i] = CreatePen(0, i * 2, RGB(0, 0, 0));
		}

		hOldPen = reinterpret_cast<HPEN>(SelectObject(hdc, hPen[0]));
		for (int i = 0; i < 3; i++) {
			MoveToEx(hdc, x, y, NULL);
			SelectObject(hdc, hPen[i]);
			hand[i].x = hand_r[i] * sin(h_angle[i] * time[i]) + x;
			hand[i].y = -hand_r[i] * cos(h_angle[i] * time[i]) + y;
			LineTo(hdc, hand[i].x, hand[i].y);
		}

		for (int i = 0; i < 3; i++) {
			DeleteObject(hPen[i]);
		}
		SelectObject(hdc, hOldPen);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;

	wc.hInstance = hInstance;
	wc.lpszClassName = _T("Window");
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);

	if (!RegisterClass(&wc)) return 0;

	hWnd = CreateWindow(
		_T("Window"),
		_T("Simple real-time clock"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		480,
		480,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, nCmdShow);
	SetTimer(hWnd, 1, 1000 / speed, NULL);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}