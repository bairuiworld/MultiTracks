#include "Point3D.h"
#include "MapSource.h"

namespace mt
{
namespace map
{

MapSource MapSource::MAPS("maps", 
						  "http://mt0.google.com/vt/lyrs=m@121&hl=fr&x=%x&y=%y&z=%z",
						  256,
						 "User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10.4; en-US; rv:1.9.2.2) Gecko/20100316 Firefox/3.6.2");

MapSource MapSource::IGN("ign",
						 "http://wxs.ign.fr/6qcguib31zj0w42ht311a1lz/geoportail/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=GEOGRAPHICALGRIDSYSTEMS.MAPS&STYLE=normal&FORMAT=image/jpeg&TILEMATRIXSET=PM&TILEMATRIX=%z&TILEROW=%y&TILECOL=%x.jpg",
						 256,
						 "Referer: http://m.geoportail.fr/index.html",
						 "User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:31.0) Gecko/20100101 Firefox/31.0");
				

MapSource::MapSource(std::string name, std::string url, int tilesize, std::string header, std::string header2) :
	mName(name), mHeaders(nullptr), mUrl(url), mTileSize(tilesize)
{
	AddHeader(header);
	if(header2 != "")
		AddHeader(header2);
}

MapSource::~MapSource()
{
	curl_slist_free_all(mHeaders);
}

void MapSource::AddHeader(std::string header)
{
	mHeaders = curl_slist_append(mHeaders, header.c_str());
}

void MapSource::InitSession(CURL* curl, const Point3D& coord)
{
	std::string url = mUrl;
	url.replace(url.find("%x"), 2, std::to_string(coord.x));
	url.replace(url.find("%y"), 2, std::to_string(coord.y));
	url.replace(url.find("%z"), 2, std::to_string(coord.z));
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	if(mHeaders != nullptr)
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, mHeaders);
}

}
}