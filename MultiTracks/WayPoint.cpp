#include "WayPoint.h"

namespace mt
{

WayPoint::WayPoint(Section* section, Section::LocationList::const_iterator& after, const Location& location) :
mSection(section), mAfter(after), mLocation(location), mReal(false)
{

}

WayPoint::WayPoint(Section* section, Section::LocationList::const_iterator& after) :
mSection(section), mAfter(after), mReal(true)
{

}

}