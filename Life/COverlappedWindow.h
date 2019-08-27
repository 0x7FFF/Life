#include <Windows.h>
#include <windowsx.h>
#include <sstream>
#include <CommCtrl.h>
#include "CMapWindow.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

class COverlappedWindow {
public:
	COverlappedWindow() : handle(0), curColor(0){}
	static bool RegisterClassEx(wchar_t*, HINSTANCE);
	bool Create(wchar_t*, HINSTANCE);
	void Show();
	//LONG SetWindowLong(HWND, int, LONG);
	CMapWindow map_window;

protected:
	void OnDestroy();
	void OnCreate();
	void OnSize();
	void OnClose();
	void OnRButtonDown();
	void OnNotify();
	void LoadWorld();
	void NewWorld();
	void SetBrushColor(BYTE);
	void ErrorHandler(WORD);
	void WarningHandler(WORD);
private:
	HWND CreateSimpleToolbar();
	int GetSize();
	HWND hwnd_cells_text;
	HWND hwnd_gen_text;
	HWND hwnd_help_text;
	HWND hwnd_size_edit;
	HWND hwnd_size_text;
	HWND hwnd_color_radiobutton;
	HWND hwnd_color_radiobutton_red;
	HWND hwnd_color_radiobutton_blue;
	HWND hwnd_color_radiobutton_green;
	HWND hwnd_status_bar;
	HWND hwnd_tool_bar;
	HWND handle;
	BYTE curColor;
	LPARAM lparam;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

};
