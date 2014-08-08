#ifndef __MULTITRACKS_MAP_H__
#define __MULTITRACKS_MAP_H__

#include <map>
#include "Vector.h"

namespace mt
{

class MapSource;
class MapViewport;
class Tile;

class Map
{
private:
	using TileMap = std::map<Vector3i, Tile*>;
	using TileList = std::list<Tile*>;

public:
	Map(MapSource* mapSource);
	~Map();

	MapViewport* GetViewport();

	Tile* GetTile(const Vector3i& coord);
	int getSize(int zoom);

private:
	MapSource* mMapSource;
	MapViewport* mMapViewport;
	TileMap mCache;
	TileList mCacheUsage;

	unsigned int mCacheSize;
};

}

#endif // !__MULTITRACKS_MAP_H__