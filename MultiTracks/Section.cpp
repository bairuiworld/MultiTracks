#include <iterator>
#include "Location.h"
#include "MapObjectContainer.h"
#include "Area.h"
#include "Section.h"

namespace mt {

Section::Section(MapObjectContainer* container) : mContainer(container)
{
	if(container)
		mProperties.SetParent(&container->GetProperties());
}

Section::Section(const Section& section) : mContainer(section.mContainer)
{
	Add(section.mLocations.begin(), section.mLocations.end());
}

Section::~Section()
{

}

void Section::Add(const Location& location)
{
	mLocations.push_back(location);
}

void Section::InsertAfter(LocationList::const_iterator after, const Location& location)
{
	mLocations.insert(after, location);
}

Section* Section::Split(LocationList::const_iterator after)
{
	if(after == mLocations.begin() || after == mLocations.end() || after == mLocations.end() - 1)
		return nullptr; // nothing to split

	Section* right = new Section;
	right->Add(*after);
	after++;

	std::move(after, mLocations.cend(), std::back_inserter(right->mLocations));
	mLocations.erase(after, mLocations.end());

	if(mContainer != nullptr)
		mContainer->InsertAfter(this, right);
	
	return right;
}

Section* Section::Split(LocationList::const_iterator after, const Location& location)
{
	if(after == mLocations.begin() || after == mLocations.end() || after == mLocations.end() - 1)
		return nullptr; // nothing to split

	Section* right = new Section;
	right->Add(location);
	after++;

	std::move(after, mLocations.cend(), std::back_inserter(right->mLocations));
	mLocations.erase(after, mLocations.end());
	Add(location);

	if(mContainer != nullptr)
		mContainer->InsertAfter(this, right);

	return right;
}

void Section::Reverse()
{
	std::reverse(mLocations.begin(), mLocations.end());
}

const Location* Section::GetFirstLocation() const
{
	if(mLocations.size() == 0) return nullptr;
	return &mLocations.front();
}

const Location* Section::GetLastLocation() const
{
	if(mLocations.size() == 0) return nullptr;
	return &mLocations.back();
}

double Section::GetElevationDifference() const
{
	if(mLocations.size() <= 1) return 0;
	return GetLastLocation()->GetElevation() - GetFirstLocation()->GetElevation();
}

double Section::GetPositiveElevation() const
{
	if(mLocations.size() <= 1) return 0;
	LocationList::const_iterator it = mLocations.begin();
	double d = 0;
	double elev = (*it).GetElevation();
	for(++it; it != mLocations.end(); it++)
	{
		double e = (*it).GetElevation();
		if(std::abs(e - elev) > 10)
		{
			if(e > elev)
				d += e - elev;
			elev = e;
		}
	}
	return d;
}

double Section::GetNegativeElevation() const
{
	if(mLocations.size() <= 1) return 0;
	LocationList::const_iterator it = mLocations.begin();
	double d = 0;
	double elev = (*it).GetElevation();
	for(++it; it != mLocations.end(); it++)
	{
		double e = (*it).GetElevation();
		if(std::abs(e - elev) > 10)
		{
			if(e < elev)
				d += elev - e;
			elev = e;
		}
	}
	return d;
}

std::vector<double> Section::GetElevationProfile() const
{
	std::vector<double> profile;
	for(const Location& l : mLocations)
		profile.push_back(l.GetElevation());
	return profile;
}

double Section::GetLength() const
{
	if(mLocations.size() <= 1) return 0;
	const Location* prev = &mLocations.front();
	double len = 0;
	for(const Location& l : mLocations)
	{
		len += prev->GetDistance(l);
		prev = &l;
	}
	return len;
}

Area Section::GetBoundingBox() const
{
	Area area;
	for(const Location& l : mLocations)
		area.Include(l);
	return area;
}

Section* Section::LoadXML(tinyxml2::XMLElement* element, MapObjectContainer* container)
{
	Section* section = new Section(container);

	int type;
	if(element->QueryIntAttribute("type", &type) == tinyxml2::XMLError::XML_NO_ERROR)
		section->mProperties.Set("type", type);
	int difficulty;
	if(element->QueryIntAttribute("difficulty", &difficulty) == tinyxml2::XMLError::XML_NO_ERROR)
		section->mProperties.Set("difficulty", difficulty);
	int interest;
	if(element->QueryIntAttribute("interest", &interest) == tinyxml2::XMLError::XML_NO_ERROR)
		section->mProperties.Set("interest", interest);

	tinyxml2::XMLElement* location = element->FirstChildElement("l");
	while(location != nullptr)
	{
		section->mLocations.push_back(Location::LoadXML(location));
		location = location->NextSiblingElement("l");
	}

	tinyxml2::XMLElement* comment = element->FirstChildElement("comment");
	if(comment)
		section->mProperties.Set("comment", std::string(comment->GetText()));
	return section;
}

tinyxml2::XMLElement* Section::SaveXML(tinyxml2::XMLDocument* doc) const
{
	tinyxml2::XMLElement* section = doc->NewElement("section");

	if(mProperties.Exists("type"))
		section->SetAttribute("type", mProperties.Get<int>("type"));
	if(mProperties.Exists("interest"))
		section->SetAttribute("interest", mProperties.Get<int>("interest"));
	if(mProperties.Exists("difficulty"))
		section->SetAttribute("difficulty", mProperties.Get<int>("difficulty"));

	for(const Location& l : mLocations)
		section->InsertEndChild(l.SaveXML(doc));

	if(mProperties.Exists("comment"))
	{
		tinyxml2::XMLElement* comment = doc->NewElement("comment");
		comment->SetText(mProperties.Get<std::string>("comment").c_str());
		section->InsertEndChild(comment);
	}
	return section;
}

}