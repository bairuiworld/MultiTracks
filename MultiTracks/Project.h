#ifndef __MULTITRACKS_PROJECT_H__
#define __MULTITRACKS_PROJECT_H__

#include <vector>
#include <map>

#include "tinyxml2.h"

namespace mt
{

class Track;

class Project
{
public:
	using TrackList = std::vector<Track*>;
	using TrackMap = std::map<std::string, TrackList>;

public:
	Project();
	~Project();

	void SetName(std::string name) { mName = name; }
	std::string GetName() const { return mName; }

	void AddTrack(Track* track, std::string group);
	const TrackList& GetTracks(std::string group) const;
	std::vector<std::string> GetGroups() const;
	const MapObjectContainer& GetDatabase() const { return mDatabase; }

	void Clear();

	bool LoadXML(std::string file);
	void LoadXML(tinyxml2::XMLDocument* doc);

private:
	void LoadDatabaseXML(tinyxml2::XMLElement* db);
	void LoadTrackGroupXML(tinyxml2::XMLElement* group);
	void LoadMapXML(tinyxml2::XMLElement* map);

private:
	std::string mFile;
	std::string mName;
	MapObjectContainer mDatabase;
	TrackMap mTracks;
};

}

#endif // !__MULTITRACKS_PROJECT_H__
