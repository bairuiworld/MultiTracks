#include <Windows.h>
#include <gdiplus.h>
#include "Layout.h"
#include "Widget.h"

namespace ww
{

Widget::Widget() : 
	mParent(nullptr), mhWnd(nullptr), mDefaultProc(nullptr), mLayout(nullptr)
{

}

Widget::Widget(const char* className, int style, Widget* parent) :
	mParent(parent), mhWnd(nullptr), mDefaultProc(nullptr), mLayout(nullptr)
{
	mhWnd = CreateWindowEx(0, className, "", style | WS_VISIBLE,
						   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						   parent ? parent->GetHandle() : HWND_DESKTOP, nullptr, GetModuleHandle(nullptr), (void*)this);
	if(!mhWnd) return;

	SubClass();
}

Widget::~Widget()
{
	for(Widget* widget : mChildren)
		delete widget;
	delete mLayout;
}

void Widget::Add(Widget* widget)
{
	/*Widget* parent = widget->GetParent();
	if(parent == this) return;
	if(parent != nullptr)
		parent->Remove(widget);*/
	mChildren.push_back(widget);
	//widget->SetParent(this);
}

void Widget::Remove(Widget* widget)
{
	WidgetList::const_iterator it = std::find(mChildren.begin(), mChildren.end(), widget);
	if(it == mChildren.end()) return;
	mChildren.erase(it);
	widget->SetParent(nullptr);
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

void Widget::SubClass()
{
	mDefaultProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(mhWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(GlobalWndProc)));
	if(mDefaultProc == GlobalWndProc)
		mDefaultProc = nullptr;
	SetWindowLongPtr(mhWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

Widget* Widget::FromHandle(HWND hWnd)
{
	return reinterpret_cast<Widget*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));
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
			OnPaint(&g);
			if(wParam == 0) EndPaint(hWnd, &ps);
		} break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			SignalResize.emit(width, height);
			if(mLayout)
				mLayout->Apply(this);
			OnResize(width, height);
		} break;
	}
	return CallDefaultProc(hWnd, msg, wParam, lParam);
}

void Widget::OnClose() {}

void Widget::OnResize(int width, int height) {}

void Widget::OnPaint(Gdiplus::Graphics* g) {}

LRESULT CALLBACK Widget::GlobalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Widget* widget = FromHandle(hWnd);
	if(widget == nullptr) return DefWindowProc(hWnd, msg, wParam, lParam);
	else return widget->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Widget::CallDefaultProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(mDefaultProc != nullptr)	return CallWindowProc(mDefaultProc, hWnd, msg, wParam, lParam);
	else return DefWindowProc(hWnd, msg, wParam, lParam);
}

}