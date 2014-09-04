#define _USE_MATH_DEFINES
#include <cmath>
#include "MapSource.h"
#include "Location.h"
#include "Algorithm.h"
#include "MapViewport.h"

namespace mt
{

MapViewport::MapViewport(MapSource* mapSource, int zoom, const Vector2d& origin) :
	mMapSource(mapSource), mOrigin(origin)
{
	SetZoom(zoom);
}

MapViewport::MapViewport(const MapViewport& vp) :
mMapSource(vp.mMapSource), mZoom(vp.mZoom), mMapSize(vp.mMapSize), mOrigin(vp.mOrigin), mViewDimension(vp.mViewDimension)
{

}

void MapViewport::SetViewDimension(int width, int height)
{
	mViewDimension.Set({width, height});
	CheckOrigin();
}

void MapViewport::SetView(const Area& view)
{
	mViewDimension.Set({0, 0});
	mOrigin.Set({0, 0});
	SetOrigin(LocationToPixel(view.GetNorthWest()));
	Vector2d br = LocationToPixel(view.GetSouthEast());
	SetViewDimension(std::ceil(br.GetX()), std::ceil(br.GetY()));
}

void MapViewport::SetOrigin(const Vector2d& origin)
{
	mOrigin = origin;
	CheckOrigin();
}

void MapViewport::MoveOrigin(double dx, double dy)
{
	Vector2d v = mOrigin;
	v += {dx, dy};
	SetOrigin(v);
}

void MapViewport::SetZoom(int zoom, const Vector2d& center)
{
	if(zoom > 18 || zoom < 3) return;

	double ratioX = (double)(center.GetX() + mOrigin.GetX())/mMapSize;
	double ratioY = (double)(center.GetY() + mOrigin.GetY())/mMapSize;

	mZoom = zoom;
	mMapSize = static_cast<int>(std::pow(2, mZoom)*mMapSource->GetTileSize());

	SetOrigin(Vector2d{ratioX*mMapSize - center.GetX(), ratioY*mMapSize - center.GetY()});
}

void MapViewport::CheckOrigin()
{
	if(mOrigin.GetX() < 0) mOrigin.SetX(0);
	if(mOrigin.GetY() < 0) mOrigin.SetY(0);
	if(mOrigin.GetX() > mMapSize - mViewDimension.GetX()) mOrigin.SetX(mMapSize - mViewDimension.GetX());
	if(mOrigin.GetY() > mMapSize - mViewDimension.GetY()) mOrigin.SetY(mMapSize - mViewDimension.GetY());
}

Location MapViewport::PixelToLocation(const Vector2d& pixel) const
{
	double ratio = mMapSize/(2*M_PI);
	return Location(toDegrees(std::atan(std::sinh((mMapSize/2 - pixel.GetY() - mOrigin.GetY())/ratio))),
					toDegrees((pixel.GetX() + mOrigin.GetX() - mMapSize/2)/ratio));
}

Vector2d MapViewport::LocationToPixel(const Location& location) const
{
	double ratio = mMapSize/(2*M_PI);
	return Vector2d(ratio*toRadians(location.GetLongitude()) - mOrigin.GetX() + mMapSize/2,
					mMapSize/2 - mOrigin.GetY() - ratio*std::log(std::tan(M_PI/4 + toRadians(location.GetLatitude())/2)));
}

double MapViewport::GetPixelDistance(const Location& l1, const Location& l2) const
{
	Vector2d p1 = LocationToPixel(l1);
	Vector2d p2 = LocationToPixel(l2);
	return GetPixelDistance(p1, p2);
}

double MapViewport::GetPixelDistance(const Location& l1, const Vector2d& p2) const
{
	Vector2d p1 = LocationToPixel(l1);
	return GetPixelDistance(p1, p2);
}

double MapViewport::GetPixelDistance(const Vector2d& p1, const Vector2d& p2) const
{
	return std::sqrt((p1.GetX() - p2.GetX())*(p1.GetX() - p2.GetX()) + (p1.GetY() - p2.GetY())*(p1.GetY() - p2.GetY()));
}

Vector3i MapViewport::GetTileCoordinate(const Location& l) const
{
	int xtile = static_cast<int>(std::floor((l.GetLongitude() + 180)/360 * (1<<mZoom)));
	int ytile = static_cast<int>(std::floor((1 - std::log(std::tan(toRadians(l.GetLatitude())) + 1/std::cos(toRadians(l.GetLatitude()))) / M_PI)/2 * (1<<mZoom)));
	if(xtile < 0) xtile = 0;
	if(xtile >= (1<<mZoom)) xtile = (1<<mZoom) - 1;
	if(ytile < 0) ytile = 0;
	if(ytile >= (1<<mZoom)) ytile = (1<<mZoom) - 1;
	return Vector3i(xtile, ytile, mZoom);
}

Vector3i MapViewport::GetNorthWestTileCoordinate()
{
	int w = static_cast<int>(std::floor( mOrigin.GetX()/mMapSource->GetTileSize() ));
	int h = static_cast<int>(std::floor( mOrigin.GetY()/mMapSource->GetTileSize() ));
	return Vector3i(w, h, mZoom);
}

Vector3i MapViewport::GetSouthEastTileCoordinate()
{
	int w = static_cast<int>(std::floor( (mOrigin.GetX() + mViewDimension.GetX())/mMapSource->GetTileSize() ));
	int h = static_cast<int>(std::floor( (mOrigin.GetY() + mViewDimension.GetY())/mMapSource->GetTileSize() ));
	return Vector3i(w, h, mZoom);
}

Vector2i MapViewport::GetTileOrigin(const Vector3i& coordinate)
{
	return Vector2i(static_cast<int>(std::floor( coordinate.GetX()*mMapSource->GetTileSize() - mOrigin.GetX() )),
					static_cast<int>(std::floor( coordinate.GetY()*mMapSource->GetTileSize() - mOrigin.GetY() )));
}

}