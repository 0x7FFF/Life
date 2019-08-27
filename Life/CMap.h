#include <Windows.h>
#include <windowsx.h>
#include <vector>
#include <fstream>
#include <string>

#define FAIL_OPEN_FILE 0x0001
#define P_MARKER_NOT_FOUND 0x0002
#define READ_POS_ERROR_X_NEGATIVE 0x0004
#define READ_POS_ERROR_Y_NEGATIVE 0x0008

#define READ_POS_WARNING_X_BIGGER_THAN_WIDTH 0x0100
#define READ_POS_WARNING_y_BIGGER_THAN_HEIGHT 0x0200
#define P_MARKER_IS_NOT_IN_THE_BEGINNING 0x0400

using std::vector;
using std::ifstream;
using std::string;

class CMap {
public:
	CMap(int w, int h, BYTE curColor) :
		width(w),
		height(h),
		field(height, vector<bool>(width, 0)),
		temp_field(field),
		paused(false),
		generation(0),
		cells_counter(0)
	{
		switch (curColor) {
			default:
			case 0:
				r = 255;
				g = 0;
				b = 0;
				break;
			case 1:
				r = 0;
				g = 255;
				b = 0;
				break;
			case 2:
				r = 0;
				g = 0;
				b = 255;
				break;
		}
	};
	void DrawGrid(HDC, RECT); 
	void DrawCells(HDC, RECT);
	int ReadPosition(ifstream&, int&, int&);
	int LoadPattern(string);
	void SetCell(RECT, int, int, bool);
	void SetRGB(BYTE, BYTE, BYTE);
	void Update();
	bool paused;
	int generation;
	int cells_counter;
	BYTE r;
	BYTE g;
	BYTE b;
private:
	int height;
	int width;
	vector<vector<bool>> field;
	vector<vector<bool>> temp_field;
};