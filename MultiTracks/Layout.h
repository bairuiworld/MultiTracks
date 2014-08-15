#ifndef __WW_LAYOUT_H__
#define __WW_LAYOUT_H__

namespace ww
{

class Container;
class Widget;

class Layout
{
public:
	virtual void Apply(Widget* widget) = 0;
};

class FillLayout : public Layout
{
	virtual void Apply(Widget* widget);
};

}

#endif // !__WW_LAYOUT_H__