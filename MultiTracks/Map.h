#ifndef __MULTITRACKS_MAP_H__
#define __MULTITRACKS_MAP_H__

#include <map>

namespace mt
{
namespace map
{

class MapSource;
class Point3D;
class Tile;

class Map
{
private:
	using TileMap = std::map<Point3D, Tile*>;
	using TileList = std::list<Tile*>;

public:
	Map(MapSource* mapSource);
	~Map();

	Tile* GetTile(const Point3D& coord);

private:
	MapSource* mMapSource;
	TileMap mCache;
	TileList mCacheUsage;

	unsigned int mCacheSize;
};

}
}

#endif // !__MULTITRACKS_MAP_H__