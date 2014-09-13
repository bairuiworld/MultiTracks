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
	if(element->QueryIntAttribute("color", &color) == tinyxml2::XMLError::XML_NO_ERROR)
		c->GetProperties().Set("color", color);
	float linewidth;
	if(element->QueryFloatAttribute("linewidth", &linewidth) == tinyxml2::XMLError::XML_NO_ERROR)
		c->GetProperties().Set("linewidth", linewidth);

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

	if(mProperties.Exists("color"))
		reviewElement->SetAttribute("color", mProperties.Get<int>("color"));
	if(mProperties.Exists("linewidth"))
		reviewElement->SetAttribute("linewidth", mProperties.Get<float>("linewidth"));

	for(Section* section : mSections)
		reviewElement->InsertEndChild(section->SaveXML(doc));
	for(const Location& l : mLocations)
		reviewElement->InsertEndChild(l.SaveXML(doc));
	return reviewElement;
}

}