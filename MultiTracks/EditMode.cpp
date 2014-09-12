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
	mSlotId = mMapRenderer->SignalMapClick += sig::slot(this, &EditMode::OnMapClick);
}

EditMode::~EditMode()
{
	mMapRenderer->SignalMapClick -= mSlotId;
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
	const Location* end = mTrack->GetLastLocation();
	if(end) mMapRenderer->RemoveComponent(end);
	mMapRenderer->Invalidate();
}

void TrackEditMode::OnMapClick(ww::MouseEvent ev, const Location& location)
{
	const Location* end = mTrack->GetLastLocation();
	if(end) mMapRenderer->RemoveComponent(end);
	mTrack->GetLastSection()->Add(location);
	mMapRenderer->AddComponent(mTrack->GetLastLocation());
	mMapRenderer->Invalidate();
}


TrackReviewMode::TrackReviewMode(WindowMapRenderer* renderer, Track* track) :
EditMode(renderer), mTrack(track), mLastWayPoint(nullptr)
{
	mWayPointSelector = new WayPointSelector;
	mWayPointSelector->Add({track});
	mWayPointSelector->SignalSelection += [this](WayPoint* wp) {
		mMapRenderer->AddComponent(wp->GetLocation());
		mMapRenderer->Invalidate();
	};
	mWayPointSelector->SignalDeselection += [this](WayPoint* wp) {
		mMapRenderer->RemoveComponent(wp->GetLocation());
		mMapRenderer->Invalidate();
	};

	MapObjectContainer* review = mTrack->GetReview();
	review->GetProperties().Set<int>("color", Gdiplus::Color::Red);
	mSectionSelector = new SectionSelector; 
	mSectionSelector->SetPriority(1);
	mSectionSelector->Add({review});
	mSectionSelector->SignalSelection += [this](Section* section, const Vector2d& nearest) {
		section->GetProperties().Push().Set<int>("color", Gdiplus::Color::Yellow);
		mMapRenderer->Invalidate();
	};
	mSectionSelector->SignalDeselection += [this](Section* section, const Vector2d& nearest) {
		section->GetProperties().Pop();
		mMapRenderer->Invalidate();
	};

	mMapRenderer->AddComponent(review);
	mMapRenderer->AddSelector(mWayPointSelector);
	mMapRenderer->AddSelector(mSectionSelector);
	mMapRenderer->Invalidate();
}

TrackReviewMode::~TrackReviewMode()
{
	mMapRenderer->RemoveSelector(mWayPointSelector);
	mMapRenderer->RemoveSelector(mSectionSelector);
	mMapRenderer->RemoveComponent(mTrack->GetReview());
	delete mSectionSelector;
	delete mWayPointSelector;
}

void TrackReviewMode::OnMapClick(ww::MouseEvent ev, const Location& location)
{
	if(ev.GetButton() != ww::MouseButton::Left || ev.GetClicks() != 1) return;
	WayPoint* wp = mWayPointSelector->GetCurrentWayPoint();
	if(!wp) return;
	if(!mLastWayPoint)
	{
		mLastWayPoint = new WayPoint(*wp);
		mMapRenderer->AddComponent(mLastWayPoint->GetLocation());
		return;
	}

	Section* subsection = mTrack->SubSection(mLastWayPoint, wp);
	subsection->GetProperties().Set<int>("color", Gdiplus::Color::Red).Set("linewidth", 3.f);
	mTrack->GetReview()->Add(subsection);
	mSectionSelector->Add({subsection});

	mMapRenderer->RemoveComponent(mLastWayPoint->GetLocation());
	delete mLastWayPoint;
	mLastWayPoint = nullptr;

	mMapRenderer->Invalidate();
}

}