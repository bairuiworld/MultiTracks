#ifndef __MULTITRACKS_MAPVIEWPORT_H__
#define __MULTITRACKS_MAPVIEWPORT_H__

#include "Vector.h"
#include "Area.h"

namespace mt
{

class Location;
class MapSource;

class MapViewport
{
public:
	MapViewport(MapSource* mapSource, int zoom = 0, const Vector2d& origin = Vector2d(0, 0));
	MapViewport(const MapViewport& vp);

	void SetViewDimension(int width, int height);
	int GetWidth() const { return mViewDimension.GetX(); }
	int GetHeight() const { return mViewDimension.GetY(); }

	void SetView(const Area& view, int padding = 0);
	void FitToArea(const Area& view);

	Vector2d GetOrigin() const { return mOrigin; }
	void SetOrigin(const Vector2d& origin);
	void MoveOrigin(double dx, double dy);

	int GetZoom() const { return mZoom; }
	void SetZoom(int zoom, const Vector2d& center = Vector2d(0, 0));

	int GetMapSize() const { return mMapSize; }
	Location PixelToLocation(const Vector2d& pixel) const;
	Vector2d LocationToPixel(const Location& location) const;

	double GetPixelDistance(const Location& l1, const Location& l2) const;
	double GetPixelDistance(const Location& l1, const Vector2d& p2) const;
	double GetPixelDistance(const Vector2d& p1, const Vector2d& p2) const;

	Vector3i GetTileCoordinate(const Location& l) const;
	Vector3i GetNorthWestTileCoordinate();
	Vector3i GetSouthEastTileCoordinate();
	Vector2i GetTileOrigin(const Vector3i& coordinate);

private:
	void CheckOrigin();

private:
	MapSource* mMapSource;
	int mZoom;
	int mMapSize;
	Vector2d mOrigin;
	Vector2i mViewDimension;
};

} // namespace mt

#endif // !__MULTITRACKS_MAPVIEWPORT_H__