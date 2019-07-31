#pragma once
#include <string>
using namespace std;

class RectDrawer {
private:
	RECT oRect;
	HDC hDC;
	HPEN pen, oldPen;
	HBRUSH brush, oldBrush;
	COLORREF lastColor;
public:
	void InitDC();
	void FreeDC();

	void SetColor(COLORREF color);
	void DrawRect(RECT nRect);
	void UnDrawRect();
};

class UIADrawerLibrary {
public:
	bool running;
	RECT tRect;
	HANDLE hThread;
	RectDrawer *rDrawer;
public:
	UIADrawerLibrary();
	~UIADrawerLibrary();

	void StartDraw(RECT rect, COLORREF color);
	void EndDraw();
};
