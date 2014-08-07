#include <algorithm>
#include "TopographicObjectContainer.h"

namespace mt
{

void TopographicObjectContainer::Add(Section* section)
{
	mSections.push_back(section);
}

void TopographicObjectContainer::InsertAfter(Section* after, Section* section)
{
	SectionList::iterator it = std::find(mSections.begin(), mSections.end(), after);
	if(it == mSections.end()) return; // after not found
	it++;
	mSections.insert(it, section);
}

}