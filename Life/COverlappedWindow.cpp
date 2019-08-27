#include "COverlappedWindow.h"
#include "resource.h"


#define IDC_STATIC1 9000
#define IDC_STATIC2 9001
#define IDC_STATIC3 9002
#define IDC_STATIC4 9004
#define IDC_EDIT1 9005
#define IDC_COMBOBOX1 9006
#define IDC_RADIOBUTTON1_RED 9007
#define IDC_RADIOBUTTON1_BLUE 9008
#define IDC_RADIOBUTTON1_GREEN 9009
#define IDC_STATUSBAR 9010

bool COverlappedWindow::RegisterClassEx(wchar_t* name, HINSTANCE hInstance){
	WNDCLASSEX tag;
	tag.cbSize = sizeof(WNDCLASSEX);
	tag.style = CS_HREDRAW | CS_VREDRAW;
	tag.lpfnWndProc = windowProc;
	tag.cbClsExtra = 0;
	tag.cbWndExtra = 0;
	tag.hIcon = static_cast<HICON>(::LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON2), 1, 32, 32, NULL));
	tag.hCursor = LoadCursor(NULL, IDC_ARROW);
	tag.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	tag.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	tag.lpszClassName = L"OverlappedWindow";
	tag.hInstance = hInstance;
	tag.hIconSm = 0;
	return ::RegisterClassEx(&tag) != 0;
}

bool COverlappedWindow::Create(wchar_t* name, HINSTANCE hInstance){
	handle = CreateWindowEx(0, L"OverlappedWindow", name, WS_OVERLAPPEDWINDOW&~WS_THICKFRAME, 50, 50, 1000, 800, 0, 0, hInstance, this);
	HMENU ret = ::LoadMenu(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
	if (ret == NULL) {
		std::wstringstream wss;
		wss << "Could not load menu! Error:" << GetLastError();
		MessageBox(NULL, wss.str().c_str(), L"Error!", MB_ICONERROR);
		return false;
	}
	return true;
}
void COverlappedWindow::Show(){
	ShowWindow(handle, SW_SHOWNORMAL);
	UpdateWindow(handle);
}

LRESULT __stdcall COverlappedWindow::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE) {
		COverlappedWindow* that = reinterpret_cast<COverlappedWindow*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		::SetWindowLong(hwnd, GWL_USERDATA, reinterpret_cast<LONG>(that));
		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}
	COverlappedWindow* that = reinterpret_cast<COverlappedWindow*>(::GetWindowLong(hwnd, GWL_USERDATA));
	switch (message){
		case WM_CREATE:
			that->handle = hwnd;
			that->OnCreate();
			return ::DefWindowProc(hwnd, message, wParam, lParam);
		case WM_SIZE:
			that->lparam = lParam;
			that->OnSize();
			return 0;
		case WM_CLOSE:
			that->OnClose();
			return 0;
		case WM_DESTROY:
			that->OnDestroy();
			return 0;
		case WM_COMMAND:
			if (HIWORD(wParam) == BN_CLICKED) {
				switch (LOWORD(wParam)) {
				case IDC_RADIOBUTTON1_RED:
					that->curColor = 0;
					that->SetBrushColor(0);
					return 0;
				case IDC_RADIOBUTTON1_GREEN:
					that->curColor = 1;
					that->SetBrushColor(1);
					return 0;
				case IDC_RADIOBUTTON1_BLUE:
					that->curColor = 2;
					that->SetBrushColor(2);
					return 0;
				case ID_NEW:
					that->NewWorld();
					return 0;
				case ID_LOAD:
					that->LoadWorld();
					return 0;
				case ID_EXIT:
					that->OnClose();
					return 0;
				}
			}
			return 0;
		case WM_RBUTTONDOWN:
			that->lparam = lParam;
			that->OnRButtonDown();
			return 0;
		case WM_NOTIFY:
			that->lparam = lParam;
			that->OnNotify();
			return 0;
		default:
			return ::DefWindowProc(hwnd, message, wParam, lParam);
	}
}

