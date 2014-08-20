#ifndef __WW_WINDOW_H__
#define __WW_WINDOW_H__

#include <map>
#include "Widget.h"
#include "Register.h"

namespace ww
{

class Window : public Register, public Widget
{
public:
	static const char* ClassName;

	Window(const char* title = "");
	virtual ~Window();

	inline void Show();
	inline void Hide();

	virtual void OnClose();
	virtual void OnResize(int width, int height);
};

}

#endif // !__WW_WINDOW_H__