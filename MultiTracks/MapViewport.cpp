#define _USE_MATH_DEFINES
#include <cmath>
#include "MapSource.h"
#include "Location.h"
#include "Algorithm.h"
#include "MapViewport.h"

namespace mt
{

MapViewport::MapViewport(MapSource* mapSource, int zoom, const Vector2d& origin) :
	mMapSource(mapSource), mZoom(zoom), mOrigin(origin)
{

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

}