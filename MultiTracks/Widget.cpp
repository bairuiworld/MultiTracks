#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include "Layout.h"
#include "Widget.h"

namespace ww
{

Widget::Widget(const char* className, const char* text, int style) :
mParent(nullptr), mClassName(className), mText(text), mhWnd(nullptr), mStyle(style), mLayout(nullptr), mIsMouseDown(false)
{
	
}

Widget::~Widget()
{
	for(Widget* widget : mChildren)
		delete widget;
	delete mLayout;
	DestroyWindow(mhWnd);
}

void Widget::Create(Widget* parent)
{
	if(parent) mStyle |= WS_CHILD;
	mhWnd = CreateWindowEx(0, mClassName, mText, mStyle | WS_VISIBLE,
						   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						   parent ? parent->GetHandle() : HWND_DESKTOP, nullptr, GetModuleHandle(nullptr), (void*)this);
	if(!mhWnd) return;
	mParent = parent;
	SetWindowLongPtr(mhWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));

	for(Widget* child : mChildren)
		child->Create(this);
}

void Widget::Add(Widget* widget)
{
	if(widget->GetParent() != nullptr) return;
	mChildren.push_back(widget);
	widget->Create(this);
	if(mLayout)
		mLayout->Apply(this);
}

void Widget::Remove(Widget* widget)
{
	WidgetList::const_iterator it = std::find(mChildren.begin(), mChildren.end(), widget);
	if(it == mChildren.end()) return;
	mChildren.erase(it);
	widget->mParent = nullptr;
}

RECT Widget::GetBounds() const
{
	RECT rc;
	GetClientRect(mhWnd, &rc);
	return rc;
}

void Widget::SetBounds(const RECT& rc)
{
	MoveWindow(mhWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
}

void Widget::SetLayout(Layout* layout)
{
	delete mLayout;
	mLayout = layout;
	if(mLayout)
		mLayout->Apply(this);
}

void Widget::Invalidate()
{
	InvalidateRect(mhWnd, nullptr, false);
}

Widget* Widget::FromHandle(HWND hWnd)
{
	return reinterpret_cast<Widget*>(GetWindowLongPtr(hWnd, GWL_USERDATA));
}

LRESULT CALLBACK Widget::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		SignalClose.emit();
		OnClose();
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC;
			if(wParam == 0) hDC = BeginPaint(hWnd, &ps);
			else hDC = (HDC)wParam;
			Gdiplus::Graphics g(hDC);
			OnPaint(&g, ps.rcPaint);
			if(wParam == 0) EndPaint(hWnd, &ps);
		} break;


	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		mIsMouseDown = true;
		SetCapture(hWnd);
		mMouseLastPos = POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		MouseEvent ev(this, mMouseLastPos, 1, wParam,
					  msg == WM_LBUTTONDOWN ? MouseButton::Left :
					  msg == WM_RBUTTONDOWN ? MouseButton::Right :
					  msg == WM_MBUTTONDOWN ? MouseButton::Middle :
					  MouseButton::None);
		SignalMouseDown.emit(ev);
		OnMouseDown(ev);
	} break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		mIsMouseDown = false;
		ReleaseCapture();
		mMouseLastPos = POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		MouseEvent ev(this, mMouseLastPos, 1, wParam,
					  msg == WM_LBUTTONUP ? MouseButton::Left :
					  msg == WM_RBUTTONUP ? MouseButton::Right :
					  msg == WM_MBUTTONUP ? MouseButton::Middle :
					  MouseButton::None);
		SignalMouseUp.emit(ev);
		OnMouseUp(ev);
	} break;

	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	{
		MouseEvent ev(this, POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}, 2, wParam,
					  msg == WM_LBUTTONDBLCLK ? MouseButton::Left :
					  msg == WM_MBUTTONDBLCLK ? MouseButton::Right :
					  msg == WM_RBUTTONDBLCLK ? MouseButton::Middle :
					  MouseButton::None);
		SignalMouseDoubleClick.emit(ev);
		OnMouseDoubleClick(ev);
	} break;

	case WM_MOUSEMOVE:
	{
		POINT mouseNewPos = POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		MouseEvent ev(this, mouseNewPos, 0, wParam, MouseButton::None, mMouseLastPos);
		mMouseLastPos = mouseNewPos;
		if(mIsMouseDown)
		{
			SignalMouseDrag.emit(ev);
			OnMouseDrag(ev);
		}
		else
		{
			SignalMouseMove.emit(ev);
			OnMouseMove(ev);
		}
	} break;

	case WM_MOUSEWHEEL:
	{
		POINT pt = POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		ScreenToClient(mhWnd, &pt);
		MouseEvent ev(this, pt, 0, GET_KEYSTATE_WPARAM(wParam), MouseButton::None, mMouseLastPos, GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA);
		SignalMouseWheel.emit(ev);
		OnMouseWheel(ev);
	} break;

	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		SignalResize.emit(width, height);
		if(mLayout)
			mLayout->Apply(this);
		OnResize(width, height);
	} break;

	case WM_COMMAND:
	{
		HWND hCtrl = reinterpret_cast<HWND>(lParam);
		if(hCtrl != nullptr)
		{
			Widget* child = Widget::FromHandle(hCtrl);
			if(child != nullptr)				
				child->OnCommand(HIWORD(wParam));
		}
	} break;

	case WM_NOTIFY:
	{
		LPNMHDR lpnmhdr = reinterpret_cast<LPNMHDR>(lParam);
		Widget* child = Widget::FromHandle(lpnmhdr->hwndFrom);
		if(child != nullptr)
			child->OnNotify(lpnmhdr);
	} break;

	}
	return CallDefaultProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Widget::GlobalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Widget* widget = FromHandle(hWnd);
	if(widget == nullptr) return DefWindowProc(hWnd, msg, wParam, lParam);
	else return widget->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Widget::CallDefaultProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

}