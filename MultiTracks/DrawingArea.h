#ifndef __WW_DRAWINGAREA_H__
#define __WW_DRAWINGAREA_H__

#include "Register.h"
#include "Widget.h"

namespace ww
{

class DrawingArea : public Register, public Widget
{
public:
	static const char* ClassName;

	DrawingArea(Widget* parent);
};

}

#endif // !__WW_DRAWINGAREA_H__