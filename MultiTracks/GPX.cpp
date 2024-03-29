#include "tinyxml2.h"
#include "Track.h"
#include "Section.h"
#include "Location.h"
#include "GPX.h"

namespace mt
{

Track* GPX::Load(const char* filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename);

	tinyxml2::XMLHandle handle(&doc);
	tinyxml2::XMLElement* trackElt = handle.FirstChildElement("gpx").FirstChildElement("trk").ToElement();
	if(trackElt == nullptr) return nullptr;
	Track* track = new Track();
	
	tinyxml2::XMLElement* nameElt = trackElt->FirstChildElement("name");
	if(nameElt != nullptr) track->SetName(nameElt->GetText());

	tinyxml2::XMLElement* segment = trackElt->FirstChildElement("trkseg");
	while(segment)
	{
		Section* section = new Section(track);
		tinyxml2::XMLElement* wayPoint = segment->FirstChildElement("trkpt");
		while(wayPoint)
		{
			double lat, lon, ele = 0;
			if(wayPoint->QueryDoubleAttribute("lat", &lat) != tinyxml2::XMLError::XML_NO_ERROR) continue;
			if(wayPoint->QueryDoubleAttribute("lon", &lon) != tinyxml2::XMLError::XML_NO_ERROR) continue;
			Location loc(lat, lon);
			tinyxml2::XMLElement* eleElt = wayPoint->FirstChildElement("ele");
			if(eleElt != nullptr)
			{
				const char* txt = eleElt->GetText();
				try
				{
					double d = std::stod(txt);
					loc.SetElevation(d);
				}
				catch(...) {}
			}
			section->Add(loc);
			wayPoint = wayPoint->NextSiblingElement();
		}
		track->Add(section);
		segment = segment->NextSiblingElement();
	}
	return track;
}

void GPX::Save(const char* filename, Track* track)
{
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLElement* root = doc.NewElement("gpx");
	doc.InsertEndChild(root);
	tinyxml2::XMLElement* trk = doc.NewElement("trk");
	root->InsertEndChild(trk);
	tinyxml2::XMLElement* name = doc.NewElement("name");
	name->SetText(track->GetName().c_str());
	trk->InsertEndChild(name);

	for(Section* section : track->GetSections())
	{
		tinyxml2::XMLElement* trkseg = doc.NewElement("trkseg");
		for(const Location& location : section->GetLocations())
		{
			tinyxml2::XMLElement* trkpt = doc.NewElement("trkpt");
			trkpt->SetAttribute("lat", location.GetLatitude());
			trkpt->SetAttribute("lon", location.GetLongitude());
			trkseg->InsertEndChild(trkpt);
		}
		trk->InsertEndChild(trkseg);
	}
	doc.SaveFile(filename);
}

}