#ifndef __WW_WIDGET_H__
#define __WW_WIDGET_H__

#include <Windows.h>
#include <gdiplus.h>
#include "SimpleSignal.h"

namespace ww
{

class Container;
class Layout;

class Widget
{
public:
	Widget();
	Widget(const char* className, int style);
	virtual ~Widget();

	using WidgetList = std::vector<Widget*>;

	void Add(Widget* widget);
	void Remove(Widget* widget);
	const WidgetList& GetChildren() const { return mChildren; }

	Layout* GetLayout() const { return mLayout; }
	void SetLayout(Layout* layout);

	RECT GetBounds() const;
	void SetBounds(const RECT& rc);

	HWND GetHandle() const { return mhWnd; }

	Widget* GetParent() const { return mParent; }
	
	void Invalidate();

protected:
	void SetParent(Widget* parent);
	
	virtual void OnClose();
	virtual void OnResize(int width, int height);
	virtual void OnPaint(Gdiplus::Graphics* g);

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	friend class Register;
	static LRESULT CALLBACK GlobalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static Widget* FromHandle(HWND hWnd);

	void SubClass();
	LRESULT CALLBACK CallDefaultProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	Widget* mParent;
	HWND mhWnd;
	WNDPROC mDefaultProc;
	Layout* mLayout;
	WidgetList mChildren;

public:
	sig::Signal<void()> SignalClose;
	sig::Signal<void(int, int)> SignalResize;
};

}

#endif // !__WW_WIDGET_H__