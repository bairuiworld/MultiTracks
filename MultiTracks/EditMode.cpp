#include <Windows.h>
#include <gdiplus.h>
#include "Track.h"
#include "Section.h"
#include "WayPoint.h"
#include "MapRenderer.h"
#include "EditMode.h"

namespace mt
{

EditMode::EditMode(WindowMapRenderer* renderer) :
mMapRenderer(renderer)
{

}

TrackEditMode::TrackEditMode(WindowMapRenderer* renderer, Track* track) :
EditMode(renderer), mTrack(track)
{
	mTrack->GetProperties().Push()
		.Set<int>("color", Gdiplus::Color::Magenta)
		.Set("linewidth", 3.f);
	mMapRenderer->AddComponent(mTrack);
	
	const Location* end = mTrack->GetLastLocation();
	if(end)
		mMapRenderer->AddComponent(end);
	
	mMapRenderer->Invalidate();
}

TrackEditMode::~TrackEditMode()
{
	mTrack->GetProperties().Pop();
	mMapRenderer->RemoveComponent(mTrack);
	const Location* end = mTrack->GetLastLocation();
	if(end) mMapRenderer->RemoveComponent(end);
	mMapRenderer->Invalidate();
}

void TrackEditMode::AppendLocation(const Location& location)
{
	const Location* end = mTrack->GetLastLocation();
	if(end) mMapRenderer->RemoveComponent(end);
	mTrack->GetLastSection()->Add(location);
	mMapRenderer->AddComponent(mTrack->GetLastLocation());
	mMapRenderer->Invalidate();
}


TrackReviewMode::TrackReviewMode(WindowMapRenderer* renderer, Track* track) :
EditMode(renderer), mTrack(track)
{
	mWPSelector = new WayPointSelector;
	mWPSelector->Add({track});
	mWPSelector->SignalSelection += [this](WayPoint* wp) {
		mMapRenderer->AddComponent(wp->GetLocation());
		mMapRenderer->Invalidate();
	};
	mWPSelector->SignalDeselection += [this](WayPoint* wp) {
		mMapRenderer->RemoveComponent(wp->GetLocation());
		mMapRenderer->Invalidate();
	};
	mMapRenderer->AddSelector(mWPSelector);
}

TrackReviewMode::~TrackReviewMode()
{
	mMapRenderer->RemoveSelector(mWPSelector);
	delete mWPSelector;
}

}