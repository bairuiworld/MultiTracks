#include "DrawingArea.h"

namespace ww
{

const char* DrawingArea::ClassName = "wwDrawingArea";

DrawingArea::DrawingArea() :
	Register(ClassName), Widget(ClassName)
{

}

LRESULT CALLBACK DrawingArea::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_ERASEBKGND: return 0;
	default: return Widget::WndProc(hWnd, msg, wParam, lParam);
	}
}

}