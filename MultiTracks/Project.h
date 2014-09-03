#ifndef __MULTITRACKS_PROJECT_H__
#define __MULTITRACKS_PROJECT_H__

#include <vector>
#include <map>
#include "MapObjectContainer.h"
#include "tinyxml2.h"

namespace mt
{

class Track;

class Project
{
public:
	using TrackList = std::vector<Track*>;
	using TrackMap = std::map<std::string, TrackList>;
	using GroupList = std::vector<std::string>;

public:
	Project();
	~Project();

	void SetName(std::string name) { mName = name; }
	std::string GetName() const { return mName; }

	void AddTrack(Track* track, std::string group);
	const TrackList& GetTracks(std::string group) const;
	GroupList GetGroups() const;
	MapObjectContainer& GetDatabase() { return mDatabase; }

	void Clear();

	bool LoadXML(std::string file);
	void LoadXML(tinyxml2::XMLDocument* doc);

	void Save();
	void Save(std::string file);

private:
	void LoadDatabaseXML(tinyxml2::XMLElement* db);
	void LoadTrackGroupXML(tinyxml2::XMLElement* group);
	void LoadMapXML(tinyxml2::XMLElement* map);

	void SaveDatabase(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* db);
	void SaveTrackGroups(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* project);

private:
	std::string mFile;
	std::string mName;
	MapObjectContainer mDatabase;
	TrackMap mTracks;
};

}

#endif // !__MULTITRACKS_PROJECT_H__
