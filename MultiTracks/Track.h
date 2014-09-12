#ifndef __MULTITRACKS_TRACK_H__
#define __MULTITRACKS_TRACK_H__

#include <string>
#include <vector>
#include "Area.h"
#include "tinyxml2.h"
#include "MapObjectContainer.h"
#include "Component.h"

namespace mt
{

class WayPoint;

class Track : public MapObjectContainer
{
public:
	Track(Track* parent = nullptr);
	virtual ~Track();

	void SetName(std::string name) { mName = name; }
	std::string GetName() const { return mName; }

	Track* NewAlternative();
	const std::vector<Track*>& GetAlternatives() const { return mAlternatives; }

	const Location* GetFirstLocation() const;
	const Location* GetLastLocation() const;
	Section* GetLastSection();

	double GetElevationDifference() const;
	double GetPositiveElevation() const;
	double GetNegativeElevation() const;
	std::vector<double> GetElevationProfile() const;

	double GetLength() const;

	Area GetBoundingBox() const;

	static Track* LoadXML(tinyxml2::XMLElement* element, Track* parent = nullptr);
	tinyxml2::XMLElement* SaveXML(tinyxml2::XMLDocument* doc);

	MapObjectContainer* GetReview();
	Section* SubSection(WayPoint* wp1, WayPoint* wp2);

private:
	std::string mName;
	MapObjectContainer* mReview;
	std::vector<Track*> mAlternatives;
	Track* mParent;
};

}
#endif // !__MULTITRACKS_TRACK_H__
