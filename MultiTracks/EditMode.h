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

class Track;
class TrackEditMode : public EditMode
{
public:
	TrackEditMode(WindowMapRenderer* renderer, Track* track);
	virtual ~TrackEditMode();

	virtual void OnMapClick(ww::MouseEvent ev, const Location& location);

protected:
	Track* mTrack;
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
	Section* mSelectionSection;

	WayPoint* mLastWayPoint;
	ww::PropertyGrid* mPropGrid;
};

}

#endif // !__MULTITRACKS_EDITMODE_H__