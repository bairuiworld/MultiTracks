#ifndef __MULTITRACKS_AREA_H__
#define __MULTITRACKS_AREA_H__

#include "Location.h"

namespace mt
{

class Area
{
public:
	Area();

	const Location& GetNorthWest() const { return mNorthWest; }
	const Location& GetSouthEast() const { return mSouthEast; }

	void Include(const Location& loc);
	void Include(const Area& area);

private:
	Location mNorthWest;
	Location mSouthEast;
};

}

#endif // !__MULTITRACKS_AREA_H__