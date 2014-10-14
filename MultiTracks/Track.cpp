#include "Section.h"
#include "Location.h"
#include "WayPoint.h"
#include "Track.h"

namespace mt
{

Track::Track(Track* parent) : mName("Unnamed track"), mReview(nullptr), mParent(parent)
{

}

Track::~Track()
{
	for(Track* alt : mAlternatives)
		delete alt;
	delete mReview;
}

Track* Track::NewAlternative()
{
	Track* alt = new Track(this);
	mAlternatives.push_back(alt);
	return alt;
}

const Location* Track::GetFirstLocation() const
{
	if(mSections.size() == 0) return nullptr;
	return mSections.front()->GetFirstLocation();
}

const Location* Track::GetLastLocation() const
{
	if(mSections.size() == 0) return nullptr;
	return mSections.back()->GetLastLocation();
}

Section* Track::GetLastSection()
{
	if(mSections.size() == 0)
		mSections.push_back(new Section(this));
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
	if(element->QueryIntAttribute(prop::Color::name, &color) == tinyxml2::XMLError::XML_NO_ERROR)
		track->GetProperties().Set<prop::Color>(color);
	float linewidth;
	if(element->QueryFloatAttribute(prop::LineWidth::name, &linewidth) == tinyxml2::XMLError::XML_NO_ERROR)
		track->GetProperties().Set<prop::LineWidth>(linewidth);

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

	e = element->FirstChildElement("review");
	if(e) track->mReview = MapObjectContainer::LoadXML(e);

	return track;
}
	
tinyxml2::XMLElement* Track::SaveXML(tinyxml2::XMLDocument* doc)
{
	tinyxml2::XMLElement* track = mParent == nullptr ? doc->NewElement("track") : doc->NewElement("alternative");
	track->SetAttribute("name", mName.c_str());
	
	if(mProperties.Exists(prop::Color::name))
		track->SetAttribute(prop::Color::name, mProperties.Get<prop::Color>());
	if(mProperties.Exists(prop::LineWidth::name))
		track->SetAttribute(prop::LineWidth::name, mProperties.Get<prop::LineWidth>());

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

	if(mReview)
		track->InsertEndChild(mReview->SaveXML(doc, "review"));
	return track;
}

MapObjectContainer* Track::GetReview()
{
	if(!mReview)
		mReview = new MapObjectContainer;
	return mReview;
}

Section* Track::SubSection(WayPoint* wp1, WayPoint* wp2)
{
	if(wp1->GetSection() != wp2->GetSection()) return nullptr;
	Section* sub = new Section;
	bool copy = false;
	Section::LocationList::const_iterator it;
	Section* section = wp1->GetSection();
	for(it = section->GetLocations().begin(); it != section->GetLocations().end(); it++)
	{
		if(copy) sub->Add(*it);

		if(it == wp1->GetAfter())
		{
			sub->Add(*wp1->GetLocation());
			if(copy) return sub;
			copy = true;
		}

		if(it == wp2->GetAfter())
		{
			sub->Add(*wp2->GetLocation());
			if(copy) return sub;
			copy = true;
		}
	}
	return sub;
}

}