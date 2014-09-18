#ifndef __MULTITRACKS_WAYPOINT_H__
#define __MULTITRACKS_WAYPOINT_H__

#include "Component.h"
#include "Location.h"
#include "Section.h"

namespace mt
{

class WayPoint : public Component
{
public:
	WayPoint(Section* section, Section::LocationList::const_iterator& after, const Location& location);
	WayPoint(Section* section, Section::LocationList::const_iterator& after);
	virtual ~WayPoint() = default;

	Location* GetLocation() { return &mLocation; }
	Section* GetSection() { return mSection; }
	const Section::LocationList::const_iterator GetAfter() const { return mAfter; }
	bool IsReal() { return mReal; }

private:
	Location mLocation;
	Section* mSection;
	Section::LocationList::const_iterator mAfter;
	bool mReal;
};

}

#endif // !__MULTITRACKS_WAYPOINT_H__