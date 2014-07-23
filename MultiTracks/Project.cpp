#include "Track.h"
#include "Project.h"

namespace mt
{
namespace track
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
	mName = "Unnamed project";
}

bool Project::LoadXML(std::string file)
{
	tinyxml2::XMLDocument doc;
	FILE* hFile;
	hFile = fopen(file.c_str(), "rb");
	if(hFile == nullptr) return false;
	mFile = file;
	doc.LoadFile(hFile);
	LoadXML(&doc);
	fclose(hFile);
	return true;
}

void Project::LoadXML(tinyxml2::XMLDocument* doc)
{
	Clear();
	tinyxml2::XMLElement* project = doc->FirstChildElement("project");
	if(project == nullptr) return;
	mName = project->Attribute("name");

	tinyxml2::XMLElement* e = project->FirstChildElement("sectiondatabase");
	if(e != nullptr) LoadSectionDatabaseXML(e);

	e = project->FirstChildElement("trackgroup");
	while(e)
	{
		LoadTrackGroupXML(e);
		e->NextSiblingElement();
	}

	e = project->FirstChildElement("map");
	if(e != nullptr) LoadMapXML(e);
}

void Project::LoadSectionDatabaseXML(tinyxml2::XMLElement* db)
{
	// TODO load attributes
	tinyxml2::XMLElement* e = db->FirstChildElement("section");
	while(e)
	{
		// TODO
		e = e->NextSiblingElement();
	}
}

void Project::LoadTrackGroupXML(tinyxml2::XMLElement* group)
{
	std::string groupName = group->Attribute("name");
	mTracks[groupName] = TrackList();
	TrackList& list = mTracks[groupName];

	tinyxml2::XMLElement* e = group->FirstChildElement("track");
	while(e)
	{
		Track* track = Track::LoadXML(e);
		list.push_back(track);
		e = e->NextSiblingElement();
	}
}

void Project::LoadMapXML(tinyxml2::XMLElement* map)
{

}

}
}