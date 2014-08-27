#include "DrawingArea.h"

namespace ww
{

const char* DrawingArea::ClassName = "wwDrawingArea";

DrawingArea::DrawingArea() :
Register(ClassName), Widget(ClassName, "", WS_CLIPCHILDREN)
{

}

LRESULT CALLBACK DrawingArea::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_ERASEBKGND) return 1;
	return Widget::WndProc(hWnd, msg, wParam, lParam);
}

}