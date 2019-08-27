#include "CMapWindow.h"

bool CMapWindow::RegisterClassEx(wchar_t* name, HINSTANCE hInstance){
	WNDCLASSEX tag;
	tag.cbSize        = sizeof(WNDCLASSEX);
	tag.style         = CS_HREDRAW | CS_VREDRAW;
	tag.lpfnWndProc   = windowProc;
	tag.cbClsExtra    = 0;
	tag.cbWndExtra    = 0;
	tag.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	tag.hCursor       = LoadCursor(NULL, IDC_ARROW);
	tag.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	tag.lpszMenuName  = NULL;
	tag.lpszClassName = L"MapWindow";
	tag.hInstance     = hInstance;
	tag.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	return ::RegisterClassEx(&tag)!=0;
}

bool CMapWindow::Create(wchar_t* name, HINSTANCE hInstance, HWND hwnd, int x, int y, int w, int h){
	handle = CreateWindowEx(0, L"MapWindow", name,
		WS_CHILD | WS_BORDER, x, y, w, h, hwnd, 0, hInstance, this);
	this->Show();
	return true;
}
void CMapWindow::Show(){
	ShowWindow(handle, SW_SHOWNORMAL);
	UpdateWindow(handle);
}

LRESULT __stdcall CMapWindow::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE) {
		CMapWindow* that = reinterpret_cast<CMapWindow*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		::SetWindowLong(hwnd, GWL_USERDATA, reinterpret_cast<LONG>(that));
		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}
	CMapWindow* that = reinterpret_cast<CMapWindow*>(::GetWindowLong(hwnd, GWL_USERDATA));
	switch(message){
		case WM_CREATE:
			that->handle = hwnd;
			that->OnCreate();
			return ::DefWindowProc(hwnd, message, wParam, lParam);
		case WM_PAINT:
			that->OnPaint();
			return 0;
		case WM_TIMER:
			that->OnTimer();
			return 0;
		case WM_DESTROY:
			that->OnDestroy();
			return 0;
		case WM_CLOSE:
			that->OnClose();
			return 0;
		case WM_LBUTTONDOWN:
			that->lparam = lParam;
			that->OnLButtonDown();
			return 0;
		case WM_RBUTTONDOWN:
			that->lparam = lParam;
			that->OnRButtonDown();
			return 0;
		case WM_COMMAND:
			switch (LOWORD(wParam)){
			case ID_PAUSE:
				that->map.paused = !that->map.paused;
				return 0;
			}
		default:
			return ::DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}



void CMapWindow::OnCreate(){
	SetTimer(this->handle, 1, 60, NULL);
	map.paused = true;
}

void CMapWindow::OnPaint(){
	PAINTSTRUCT ps;
	RECT rc;
	GetClientRect(handle, &rc);

	HDC hDC = BeginPaint(handle, &ps);
	HDC cDC = CreateCompatibleDC(hDC);
	HBITMAP cBMP = CreateCompatibleBitmap(hDC, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);

	HBITMAP hBMP = reinterpret_cast<HBITMAP>(SelectObject(cDC, cBMP));
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH oBrush = reinterpret_cast<HBRUSH>(SelectObject(cDC, hBrush));
	FillRect(cDC, &rc, hBrush);
	SelectObject(cDC, oBrush);
	DeleteObject(hBrush);

	this->map.DrawGrid(cDC, rc);
	this->map.DrawCells(cDC, rc);
	
	BitBlt(hDC,
		ps.rcPaint.left,
		ps.rcPaint.top,
		ps.rcPaint.right - ps.rcPaint.left,
		ps.rcPaint.bottom - ps.rcPaint.top,
		cDC,
		ps.rcPaint.left,
		ps.rcPaint.top,
		SRCCOPY);

	SelectObject(cDC, hBMP);
	DeleteObject(cBMP);
	DeleteDC(cDC);
	EndPaint(handle, &ps);
}

void CMapWindow::OnTimer(){
	CMapWindow* ptr = this;
	RECT rec;
	GetClientRect(handle, &rec);
	
	wchar_t status[8] = L"Paused";
	if (!map.paused){
		map.Update();
		wcscpy_s(status, L"Running");
	}
	::SendMessage(statusbar, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(status));

	wchar_t str[200];
	_itow_s(map.generation, str, 10);
	std::wstring s1 = std::wstring(L"Generation: ");
	std::wstring s2 = std::wstring(str);
	::SendMessage(gen, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((s1 + s2).c_str()));
	_itow_s(map.cells_counter, str, 10);
	s1 = std::wstring(L"Active cells: ");
	s2 = std::wstring(str);
	::SendMessage(cells, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((s1 + s2).c_str()));

	InvalidateRect(handle, NULL, FALSE);
}

void CMapWindow::OnLButtonDown(){
	int xPos = GET_X_LPARAM(lparam);
	int yPos = GET_Y_LPARAM(lparam);
	RECT rc;
	GetClientRect(handle, &rc);
	map.SetCell(rc, xPos, yPos, 1);
}

void CMapWindow::OnRButtonDown(){
	int xPos = GET_X_LPARAM(lparam);
	int yPos = GET_Y_LPARAM(lparam);
	RECT rc;
	GetClientRect(handle, &rc);
	map.SetCell(rc, xPos, yPos, 0);
}

void CMapWindow::OnClose(){
	::DestroyWindow(this->handle);
}

void CMapWindow::OnDestroy(){
	KillTimer(handle, 1);
	PostQuitMessage(WM_QUIT);
}

void CMapWindow::SetCellBrushColor(BYTE mode) {
	switch (mode) {
	case 0:
		map.SetRGB(255, 0, 0);
		break;
	case 1:
		map.SetRGB(0, 255, 0);
		break;
	case 2:
		map.SetRGB(0, 0, 255);
		break;
	}
}