#ifndef __MULTITRACKS_EDITMODE_H__
#define __MULTITRACKS_EDITMODE_H__

#include "PropertyGrid.h"

namespace mt
{

class WindowMapRenderer;

class EditMode
{
public:
	EditMode(WindowMapRenderer* renderer);
	virtual ~EditMode();

	virtual void OnMapClick(ww::MouseEvent ev, const Location& location) {}

protected:
	WindowMapRenderer* mMapRenderer;
	int mSlotId;
};

class MapObjectContainer;
class ContainerEditMode : public EditMode
{
public:
	ContainerEditMode(WindowMapRenderer* renderer, MapObjectContainer* container);
	virtual ~ContainerEditMode();

	virtual void OnMapClick(ww::MouseEvent ev, const Location& location);

protected:
	void OnSelectSectionEnd(Section* section, Location* sectionEnd);
	void OnDeselectSectionEnd(Section* section, Location* sectionEnd);
	void OnSectionClick(WayPoint* wp);
	void OnMouseMove(ww::MouseEvent ev);

	void ClearCurrentLocation();

protected:
	MapObjectContainer* mContainer;
	SectionEndSelector* mSectionEndSelector;
	WayPointSelector* mWayPointSelector;
	Section* mCurrentSection;
	Location* mCurrentLocation;
};

class TrackReviewMode : public EditMode
{
public:
	TrackReviewMode(WindowMapRenderer* renderer, Track* track);
	virtual ~TrackReviewMode();

	virtual void OnMapClick(ww::MouseEvent ev, const Location& location);

protected:
	void OnSelectSection(Section* section, const Vector2d& nearest);
	void OnDeselectSection(Section* section, const Vector2d& nearest);
	void OnSelectWayPoint(WayPoint* wp);
	void OnDeselectWayPoint(WayPoint* wp);
	void OnSelectCurrentSection(Section* section, const Vector2d& nearest);
	void OnDeselectCurrentSection(Section* section, const Vector2d& nearest);

protected:
	Track* mTrack;
	WayPointSelector* mWayPointSelector;
	SectionSelector* mSectionSelector;
	SectionSelector* mCurrentSectionSelector;
	Section* mHoverSection;
	Section* mSelectionSection;

	WayPoint* mLastWayPoint;
	ww::PropertyGrid* mPropGrid;
};

}

#endif // !__MULTITRACKS_EDITMODE_H__