#include <iostream>
#include <Windows.h>

void mouseClick() {

	INPUT inputs[2] = {};

	// MOUSEEVENTF_LEFTDOWN
	inputs[0].type = INPUT_MOUSE;
	inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	// MOUSEVENTF_LEFTUP
	inputs[1].type = INPUT_MOUSE;
	inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(2, inputs, sizeof(INPUT));
}

int main() {
	HDC hdc = GetDC(NULL);

	int x = 100;
	int y = 100;

	COLORREF color = GetPixel(hdc, x, y);

	BYTE red = GetRValue(color);
	BYTE green = GetGValue(color);
	BYTE blue = GetBValue(color);

	if (SetCursorPos(x, y)) {
		std::cout << "Mouse mov ( " << x << ", " << y << " )" << std::endl;
	}
	else {
		std::cout << "Mouse movement error" << std::endl;
	}

	std::cout << "Color (" << x << ", " << y << "): "
		<< "R=" << int(red) << " G=" << int(green) << " B=" << int(red);

	ReleaseDC(NULL, hdc);

	return 0;
}
