#include "CMap.h"
#include "resource.h"
#include <CommCtrl.h>

class CMapWindow {
public:
	CMapWindow() : handle(0), map(200, 200, 0), gen(0), cells(0), statusbar(0){}

	static bool RegisterClassEx(wchar_t*, HINSTANCE);
	bool Create(wchar_t*, HINSTANCE, HWND, int, int, int, int);
	void Show();
	//LONG SetWindowLong(HWND, int, LONG);
	void SetCellBrushColor(BYTE mode);
	HWND handle;
	HWND gen;
	HWND cells;
	HWND statusbar;
	CMap map;
protected:
	void OnDestroy();
	void OnPaint();
	void OnTimer();
	void OnCreate();
	void OnClose();
	void OnLButtonDown();
	void OnRButtonDown();
private:
	LPARAM lparam;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
