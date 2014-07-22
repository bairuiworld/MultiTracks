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
	typedef std::list<Section*> SectionList;
	typedef std::list<Location*> LocationList;

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