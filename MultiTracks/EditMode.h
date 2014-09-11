#ifndef __MULTITRACKS_EDITMODE_H__
#define __MULTITRACKS_EDITMODE_H__

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
	Track* mTrack;
	WayPointSelector* mWayPointSelector;
	SectionSelector* mSectionSelector;

	WayPoint* mLastWayPoint;
};

}

#endif // !__MULTITRACKS_EDITMODE_H__