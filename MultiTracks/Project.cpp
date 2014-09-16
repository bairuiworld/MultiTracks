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
	if(mTracks.find(group) == mTracks.end())
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
	mFile = file;
	tinyxml2::XMLDocument doc;
	doc.LoadFile(file.c_str());
	if(doc.Error()) return false;
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
	if(db->QueryIntAttribute(prop::Color::name, &color) == tinyxml2::XMLError::XML_NO_ERROR)
		mDatabase.GetProperties().Set<prop::Color>(color);

	float linewidth;
	if(db->QueryFloatAttribute(prop::LineWidth::name, &linewidth) == tinyxml2::XMLError::XML_NO_ERROR)
		mDatabase.GetProperties().Set<prop::LineWidth>(linewidth);

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

void Project::Save()
{
	Save(mFile);
}

void Project::Save(std::string file)
{
	tinyxml2::XMLDocument doc;
	
	tinyxml2::XMLElement* root = doc.NewElement("multitracks");
	doc.InsertEndChild(root);
	tinyxml2::XMLElement* project = doc.NewElement("project");
	project->SetAttribute("name", mName.c_str());
	root->InsertEndChild(project);

	tinyxml2::XMLElement* sectionDatabase = doc.NewElement("database");
	SaveDatabase(&doc, sectionDatabase);
	SaveTrackGroups(&doc, project);

	doc.SaveFile(file.c_str());
}

void Project::SaveDatabase(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* db)
{
	if(mDatabase.GetProperties().Exists(prop::Color::name))
	{
		int color = mDatabase.GetProperties().Get<prop::Color>();
		db->SetAttribute(prop::Color::name, color);
	}

	if(mDatabase.GetProperties().Exists(prop::LineWidth::name))
	{
		float lineWidth = mDatabase.GetProperties().Get<prop::LineWidth>();
		db->SetAttribute(prop::LineWidth::name, lineWidth);
	}

	for(Section* s : mDatabase.GetSections())
		db->InsertEndChild(s->SaveXML(doc));
}

void Project::SaveTrackGroups(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* project)
{
	for(auto entry : mTracks)
	{
		tinyxml2::XMLElement* group = doc->NewElement("trackgroup");
		project->InsertEndChild(group);
		group->SetAttribute("name", entry.first.c_str());
		for(Track* t : entry.second)
			group->InsertEndChild(t->SaveXML(doc));
	}
}

}