#include "Area.h"

namespace mt
{

Area::Area() :
mNorthWest(-90, 180), mSouthEast(90, -180)
{

}

void Area::Include(const Location& loc)
{
	if(loc.GetLatitude() > mNorthWest.GetLatitude()) mNorthWest.SetLatitude(loc.GetLatitude());
	if(loc.GetLatitude() < mSouthEast.GetLatitude()) mSouthEast.SetLatitude(loc.GetLatitude());
	if(loc.GetLongitude() > mSouthEast.GetLongitude()) mSouthEast.SetLongitude(loc.GetLongitude());
	if(loc.GetLongitude() < mNorthWest.GetLongitude()) mNorthWest.SetLongitude(loc.GetLongitude());
}

void Area::Include(const Area& area)
{
	if(area.mNorthWest.GetLatitude() > mNorthWest.GetLatitude()) mNorthWest.SetLatitude(area.mNorthWest.GetLatitude());
	if(area.mSouthEast.GetLatitude() < mSouthEast.GetLatitude()) mSouthEast.SetLatitude(area.mSouthEast.GetLatitude());
	if(area.mSouthEast.GetLongitude() > mSouthEast.GetLongitude()) mSouthEast.SetLongitude(area.mSouthEast.GetLongitude());
	if(area.mNorthWest.GetLongitude() < mNorthWest.GetLongitude()) mNorthWest.SetLongitude(area.mNorthWest.GetLongitude());
}

}