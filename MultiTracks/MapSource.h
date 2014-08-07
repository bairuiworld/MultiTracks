#ifndef __MULTITRACKS_MAPSOURCE_H__
#define __MULTITRACKS_MAPSOURCE_H__

#include <vector>
#include <string>
#include <curl\curl.h>
#include "Vector.h"

namespace mt
{

class MapSource
{
public:
	MapSource(std::string name, std::string url, int tilesize, std::string header, std::string header2 = "");
	~MapSource();

	void AddHeader(std::string header);
	void InitSession(CURL* curl, const Vector3i& coord);

	int GetTileSize() const { return mTileSize; }

public:
	static MapSource MAPS;
	static MapSource IGN;

private:
	std::string mName;
	curl_slist* mHeaders;
	std::string mUrl;
	int mTileSize;
};

}

#endif // !__MULTITRACKS_MAPSOURCE_H__
