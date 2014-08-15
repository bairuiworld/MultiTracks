#include "Widget.h"
#include "Layout.h"

namespace ww
{

void FillLayout::Apply(Widget* widget)
{
	RECT rc = widget->GetBounds();
	for(Widget* child : widget->GetChildren())
		child->SetBounds(rc);
}

}