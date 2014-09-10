#ifndef __MULTITRACKS_EDITMODE_H__
#define __MULTITRACKS_EDITMODE_H__

namespace mt
{

class WindowMapRenderer;

class EditMode
{
public:
	EditMode(WindowMapRenderer* renderer);
	virtual ~EditMode() = default;

	virtual void AppendLocation(const Location& location) {};

protected:
	WindowMapRenderer* mMapRenderer;
};

class Track;
class TrackEditMode : public EditMode
{
public:
	TrackEditMode(WindowMapRenderer* renderer, Track* track);
	virtual ~TrackEditMode();

	virtual void AppendLocation(const Location& location);

protected:
	Track* mTrack;
};

class TrackReviewMode : public EditMode
{
public:
	TrackReviewMode(WindowMapRenderer* renderer, Track* track);
	virtual ~TrackReviewMode() = default;

protected:
	Track* mTrack;
	WayPointSelector* mWPSelector;
};

}

#endif // !__MULTITRACKS_EDITMODE_H__