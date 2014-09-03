#ifndef __MULTITRACKS_COMPONENT_H__
#define __MULTITRACKS_COMPONENT_H__

#include "Properties.h"

namespace mt
{

class Component
{
public:
	virtual ~Component() = default;
	Properties& GetProperties() { return mProperties; }
	const Properties& GetProperties() const { return mProperties; }

protected:
	Properties mProperties;
};

}

#endif // !__MULTITRACKS_COMPONENT_H__