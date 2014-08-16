#ifndef __WW_EVENT_H__
#define __WW_EVENT_H__

#include <windows.h>

namespace ww
{

class Widget;
class Event
{
public:
	Event(Widget* source);
	virtual ~Event();

protected:
	Widget* mSource;
};

enum class MouseButton { Left, Right, Middle, None };
class MouseEvent : public Event
{
public:
	MouseEvent(Widget* source, POINT point, int clicks, int flags, MouseButton button, POINT prevPoint = POINT{0, 0}, int delta = 0);
	virtual ~MouseEvent();

	int GetX() const;
	int GetY() const;
	POINT GetPoint() const;
	POINT GetPrevPoint() const;

	int GetClicks() const;
	MouseButton GetButton() const;

	bool IsLeftButtonPressed() const;
	bool IsRightButtonPressed() const;
	bool IsMiddleButtonPressed() const;
	bool IsCtrlPressed() const;
	bool IsShiftPressed() const;
	bool IsAltPressed() const;

	int GetWheelRotation() const;

private:
	POINT mPoint;
	POINT mPrevPoint;
	int mClicks;
	int mFlags;
	MouseButton mButton;
	int mDelta;
};

}

#endif // !__WW_EVENT_H__