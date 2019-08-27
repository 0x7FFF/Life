#include "CMap.h"

inline int mod(int input, int base){
	if (input >= 0) return input % base; else return base + input;
}

void CMap::DrawGrid(HDC dc, RECT rc){
	for (int i = 0; i<height; i++) {
		int y = i * (rc.bottom - rc.top)/height;
		MoveToEx(dc, rc.left, y, NULL);
		LineTo(dc, rc.right, y);
	}
	for (int i = 0; i<width; i++) {
		int x = i * (rc.right - rc.left)/width;
		MoveToEx(dc, x, rc.top, NULL);
		LineTo(dc, x, rc.bottom);
	}
}

void CMap::DrawCells(HDC dc, RECT rc){
	HBRUSH hBrush = CreateSolidBrush(RGB(r, g, b));
	HBRUSH oBrush = (HBRUSH)SelectObject(dc, hBrush);
	RECT cell;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (field[i][j] == 1){
				cell.left = i * (rc.right - rc.left) / width;
				cell.right = (i+1) * (rc.right - rc.left) / width +1;
				cell.top = j * (rc.bottom - rc.top) / height;
				cell.bottom = (j+1) * (rc.bottom - rc.top) / height + 1;
				FillRect(dc, &cell, hBrush);
			}
		}
	}
	SelectObject(dc, oBrush);
	DeleteObject(hBrush);
}

void CMap::Update(){
	temp_field = field;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int around = 0;
			for (int k = -1; k <= 1; k++){
				for (int m = -1; m <= 1; m++){
					if (!(m == 0 && k == 0)){
						if (field[mod(i + k, height)][mod(j + m, width)] == 1)
							around++;
					}
				}
			}
			if (field[i][j] == 1){
				if (around != 2 && around != 3){
					temp_field[i][j] = 0;
					cells_counter -= 1;
				}
			}
			else{
				if (around == 3){
					temp_field[i][j] = 1;
					cells_counter += 1;
				}
			}
		}
	}
	field = temp_field;
	generation += 1;
}

void CMap::SetCell(RECT rc, int x, int y, bool value){
	int i = x*width / (rc.right - rc.left);
	int j = y*height / (rc.bottom - rc.top);
	if (field[i][j] != value){
		if (value == true){
			cells_counter += 1;
		}
		else{
			cells_counter -= 1;
		}
	}
	field[i][j] = value;
	
}

int CMap::ReadPosition(ifstream& fin, int& x, int& y){
	int ret = 0;
	string str;
	fin >> str;
	x = atoi(str.c_str());
	fin >> str;
	y = atoi(str.c_str());
	x += width / 2;
	y += height / 2;
	//indicate if x and y are bigger than size
	if(x > width) {
		ret |= READ_POS_WARNING_X_BIGGER_THAN_WIDTH;
	}
	if (y > height) {
		ret |= READ_POS_WARNING_y_BIGGER_THAN_HEIGHT;
	}
	x = mod(x, width);
	y = mod(y, height);
	//if after modulo it's negative then pop an error
	if (x < 0) {
		ret |= READ_POS_ERROR_X_NEGATIVE;
	}
	if (y < 0) {
		ret |= READ_POS_ERROR_Y_NEGATIVE;
	}
	return ret;
}

int CMap::LoadPattern(string path){
	ifstream fin;
	fin.open(path, ifstream::in);
	if (fin.fail()){
		return FAIL_OPEN_FILE;
	}
	int status = 0;
	string str;
	int x, y, i, j;
	int c; // not char, because 0xFF is defined as я so it's not viable
	int linePos = 0;
	//searching for #P

	while (str != "#P" && fin.good()) { // to prevent infinite looping 
		fin >> str;
		linePos++;
		if (!(c = fin.get()) || c == EOF) { // if #P is literally the last one, it won't trigger, instead it'll load empty map
			return P_MARKER_NOT_FOUND;
		}
	}
	if (linePos > 1) {
		status |= P_MARKER_IS_NOT_IN_THE_BEGINNING;
	}
	bool badBit = false; // if X or Y are negative
	status |= ReadPosition(fin, x, y); // initializes the marker, error-prone
	if (LOBYTE(status) != 0) {
		badBit = true;
	}
	i = x, j = y;
	c = fin.get();
	while (c != EOF && fin.good() && !badBit) {
		while (c != EOF && fin.good() && c != '#') {
			if (!(c = fin.get()) || c == EOF)
				break;
			switch (c) {
			case '*':
				field[i++][j] = 1;
				cells_counter += 1;
				i = mod(i, height);
				break;
			case '.':
				field[i++][j] = 0;
				i = mod(i, height);
				break;
			case '\n':
				i = x;
				j = mod(++j, width);
				break;
			default:
				break;
			}
		}
		if (c == '#') {
			c = fin.get();
			status = ReadPosition(fin, x, y);
			if (LOBYTE(status) != 0) {
				badBit = true;
			}
			i = x, j = y;
		//	c = fin.get();
		}
	}
	fin.close();
	return status;
}

void CMap::SetRGB(BYTE r, BYTE g, BYTE b) {
	this->r = r;
	this->g = g;
	this->b = b;
}