void COverlappedWindow::OnCreate(){
	this->map_window = CMapWindow();
	RECT rec;
	GetClientRect(handle, &rec);	

	this->map_window.RegisterClassEx(L"Map", GetModuleHandle(NULL));
	this->map_window.Create(L"Map", GetModuleHandle(NULL), this->handle, 0, rec.top+44, rec.right*0.8, rec.bottom-64);

	this->hwnd_gen_text = CreateWindowEx(0, L"STATIC", L"Generation",  WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, this->handle, 
		reinterpret_cast<HMENU>(IDC_STATIC1), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_cells_text = CreateWindowEx(0, L"STATIC",L"Active cells", WS_CHILD | WS_VISIBLE, 0, 0, 0, 20, this->handle,
		reinterpret_cast<HMENU>(IDC_STATIC2), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_help_text = CreateWindowEx(0, L"STATIC", L"Help text", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, this->handle,
		reinterpret_cast<HMENU>(IDC_STATIC3), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)),NULL);
	this->hwnd_size_text = CreateWindowEx(0, L"STATIC", L"Size text", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, this->handle,
		reinterpret_cast<HMENU>(IDC_STATIC4), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_size_edit = ::CreateWindowEx(0, L"EDIT", NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, this->handle, 
		reinterpret_cast<HMENU>(IDC_EDIT1), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)),  NULL); 
	this->hwnd_color_radiobutton = ::CreateWindowEx(0, L"BUTTON", L"Select colour", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0, 0, 0, 0, this->handle,
		reinterpret_cast<HMENU>(IDC_COMBOBOX1), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_color_radiobutton_red = ::CreateWindowEx(0, L"BUTTON", L"Red", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 0, 0, 0, 0, this->handle,
		reinterpret_cast<HMENU>(IDC_RADIOBUTTON1_RED), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_color_radiobutton_blue = ::CreateWindowEx(0, L"BUTTON", L"Blue", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 0, 0, 0, 0, this->handle,
		reinterpret_cast<HMENU>(IDC_RADIOBUTTON1_BLUE), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_color_radiobutton_green = ::CreateWindowEx(0, L"BUTTON", L"Green", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 0, 0, 0, 0, this->handle,
		reinterpret_cast<HMENU>(IDC_RADIOBUTTON1_GREEN), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_status_bar = ::CreateWindowEx(0,STATUSCLASSNAME,L"Paused",WS_CHILD | WS_VISIBLE | SBARS_TOOLTIPS, 0, 0, 0, 0, this->handle,
		reinterpret_cast<HMENU>(IDC_STATUSBAR), reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);
	this->hwnd_tool_bar = CreateSimpleToolbar();

	this->map_window.gen = this->hwnd_gen_text;
	this->map_window.cells = this->hwnd_cells_text;
	this->map_window.statusbar = this->hwnd_status_bar;

	::SendMessage(this->hwnd_gen_text, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L"Generation: 0"));
	::SendMessage(this->hwnd_cells_text, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L"Active cells: 0"));
	::SendMessage(this->hwnd_help_text, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L"Help:\nSpace - pause/play\nLMB - add cell\nRMB - delete cell"));
	::SendMessage(this->hwnd_size_text, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L"Size: "));
	::SendMessage(this->hwnd_size_edit, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L"200"));
}

void COverlappedWindow::OnRButtonDown() {
	HMENU hPopupMenu = CreatePopupMenu();

	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Exit");
	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_LOAD, L"Load World");
	InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_NEW, L"New World");
	
	SetForegroundWindow(this->handle);
	POINT p;
	p.x = GET_X_LPARAM(lparam);
	p.y = GET_Y_LPARAM(lparam);

	ClientToScreen(this->handle, &p);
	TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, p.x, p.y, 0, this->handle, NULL);
}

void COverlappedWindow::OnNotify() {
	LPTOOLTIPTEXT TTStr;
	TTStr = (LPTOOLTIPTEXT)lparam;
	if (TTStr->hdr.code != TTN_NEEDTEXT)
		return;
	switch (TTStr->hdr.idFrom) {
	case ID_NEW:
		TTStr->lpszText = L"New Game";
		break;
	case ID_LOAD :
		TTStr->lpszText = L"Load map";
		break;
	}
}

void COverlappedWindow::OnSize() {
	RECT rec;
	GetClientRect(handle, &rec);
	SetWindowPos(this->map_window.handle, HWND_TOP, 0, rec.top+44, rec.right*0.8, rec.bottom-64, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_gen_text, HWND_TOP, rec.right*0.8 + 10, 48, rec.right*0.2 - 20, 20, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_cells_text, HWND_TOP, rec.right*0.8 + 10, 88, rec.right*0.2 - 20, 20, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_help_text, HWND_TOP, rec.right*0.8 + 10, 178, rec.right*0.2 - 20, 80, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_size_text, HWND_TOP, rec.right*0.8 + 10, 128, 50, 20, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_size_edit, HWND_TOP, rec.right*0.8 + 65, 128, rec.right*0.2 - 20, 80, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_color_radiobutton, HWND_TOP, rec.right*0.8 + 10, 278, rec.right*0.2 - 20, 120, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_color_radiobutton_red, HWND_TOP, rec.right*0.8 + 10, 308, rec.right*0.2 - 20, 20, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_color_radiobutton_blue, HWND_TOP, rec.right*0.8 + 10, 338, rec.right*0.2 - 20, 20, SWP_DRAWFRAME);
	SetWindowPos(this->hwnd_color_radiobutton_green, HWND_TOP, rec.right*0.8 + 10, 368, rec.right*0.2 - 20, 20, SWP_DRAWFRAME);
	// Allocate an array for holding the right edge coordinates.
	int cParts = 1;
	HLOCAL hloc = LocalAlloc(LHND, sizeof(int) * cParts);
	PINT paParts = (PINT)LocalLock(hloc);

	// Calculate the right edge coordinate for each part, and
	// copy the coordinates to the array.
	int nWidth = rec.right / cParts;
	int rightEdge = nWidth;
	for (int i = 0; i < cParts; i++) {
		paParts[i] = rightEdge;
		rightEdge += nWidth;
	}

	// Tell the status bar to create the window parts.
	::SendMessage(this->hwnd_status_bar, SB_SETPARTS, (WPARAM)cParts, (LPARAM)
		paParts);

	// Free the array, and return.
	LocalUnlock(hloc);
	LocalFree(hloc);
	SetWindowPos(this->hwnd_status_bar, HWND_TOP, rec.right*0.8 + 10, 350, rec.right - rec.left, 32, SWP_DRAWFRAME);
}

