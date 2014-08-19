#ifndef __MULTITRACKS_SECTION_H__
#define __MULTITRACKS_SECTION_H__

#include <vector>
#include <algorithm>
#include "tinyxml2.h"
#include "Component.h"

namespace mt {

class MapObjectContainer;
class Location;

class Section : public Component
{
public:
	using LocationList = std::vector<Location>;

public:
	Section(MapObjectContainer* container = nullptr);
	Section(const Section& s);
	~Section();

	void Add(const Location& location);
	template <class LocItr>	void Add(LocItr begin, LocItr end)
	{
		std::for_each(begin, end, [this](const Location& location) { Add(location); });
	}

	void InsertAfter(LocationList::const_iterator after, const Location& location);
	
	Section* Split(LocationList::const_iterator after);
	Section* Split(LocationList::const_iterator after, const Location& location);

	void Reverse();

	const LocationList& GetLocations() const { return mLocations; }
	const Location* GetFirstLocation() const;
	const Location* GetLastLocation() const;

	double GetElevationDifference() const;
	double GetPositiveElevation() const;
	double GetNegativeElevation() const;
	std::vector<double> GetElevationProfile() const;

	double GetLength() const;

	static Section* LoadXML(tinyxml2::XMLElement* element, MapObjectContainer* container = nullptr);
	tinyxml2::XMLElement* SaveXML(tinyxml2::XMLDocument* doc) const;

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
	MapObjectContainer* mContainer;
};

}

#endif // __MULTITRACKS_SECTION_H__