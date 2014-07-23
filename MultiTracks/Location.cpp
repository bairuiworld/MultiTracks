#include <cmath>
#include "Algorithm.h"
#include "Location.h"

namespace mt {
namespace track {

const double Location::mEpsilon = 1./(3600*1000); // 0.001"
const double Location::mEarthRadius = 6371; // km

Location::Location() { Set(0, 0); }
Location::Location(double latitude, double longitude, double elevation) { Set(latitude, longitude, elevation); }
Location::Location(const Location& location) { Set(location.mLatitude, location.mLongitude, location.mElevation); }

void Location::Set(double latitude, double longitude, double elevation)
{
	mLatitude = latitude;
	mLongitude = longitude;
	mElevation = elevation;
}

double Location::GetDistance(const Location& l)
{
	double K1 = sin(math::toRadians(l.mLatitude - mLatitude)/2);
    double K2 = sin(math::toRadians(l.mLongitude - mLongitude)/2);
    return mEarthRadius*2*asin(sqrt(K1*K1 + cos(math::toRadians(l.mLatitude))*cos(math::toRadians(mLatitude))*K2*K2));
}

bool Location::operator ==(const Location& rhs) const
{
	return abs(mLatitude - rhs.mLatitude) < mEpsilon
		&& abs(mLongitude - rhs.mLongitude) < mEpsilon;
}

bool Location::operator !=(const Location& rhs) const
{
	return !(*this == rhs);
}

Location* Location::LoadXML(tinyxml2::XMLElement* element)
{
	Location* loc = new Location;
	element->QueryDoubleAttribute("lat", &loc->mLatitude);
	element->QueryDoubleAttribute("lon", &loc->mLongitude);
	element->QueryDoubleAttribute("ele", &loc->mElevation);
	return loc;
}

tinyxml2::XMLElement* Location::SaveXML(tinyxml2::XMLDocument* doc)
{
	tinyxml2::XMLElement* l = doc->NewElement("l");
	l->SetAttribute("lat", mLatitude);
	l->SetAttribute("lon", mLongitude);
	if(mLongitude != 0)
		l->SetAttribute("ele", mElevation);
	return l;
}

} // track
} // mt