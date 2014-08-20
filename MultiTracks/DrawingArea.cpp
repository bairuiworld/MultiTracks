#include "DrawingArea.h"

namespace ww
{

const char* DrawingArea::ClassName = "wwDrawingArea";

DrawingArea::DrawingArea() :
	Register(ClassName), Widget(ClassName, WS_POPUP)
{

}

}