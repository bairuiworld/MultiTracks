#ifndef __WW_BUTTON_H__
#define __WW_BUTTON_H__

#include "SimpleSignal.h"
#include "Widget.h"

namespace ww
{

class Button : public Widget
{
public:
	Button(const char* text = "");
	virtual ~Button() = default;

protected:
	virtual void OnCommand(int code);

	virtual void OnClicked() {}

public:
	sig::Signal<void()> SignalClicked;
};

}

#endif // !__WW_BUTTON_H__