#include "Widget.h"
#include "Event.h"

namespace ww
{

Event::Event(Widget* source) :
mSource(source)
{

}

Event::~Event()
{

}

MouseEvent::MouseEvent(Widget* source, POINT point, int clicks, int flags, MouseButton button, POINT prevPoint, int delta) :
Event(source), mPoint(point), mClicks(clicks), mFlags(flags), mButton(button), mPrevPoint(prevPoint), mDelta(delta)
{

}

MouseEvent::~MouseEvent()
{

}

int MouseEvent::GetX() const { return mPoint.x; }
int MouseEvent::GetY() const { return mPoint.y; }
POINT MouseEvent::GetPoint() const { return mPoint; }
POINT MouseEvent::GetPrevPoint() const { return mPrevPoint; }

int MouseEvent::GetClicks() const { return mClicks; }
MouseButton MouseEvent::GetButton() const { return mButton; }

bool MouseEvent::IsLeftButtonPressed() const { return (mFlags & MK_LBUTTON) != 0; }
bool MouseEvent::IsRightButtonPressed() const { return (mFlags & MK_RBUTTON) != 0; }
bool MouseEvent::IsMiddleButtonPressed() const { return (mFlags & MK_MBUTTON) != 0; }
bool MouseEvent::IsShiftPressed() const { return (mFlags & MK_SHIFT) != 0; }
bool MouseEvent::IsCtrlPressed() const { return (mFlags & MK_CONTROL) != 0; }
bool MouseEvent::IsAltPressed() const { return (mFlags & MK_ALT) != 0; }

}