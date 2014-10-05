#ifndef __MULTITRACKS_SECTION_H__
#define __MULTITRACKS_SECTION_H__

#include <vector>
#include <algorithm>
#include "tinyxml2.h"
#include "Component.h"
#include "MapObjectContainer.h"

namespace mt {

namespace SectionType
{
static const int Flat = 1;
static const int DownhillOnly = 2;
static const int UpDownBoth = 3;
static const int UphillPreferred = 4;
}

namespace SectionDifficulty
{
static const int None = 1;
static const int Technical = 2;
static const int Physical = 3;
static const int TechnicalPhysical = 4;
}

namespace SectionInterest
{
static const int None = 1;
static const int Usable = 2;
static const int Pleasant = 3;
static const int UseIfPossible = 4;
static const int Unavoidable = 5;
static const int Impraticable = 6;
static const int ToBeAvoided = 7;
}

class Location;
class Area;

class Section : public Component
{
public:
	using LocationList = std::vector<Location>;

public:
	Section(MapObjectContainer* container = nullptr);
	Section(const Section& s);
	virtual ~Section();

	void SetParent(MapObjectContainer* container) { mContainer = container; mProperties.SetParent(&container->GetProperties()); }
	MapObjectContainer* GetParent() const { return mContainer; }

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
	Location* GetFirstLocation();
	Location* GetLastLocation();

	double GetElevationDifference() const;
	double GetPositiveElevation() const;
	double GetNegativeElevation() const;
	std::vector<double> GetElevationProfile() const;

	double GetLength() const;

	static Section* LoadXML(tinyxml2::XMLElement* element, MapObjectContainer* container = nullptr);
	tinyxml2::XMLElement* SaveXML(tinyxml2::XMLDocument* doc) const;

	Area GetBoundingBox() const;

private:
	LocationList mLocations;
	MapObjectContainer* mContainer;
};

}

#endif // __MULTITRACKS_SECTION_H__