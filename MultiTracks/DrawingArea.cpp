#include "DrawingArea.h"

namespace ww
{

const char* DrawingArea::ClassName = "wwDrawingArea";

DrawingArea::DrawingArea(Widget* parent) :
	Register(ClassName), Widget(ClassName, WS_CHILD, parent)
{

}

}