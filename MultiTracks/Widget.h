#ifndef __WW_WIDGET_H__
#define __WW_WIDGET_H__

#include <Windows.h>
#include <gdiplus.h>
#include "SimpleSignal.h"
#include "Event.h"

namespace ww
{

class Container;
class Layout;

class Widget
{
public:
	Widget(const char* className, const char* text = "", int style = 0);
	virtual ~Widget();

	using WidgetList = std::vector<Widget*>;

	void Add(Widget* widget);
	void Remove(Widget* widget);
	const WidgetList& GetChildren() const { return mChildren; }

	Layout* GetLayout() const { return mLayout; }
	void SetLayout(Layout* layout);

	void SetText(const char* text);

	RECT GetBounds() const;
	void SetBounds(const RECT& rc);

	HWND GetHandle() const { return mhWnd; }

	Widget* GetParent() const { return mParent; }
	
	void Invalidate();

protected:
	virtual void Create(Widget* parent);

	virtual void OnClose() {}
	virtual void OnResize(int width, int height) {}
	virtual void OnPaint(Gdiplus::Graphics* g, const RECT& clip) {}
	virtual void OnMouseDown(MouseEvent ev) {}
	virtual void OnMouseUp(MouseEvent ev) {}
	virtual void OnMouseDoubleClick(MouseEvent ev) {}
	virtual void OnMouseMove(MouseEvent ev) {}
	virtual void OnMouseDrag(MouseEvent ev) {}
	virtual void OnMouseWheel(MouseEvent ev) {}
	virtual void OnNotify(LPNMHDR lpnmhdr) {}
	virtual void OnCommand(int code) {}

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	friend class Register;
	static LRESULT CALLBACK GlobalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static Widget* FromHandle(HWND hWnd);

	void SubClass();
	LRESULT CALLBACK CallDefaultProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	Widget* mParent;
	const char* mClassName;
	const char* mText;
	HWND mhWnd;
	int mStyle;
	Layout* mLayout;
	WidgetList mChildren;

	bool mIsMouseDown;
	POINT mMouseLastPos;

public:
	sig::Signal<void()> SignalClose;
	sig::Signal<void(int, int)> SignalResize;
	sig::Signal<void(MouseEvent)> SignalMouseDown;
	sig::Signal<void(MouseEvent)> SignalMouseUp;
	sig::Signal<void(MouseEvent)> SignalMouseDoubleClick;
	sig::Signal<void(MouseEvent)> SignalMouseMove;
	sig::Signal<void(MouseEvent)> SignalMouseDrag;
	sig::Signal<void(MouseEvent)> SignalMouseWheel;
};

}

#endif // !__WW_WIDGET_H__