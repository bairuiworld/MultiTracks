#ifndef __MULTITRACKS_MAPSOURCE_H__
#define __MULTITRACKS_MAPSOURCE_H__

#include <vector>
#include <string>
#include <curl\curl.h>

namespace mt
{
namespace map
{

class MapSource
{
public:
	MapSource(std::string name, std::string url, int tilesize, std::string header);
	~MapSource();

	void AddHeader(std::string header);
	void InitSession(CURL* curl, const Point3D& coord);

public:
	static MapSource MAPS;

private:
	std::string mName;
	curl_slist* mHeaders;
	std::string mUrl;
	int mTileSize;
};

}
}

#endif // !__MULTITRACKS_MAPSOURCE_H__
