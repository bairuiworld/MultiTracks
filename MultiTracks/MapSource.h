#ifndef __MULTITRACKS_MAPSOURCE_H__
#define __MULTITRACKS_MAPSOURCE_H__

#include <vector>
#include <string>
#include <curl\curl.h>

namespace mt
{
namespace map
{

class Point3D;

class MapSource
{
public:
	MapSource(std::string name, std::string url, int tilesize, std::string header, std::string header2 = "");
	~MapSource();

	void AddHeader(std::string header);
	void InitSession(CURL* curl, const Point3D& coord);

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
}

#endif // !__MULTITRACKS_MAPSOURCE_H__