int COverlappedWindow::GetSize(){
	DWORD len = ::SendMessage(this->hwnd_size_edit, WM_GETTEXTLENGTH, 0, 0);
	wchar_t* buffer = new wchar_t[len];
	::SendMessage(this->hwnd_size_edit, WM_GETTEXT, (WPARAM)len + 1, (LPARAM)buffer);

	//int size = _wtof(buffer);
	int size = _wtoi(buffer);
	return size;
}

void COverlappedWindow::NewWorld(){
	int size = GetSize();
	map_window.map = CMap(size, size, curColor);
	map_window.map.paused = true;
}

void COverlappedWindow::LoadWorld(){
	OPENFILENAME ofn = { 0 };
	wchar_t szDirect[MAX_PATH];
	wchar_t szFileName[MAX_PATH];
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szDirect;
	*(ofn.lpstrFile) = 0;
	ofn.nMaxFile = sizeof(szDirect);
	ofn.lpstrFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = szFileName;
	*(ofn.lpstrFileTitle) = 0;
	ofn.nMaxFileTitle = sizeof(szFileName);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER;
	GetOpenFileName(&ofn);

	std::wstring wide(ofn.lpstrFile);
	std::string str(wide.begin(), wide.end());

	int size = GetSize();
	CMap map = CMap(size, size, curColor);
	map.paused = true;
	int status = map.LoadPattern(str);
	WORD loStatus = (WORD)(LOBYTE(status));
	ErrorHandler(loStatus);
	if (loStatus == 0) {
		WORD hiStatus = (WORD)(HIBYTE(status));
		WarningHandler(hiStatus);
		map_window.map = map;
	}
}

void COverlappedWindow::OnClose(){
	int msgBox = MessageBox(handle, L"Do you want to exit?", L"Exit", MB_YESNO | MB_ICONQUESTION);
	switch (msgBox) {
		case IDYES:
			break;
		case IDNO:
			return;
	}
	::DestroyWindow(handle);
}

void COverlappedWindow::OnDestroy(){
	KillTimer(handle, 1);
	PostQuitMessage(WM_QUIT);
}

void COverlappedWindow::SetBrushColor(BYTE mode) {
	map_window.SetCellBrushColor(mode);
}

void COverlappedWindow::ErrorHandler(WORD status) {
	const wchar_t errorMessage[] = L"Error";
	if (status & 0x01) {
		MessageBox(NULL, L"Failed to open file!", errorMessage, MB_ICONERROR);
	}	
	if (status & 0x02) {
		MessageBox(NULL, L"P marker is not found! (Maybe you forgot to put it in?)", errorMessage, MB_ICONERROR);
	}
	if (status & 0x04) {
		MessageBox(NULL, L"X pointer coordinate is exceeding negative range!", errorMessage, MB_ICONERROR);
	}
	if (status & 0x08) {
		MessageBox(NULL, L"Y pointer coordinate is exceeding negative range!", errorMessage, MB_ICONERROR);
	}

}

void COverlappedWindow::WarningHandler(WORD status) {
	const wchar_t warnMessage[] = L"Warning";
	if (status & 0x01) {
		MessageBox(NULL, L"X pointer coordinate is bigger than width, modulo will be extracted!", warnMessage, MB_ICONWARNING);
	}
	if (status & 0x02) {
		MessageBox(NULL, L"Y pointer coordinate is bigger than height, modulo will be extracted!", warnMessage, MB_ICONWARNING);
	}
	if (status & 0x04) {
		MessageBox(NULL, L"The first P marker is not located on the first string, the input before the marker will be discarded!", warnMessage, MB_ICONWARNING);
	}
}

HWND COverlappedWindow::CreateSimpleToolbar()
{
	HIMAGELIST hImageList = NULL;
	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 2;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS, 0, 0, 0, 0,
		this->handle, NULL, reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Create the image list.
	hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		numButtons, 0); 
	//ImageList_Add(hImageList, LoadBitmap(reinterpret_cast<HINSTANCE>(::GetWindowLong(this->handle, GWL_HINSTANCE)), L"tbar.bmp"), NULL);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.
	// IDM_NEW and IDM_OPEN are application-defined command constants.

	TBBUTTON tbButtons[numButtons] =
	{
		{ MAKELONG(STD_FILENEW,  ImageListID), ID_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"New" },
		{ MAKELONG(STD_FILEOPEN, ImageListID), ID_LOAD, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Open"},
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}