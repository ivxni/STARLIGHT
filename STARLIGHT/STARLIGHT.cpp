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

	// Device context 
	HDC hdc = GetDC(NULL);
	if (hdc == NULL) {
		std::cerr << "Error: Couldnt get HDC" << std::endl;
		return 1;
	}

	// FOV
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int centerX = screenWidth / 2;
	int centerY = screenHeight / 2;

	// Detecting color
	int colorR = 255;
	int colorG = 0;
	int colorB = 0;

	// Detection range
	int range = 75;

	for (int x = centerX - range; x <= centerX + range; x++) {
		for (int y = centerY - range; y <= centerY + range; y++) {

			COLORREF color = GetPixel(hdc, x, y);

			// Extract R, G, B
			BYTE r = GetRValue(color);
			BYTE g = GetGValue(color);
			BYTE b = GetBValue(color);

			// Compare with detecting color
			if (r == colorR && g == colorG && b == colorB) {
				SetCursorPos(x, y);
			}
		}
	}

	ReleaseDC(NULL, hdc);

	return 0;
}
