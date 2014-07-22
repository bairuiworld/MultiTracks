#ifndef __MULTITRACKS_LOCATION_H__
#define __MULTITRACKS_LOCATION_H__

#include "tinyxml2.h"
#include "Properties.h"

namespace mt {
namespace track {

class Location
{
protected:
	static const double mEpsilon;
	static const double mEarthRadius;

public:
	Location();
	Location(double latitude, double longitude, double elevation = 0);
	Location(const Location& location);

	void Set(double latitude, double longitude, double elevation = 0);
	void SetLatitude(double latitude)	{ mLatitude = latitude; }
	void SetLongitude(double longitude) { mLongitude = longitude; }
	void SetElevation(double elevation) { mElevation = elevation; }
	double GetLatitude()  const { return mLatitude; }
	double GetLongitude() const { return mLongitude; }
	double GetElevation() const { return mElevation; }

	double GetDistance(const Location& l); // km

	bool operator ==(const Location& rhs) const;
	bool operator !=(const Location& rhs) const;

	void LoadXML(tinyxml2::XMLElement* element);
	tinyxml2::XMLElement* SaveXML(tinyxml2::XMLDocument* doc);

protected:
	double mLatitude;
	double mLongitude;
	double mElevation;
};

} // track
} // mt

#endif // __MULTITRACKS_LOCATION_H__