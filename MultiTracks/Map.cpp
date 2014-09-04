#include "MapSource.h"
#include "MapViewport.h"
#include "Tile.h"
#include "Map.h"

#include <iostream>

namespace mt
{

Map::Map(MapSource* mapSource) :
	mMapSource(mapSource), mCacheSize(500)
{
	mMapViewport = new MapViewport(mMapSource);
}

Map::~Map()
{
	for(Tile* tile : mCacheUsage)
		delete tile;
	delete mMapViewport;
}

Tile* Map::GetTile(const Vector3i& coord, bool download)
{
	TileMap::const_iterator it = mCache.find(coord);
	if(it == mCache.end())
	{
		Tile* tile = new Tile(coord, mMapSource);
		if(!download) return tile;
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

void Map::Draw(Gdiplus::Graphics* g)
{
	Vector3i northWestTile = mMapViewport->GetNorthWestTileCoordinate();
	Vector3i southEastTile = mMapViewport->GetSouthEastTileCoordinate();
	Vector2i origin = mMapViewport->GetTileOrigin(northWestTile);
	int xTileCount = southEastTile.GetX() - northWestTile.GetX() + 1;
	int yTileCount = southEastTile.GetY() - northWestTile.GetY() + 1;
	int tileCount = xTileCount*yTileCount;

	for(int i = 0; i<xTileCount; i++)
	{
		for(int j = 0; j<yTileCount; j++)
		{
			Vector3i coord(northWestTile.GetX() + i, northWestTile.GetY() + j, mMapViewport->GetZoom());
			Tile* tile = GetTile(coord);
			if(!tile->IsLoaded())
			{
				tile->SignalReady += [this](Tile* tile) {
					std::lock_guard<std::mutex> lock(signal_mutex);
					SignalNewTile.emit();
				};
				continue;
			}
			Gdiplus::Image* im = tile->GetImage();
			if(im)
				g->DrawImage(im, origin.GetX() + i*mMapSource->GetTileSize(), origin.GetY() + j*mMapSource->GetTileSize());
		}
	}
}

void Map::SyncDraw(Gdiplus::Graphics* g)
{
	Vector3i northWestTile = mMapViewport->GetNorthWestTileCoordinate();
	Vector3i southEastTile = mMapViewport->GetSouthEastTileCoordinate();
	Vector2i origin = mMapViewport->GetTileOrigin(northWestTile);
	int xTileCount = southEastTile.GetX() - northWestTile.GetX() + 1;
	int yTileCount = southEastTile.GetY() - northWestTile.GetY() + 1;
	int tileCount = xTileCount*yTileCount;

	std::mutex count_lock;
	std::condition_variable cv;
	int tileLeft = 0;

	for(int i = 0; i<xTileCount; i++)
	{
		for(int j = 0; j<yTileCount; j++)
		{
			Vector3i coord(northWestTile.GetX() + i, northWestTile.GetY() + j, mMapViewport->GetZoom());
			Tile* tile = GetTile(coord, false);
			if(!tile->IsLoaded())
			{
				{
					std::lock_guard<std::mutex> lock(count_lock);
					tileLeft++;
				}
				tile->Download(true);

				tile->SignalReady += [this, &count_lock, &tileLeft, &cv, &origin, g, i, j](Tile* tile) {
					std::lock_guard<std::mutex> lock(count_lock);
					/*Gdiplus::Image* im = tile->GetImage();
					if(im)
						g->DrawImage(im, origin.GetX() + i*mMapSource->GetTileSize(), origin.GetY() + j*mMapSource->GetTileSize());*/
					tileLeft--;
					tile->Dispose();
					if(tileLeft <= 0)
						cv.notify_all();
				};
			}
			else
			{
				Gdiplus::Image* im = tile->GetImage();
				if(im)
				{
					//std::lock_guard<std::mutex> lock(count_lock);
					g->DrawImage(im, origin.GetX() + i*mMapSource->GetTileSize(), origin.GetY() + j*mMapSource->GetTileSize());
				}
			}
		}
	}

	std::unique_lock<std::mutex> lock(count_lock);
	if(tileLeft <= 0) return;
	while(tileLeft > 0)
		cv.wait(lock);
}

}