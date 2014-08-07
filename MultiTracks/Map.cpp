#include "MapSource.h"
#include "Tile.h"
#include "Map.h"

namespace mt
{
namespace map
{

Map::Map(MapSource* mapSource) :
	mMapSource(mapSource), mCacheSize(2)
{

}

Map::~Map()
{
	for(Tile* tile : mCacheUsage)
		delete tile;
}

Tile* Map::GetTile(const Point3D& coord)
{
	TileMap::const_iterator it = mCache.find(coord);
	if(it == mCache.end())
	{
		Tile* tile = new Tile(coord, mMapSource);
		tile->Download(true);
		if(mCache.size() >= mCacheSize)
		{
			Tile* lessUsed = mCacheUsage.back();
			mCache.erase(lessUsed->GetCoordinates());
			mCacheUsage.pop_back();
			delete lessUsed;
		}
		mCache.insert({coord, tile});
		mCacheUsage.push_front(tile);
		return tile;
	}
	else
	{
		TileList::const_iterator uit = std::find(mCacheUsage.begin(), mCacheUsage.end(), it->second);
		if(uit != mCacheUsage.begin())
			mCacheUsage.splice(mCacheUsage.begin(), mCacheUsage, uit, std::next(uit));
		return it->second;
	}
}

}
}