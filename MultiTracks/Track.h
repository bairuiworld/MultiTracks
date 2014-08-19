#ifndef __MULTITRACKS_TRACK_H__
#define __MULTITRACKS_TRACK_H__

#include <string>
#include <vector>
#include "tinyxml2.h"
#include "MapObjectContainer.h"
#include "Component.h"

namespace mt
{

class Track : public MapObjectContainer
{
public:
	Track();
	Track(Track* parent);
	~Track();

	void SetName(std::string name) { mName = name; }
	std::string GetName() const { return mName; }

	Track* NewAlternative();
	const std::vector<Track*>& GetAlternatives() const { return mAlternatives; }

	double GetElevationDifference() const;
	double GetPositiveElevation() const;
	double GetNegativeElevation() const;
	std::vector<double> GetElevationProfile() const;

	double GetLength() const;

	static Track* LoadXML(tinyxml2::XMLElement* element);
	tinyxml2::XMLElement* SaveXML(tinyxml2::XMLDocument* doc);

private:
	std::string mName;
	std::vector<Track*> mAlternatives;
	Track* mParent;
};

}
#endif // !__MULTITRACKS_TRACK_H__
