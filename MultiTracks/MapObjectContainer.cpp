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
	section->SetParent(this);
}

void MapObjectContainer::Remove(Section* section)
{
	SectionList::iterator it = std::find(mSections.begin(), mSections.end(), section);
	if(it == mSections.end()) return;
	mSections.erase(it);
}

void MapObjectContainer::InsertAfter(Section* after, Section* section)
{
	SectionList::iterator it = std::find(mSections.begin(), mSections.end(), after);
	if(it == mSections.end()) return; // after not found
	it++;
	mSections.insert(it, section);
}

MapObjectContainer* MapObjectContainer::LoadXML(tinyxml2::XMLElement* element)
{
	MapObjectContainer* c = new MapObjectContainer;
	
	int color;
	if(element->QueryIntAttribute(prop::Color::name, &color) == tinyxml2::XMLError::XML_NO_ERROR)
		c->mProperties.Set<prop::Color>(color);
	float linewidth;
	if(element->QueryFloatAttribute(prop::LineWidth::name, &linewidth) == tinyxml2::XMLError::XML_NO_ERROR)
		c->mProperties.Set<prop::LineWidth>(linewidth);

	tinyxml2::XMLElement* e = element->FirstChildElement("section");
	while(e)
	{
		Section* s = Section::LoadXML(e, c);
		c->mSections.push_back(s);
		e = e->NextSiblingElement("section");
	}

	e = element->FirstChildElement("l");
	while(e)
	{
		c->mLocations.push_back(Location::LoadXML(e));
		e = e->NextSiblingElement("l");
	}
	return c;
}

tinyxml2::XMLElement* MapObjectContainer::SaveXML(tinyxml2::XMLDocument* doc, const char* name)
{
	tinyxml2::XMLElement* reviewElement = doc->NewElement(name);

	if(mProperties.Exists(prop::Color::name))
		reviewElement->SetAttribute(prop::Color::name, mProperties.Get<prop::Color>());
	if(mProperties.Exists(prop::LineWidth::name))
		reviewElement->SetAttribute(prop::LineWidth::name, mProperties.Get<prop::LineWidth>());

	for(Section* section : mSections)
		reviewElement->InsertEndChild(section->SaveXML(doc));
	for(const Location& l : mLocations)
		reviewElement->InsertEndChild(l.SaveXML(doc));
	return reviewElement;
}

}