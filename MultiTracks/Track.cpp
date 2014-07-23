#include "Section.h"
#include "Location.h"
#include "Track.h"

namespace mt
{
namespace track
{

Track::Track() : mName("Unnamed track"), mParent(nullptr)
{

}

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

Track* Track::LoadXML(tinyxml2::XMLElement* element)
{
	Track* track = new Track();
	track->mName = element->Attribute("name");

	/*Attribute color = trackElement.getAttribute(COLOR);
	if(color != null) try { track.getProperties().set(COLOR, new Color(color.getIntValue())); } catch(DataConversionException e) {}
	Attribute lineWidth = trackElement.getAttribute(LINEWIDTH);
	if(lineWidth != null) try { track.getProperties().set(LINEWIDTH, lineWidth.getIntValue()); } catch(DataConversionException e) {}
	*/
		
	tinyxml2::XMLElement* e = element->FirstChildElement("section");
	while(e)
	{
		Section* s = new Section;
		s->LoadXML(e);
		track->mSections.push_back(s);
		e = e->NextSiblingElement();
	}
				
	e = element->FirstChildElement("spots");
	if(e)
	{
		e = e->FirstChildElement("l");
		while(e)
		{
			Location* l = Location::LoadXML(e);
			track->mLocations.push_back(l);
			e = e->NextSiblingElement();
		}
	}
		
	e = element->FirstChildElement("alternatives");
	while(e)
	{
		Track* alternative = Track::LoadXML(e);
		track->mAlternatives.push_back(alternative);
		e = e->NextSiblingElement();
	}

	return track;
}
	
tinyxml2::XMLElement* Track::SaveXML(tinyxml2::XMLDocument* doc)
{
	tinyxml2::XMLElement* track = mParent == nullptr ? doc->NewElement("track") : doc->NewElement("alternative");
	track->SetAttribute("name", mName.c_str());
	/*if(mProperties.exists(COLOR))
	{
		Color color = mProperties.get(COLOR);
		track.setAttribute(COLOR, Integer.toString(color.getRGB()));
	}
		
	if(mProperties.exists(LINEWIDTH))
	{
		int lineWidth = mProperties.get(LINEWIDTH);
		track.setAttribute(LINEWIDTH, Integer.toString(lineWidth));
	}*/

	if(mSections.size() != 0)
	{
		for(Section* s : mSections)
			track->InsertEndChild(s->SaveXML(doc));
	}
		
	if(mLocations.size() != 0)
	{
		tinyxml2::XMLElement* spotsElement = doc->NewElement("spots");
		for(Location* l : mLocations)
			spotsElement->InsertEndChild(l->SaveXML(doc));
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
}