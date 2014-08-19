#include <algorithm>
#include "Section.h"
#include "MapObjectContainer.h"

namespace mt
{

MapObjectContainer::~MapObjectContainer()
{
	Clear();
}

void MapObjectContainer::Clear()
{
	for(Section* section : mSections)
		delete section;
	mSections.clear();
	mLocations.clear();
}

void MapObjectContainer::Add(Section* section)
{
	mSections.push_back(section);
}

void MapObjectContainer::InsertAfter(Section* after, Section* section)
{
	SectionList::iterator it = std::find(mSections.begin(), mSections.end(), after);
	if(it == mSections.end()) return; // after not found
	it++;
	mSections.insert(it, section);
}

}