#include "Section.h"
#include "Location.h"
#include "Track.h"

namespace mt
{

Track::Track(Track* parent) : mName("Unnamed track"), mParent(parent)
{

}

Track::~Track()
{
	for(Track* alt : mAlternatives)
		delete alt;
}

Track* Track::NewAlternative()
{
	Track* alt = new Track(this);
	mAlternatives.push_back(alt);
	return alt;
}

const Location* Track::GetFirstLocation() const
{
	return mSections.front()->GetFirstLocation();
}

const Location* Track::GetLastLocation() const
{
	return mSections.back()->GetLastLocation();
}

Section* Track::GetLastSection()
{
	return mSections.back();
}

double Track::GetElevationDifference() const
{
	if(mSections.size() <= 1) return 0;
	const Location* first = mSections.front()->GetFirstLocation();
	const Location* last = mSections.back()->GetLastLocation();
	if(first == nullptr || last == nullptr) return 0;
	return last->GetElevation() - first->GetElevation();
}

double Track::GetPositiveElevation() const
{
	double e = 0;
	for(Section* s : mSections)
		e += s->GetPositiveElevation();
	return e;
}

double Track::GetNegativeElevation() const
{
	double e = 0;
	for(Section* s : mSections)
		e += s->GetNegativeElevation();
	return e;
}

std::vector<double> Track::GetElevationProfile() const
{
	std::vector<double> profile;
	for(Section* s : mSections)
	{
		std::vector<double> sectionProfile = s->GetElevationProfile();
		profile.insert(profile.end(), sectionProfile.begin(), sectionProfile.end());
	}
	return profile;
}

double Track::GetLength() const
{
	double len = 0;
	for(Section* s : mSections)
		len += s->GetLength();
	return len;
}

Area Track::GetBoundingBox() const
{
	Area area;
	for(Section* s : mSections)
		area.Include(s->GetBoundingBox());
	return area;
}

Track* Track::LoadXML(tinyxml2::XMLElement* element, Track* parent)
{
	Track* track = new Track(parent);
	track->mName = element->Attribute("name");

	int color;
	if(element->QueryIntAttribute("color", &color) == tinyxml2::XMLError::XML_NO_ERROR)
		track->GetProperties().Set("color", color);
	float linewidth;
	if(element->QueryFloatAttribute("linewidth", &linewidth) == tinyxml2::XMLError::XML_NO_ERROR)
		track->GetProperties().Set("linewidth", linewidth);

		
	tinyxml2::XMLElement* e = element->FirstChildElement("section");
	while(e)
	{
		Section* s = Section::LoadXML(e, track);
		track->mSections.push_back(s);
		e = e->NextSiblingElement("section");
	}
				
	e = element->FirstChildElement("spots");
	if(e)
	{
		e = e->FirstChildElement("l");
		while(e)
		{
			track->mLocations.push_back(Location::LoadXML(e));
			e = e->NextSiblingElement("l");
		}
	}
		
	e = element->FirstChildElement("alternatives");
	while(e)
	{
		Track* alternative = Track::LoadXML(e, track);
		track->mAlternatives.push_back(alternative);
		e = e->NextSiblingElement("alternatives");
	}

	return track;
}
	
tinyxml2::XMLElement* Track::SaveXML(tinyxml2::XMLDocument* doc)
{
	tinyxml2::XMLElement* track = mParent == nullptr ? doc->NewElement("track") : doc->NewElement("alternative");
	track->SetAttribute("name", mName.c_str());
	
	if(mProperties.Exists("color"))
		track->SetAttribute("color", mProperties.Get<int>("color"));
	if(mProperties.Exists("linewidth"))
		track->SetAttribute("linewidth", mProperties.Get<float>("linewidth"));

	if(mSections.size() != 0)
	{
		for(Section* s : mSections)
			track->InsertEndChild(s->SaveXML(doc));
	}
		
	if(mLocations.size() != 0)
	{
		tinyxml2::XMLElement* spotsElement = doc->NewElement("spots");
		for(const Location& l : mLocations)
			spotsElement->InsertEndChild(l.SaveXML(doc));
		track->InsertEndChild(spotsElement);
	}
		
	if(mAlternatives.size() != 0)
	{
		for(Track* t : mAlternatives)
			track->InsertEndChild(t->SaveXML(doc));
	}
	return track;
}

}