#ifndef __MULTITRACKS_SECTION_H__
#define __MULTITRACKS_SECTION_H__

#include <vector>
#include <algorithm>
#include "tinyxml2.h"

namespace mt {
namespace track {

class TopographicObjectContainer;
class Location;

class Section
{
public:
	using LocationList = std::vector<Location*>;

public:
	Section();
	Section(TopographicObjectContainer* parent);
	Section(const Section& s);
	~Section();

	void Add(Location* location);
	template <class LocItr>	void Add(LocItr begin, LocItr end)
	{
		std::for_each(begin, end, [this](Location* location) { Add(location); });
	}

	void InsertAfter(LocationList::const_iterator after, const Location& location);
	
	Section* Split(LocationList::const_iterator after, Location* location = nullptr);

	void Reverse();

	const LocationList& GetLocations() const { return mLocations; }
	const Location* GetFirstLocation() const;
	const Location* GetLastLocation() const;

	double GetElevationDifference() const;
	double GetPositiveElevation() const;
	double GetNegativeElevation() const;
	std::vector<double> GetElevationProfile() const;

	double GetLength() const;

	void LoadXML(tinyxml2::XMLElement* element);
	tinyxml2::XMLElement* SaveXML(tinyxml2::XMLDocument* doc);

	/*public Area getBoundingRect()
	{
		Area area = null;
		for(Location l : mLocations)
		{
			if(area == null) area = new Area(l);
			else area.include(l);
		}
		return area;
	}*/

private:
	LocationList mLocations;
	TopographicObjectContainer* mParent;
};

}
}

#endif // __MULTITRACKS_SECTION_H__