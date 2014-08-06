#ifndef __MULTITRACKS_TOPOGRAPHICSOBJECTCONTAINER_H__
#define __MULTITRACKS_TOPOGRAPHICSOBJECTCONTAINER_H__

#include <list>

namespace mt {
namespace track {

class Section;
class Location;

class TopographicObjectContainer
{
public:
	using SectionList = std::list<Section*>;
	using LocationList = std::list<Location*>;

public:
	void Add(Section* section);
	void InsertAfter(Section* after, Section* section);

	const SectionList& GetSections() const { return mSections; }
	const LocationList& GetLocations() const { return mLocations; }

protected:
	SectionList mSections;
	LocationList mLocations;
};

}
}

#endif // __MULTITRACKS_TOPOGRAPHICSOBJECTCONTAINER_H__