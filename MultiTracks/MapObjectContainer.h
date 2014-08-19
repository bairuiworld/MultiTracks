#ifndef __MULTITRACKS_MAPOBJECTCONTAINER_H__
#define __MULTITRACKS_MAPOBJECTCONTAINER_H__

#include <list>
#include "Component.h"
#include "Location.h"

namespace mt {

class Section;

class MapObjectContainer : public Component
{
public:
	using SectionList = std::list<Section*>;
	using LocationList = std::list<Location>;

public:
	virtual ~MapObjectContainer();

	void Add(Section* section);
	void InsertAfter(Section* after, Section* section);

	void Clear();

	const SectionList& GetSections() const { return mSections; }
	const LocationList& GetLocations() const { return mLocations; }

protected:
	SectionList mSections;
	LocationList mLocations;
};

}

#endif // __MULTITRACKS_MAPOBJECTCONTAINER_H__