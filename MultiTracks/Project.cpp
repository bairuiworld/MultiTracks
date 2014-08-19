#include "Track.h"
#include "Section.h"
#include "Project.h"

namespace mt
{

Project::Project() : mName("Nouveau projet")
{

}

Project::~Project()
{
	Clear();
}

void Project::AddTrack(Track* track, std::string group)
{
	if(mTracks.find(group) != mTracks.end())
		mTracks[group] = TrackList();
	mTracks[group].push_back(track);
}

const Project::TrackList& Project::GetTracks(std::string group) const
{
	return mTracks.at(group);
}

std::vector<std::string> Project::GetGroups() const
{
	std::vector<std::string> keys;
	for(auto it : mTracks)
		keys.push_back(it.first);
	return keys;
}

void Project::Clear()
{
	for(auto it : mTracks)
		for(Track* track : it.second)
			delete track;
	mTracks.clear();
	mDatabase.Clear();
	mName = "Unnamed project";
}

bool Project::LoadXML(std::string file)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(file.c_str());
	LoadXML(&doc);
	return true;
}

void Project::LoadXML(tinyxml2::XMLDocument* doc)
{
	Clear();
	tinyxml2::XMLHandle handle(doc);
	tinyxml2::XMLElement* project = handle.FirstChildElement("multitracks").FirstChildElement("project").ToElement();
	if(project == nullptr) return;
	mName = project->Attribute("name");

	tinyxml2::XMLElement* e = project->FirstChildElement("database");
	if(e != nullptr) LoadDatabaseXML(e);

	e = project->FirstChildElement("trackgroup");
	while(e)
	{
		LoadTrackGroupXML(e);
		e = e->NextSiblingElement("trackgroup");
	}

	e = project->FirstChildElement("map");
	if(e != nullptr) LoadMapXML(e);
}

void Project::LoadDatabaseXML(tinyxml2::XMLElement* db)
{
	int color;
	if(db->QueryIntAttribute("color", &color) == tinyxml2::XMLError::XML_NO_ERROR)
		mDatabase.GetProperties().Set("color", color);

	float linewidth;
	if(db->QueryFloatAttribute("linewidth", &linewidth) == tinyxml2::XMLError::XML_NO_ERROR)
		mDatabase.GetProperties().Set("linewidth", linewidth);

	tinyxml2::XMLElement* e = db->FirstChildElement("section");
	while(e)
	{
		Section* section = Section::LoadXML(e, &mDatabase);
		mDatabase.Add(section);
		e = e->NextSiblingElement("section");
	}
}

void Project::LoadTrackGroupXML(tinyxml2::XMLElement* group)
{
	const char* nameAtt = group->Attribute("name");
	if(!nameAtt) return;
	std::string groupName = nameAtt;
	mTracks[groupName] = TrackList();
	TrackList& list = mTracks[groupName];

	tinyxml2::XMLElement* e = group->FirstChildElement("track");
	while(e)
	{
		Track* track = Track::LoadXML(e);
		list.push_back(track);
		e = e->NextSiblingElement("track");
	}
}

void Project::LoadMapXML(tinyxml2::XMLElement* map)
{
	// todo
}

}