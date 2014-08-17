#ifndef __MULTITRACKS_MAP_H__
#define __MULTITRACKS_MAP_H__

#include <Windows.h>
#include <gdiplus.h>
#include <map>
#include <list>
#include <mutex>
#include "Vector.h"
#include "SimpleSignal.h"

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

	MapViewport* GetViewport() const { return mMapViewport; };

	Tile* GetTile(const Vector3i& coord);

	void Draw(Gdiplus::Graphics* g);

	sig::Signal<void()> SignalNewTile;

private:
	MapSource* mMapSource;
	MapViewport* mMapViewport;
	TileMap mCache;
	TileList mCacheUsage;
	std::mutex signal_mutex;

	unsigned int mCacheSize;
};

}

#endif // !__MULTITRACKS_MAP_H__