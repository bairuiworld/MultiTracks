#include <Windows.h>
#include <gdiplus.h>
#include "Track.h"
#include "Section.h"
#include "WayPoint.h"
#include "MapRenderer.h"
#include "Map.h"
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
EditMode(renderer), mTrack(track), mLastWayPoint(nullptr), mPropGrid(nullptr)
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
	review->GetProperties()
		.Set<int>("color", Gdiplus::Color::Red)
		.Set<float>("linewidth", 3.f);
	mSectionSelector = new SectionSelector; 
	mCurrentSectionSelector = new SectionSelector;
	mSectionSelector->SetPriority(1);
	mSectionSelector->Add({review});
	mCurrentSectionSelector->Add({review});
	mSectionSelector->SignalSelection += [this](Section* section, const Vector2d& nearest) {
		section->GetProperties().Push().Set<int>("color", Gdiplus::Color::Yellow);
		mMapRenderer->Invalidate();
	};
	mSectionSelector->SignalDeselection += [this](Section* section, const Vector2d& nearest) {
		section->GetProperties().Pop();
		mMapRenderer->Invalidate();
	};

	mCurrentSectionSelector->SignalSelection += [this](Section* section, const Vector2d& nearest) {
		mSelectionSection = new Section(*section);
		mSelectionSection->GetProperties().Set<int>("color", 0xffffffff).Set<float>("linewidth", 6.f);
		mMapRenderer->AddComponent(mSelectionSection);
		mMapRenderer->RemoveComponent(section);
		mMapRenderer->AddComponent(section);
		mPropGrid = new ww::PropertyGrid;
		mMapRenderer->Add(mPropGrid);
		mPropGrid->SetBounds({10, 510, 250, 650});
		ww::ColorProperty* color = new ww::ColorProperty("Couleur", "Section", section->GetProperties().Get<int>("color", 0));
		mPropGrid->AddProperty(color);
		color->SignalPropertyChanged += [this, section](int color) { section->GetProperties().Set("color", color); mMapRenderer->Invalidate(); };
		mMapRenderer->Invalidate();
	};
	mCurrentSectionSelector->SignalDeselection += [this](Section* section, const Vector2d& nearest) {
		if(mPropGrid)
		{
			mMapRenderer->Remove(mPropGrid);
			mMapRenderer->RemoveComponent(mSelectionSection);
			delete mSelectionSection;
			delete mPropGrid;
			mPropGrid = nullptr;
		}
		section->GetProperties().Pop();
		mMapRenderer->Invalidate();
	};

	mMapRenderer->AddComponent(review);
	mMapRenderer->AddSelector(mWayPointSelector, SelectorAction::MouseMove);
	mMapRenderer->AddSelector(mSectionSelector, SelectorAction::MouseMove);
	mMapRenderer->AddSelector(mCurrentSectionSelector, SelectorAction::MouseClick);
	mMapRenderer->Invalidate();
}

TrackReviewMode::~TrackReviewMode()
{
	mMapRenderer->RemoveSelector(mWayPointSelector, SelectorAction::MouseMove);
	mMapRenderer->RemoveSelector(mSectionSelector, SelectorAction::MouseMove);
	mMapRenderer->RemoveSelector(mCurrentSectionSelector, SelectorAction::MouseClick);
	mMapRenderer->RemoveComponent(mTrack->GetReview());
	delete mSectionSelector;
	delete mWayPointSelector;
	delete mCurrentSectionSelector;
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
	subsection->SetParent(mTrack->GetReview());
	mTrack->GetReview()->Add(subsection);
	mSectionSelector->Add({subsection});
	mCurrentSectionSelector->Add({subsection});

	mMapRenderer->RemoveComponent(mLastWayPoint->GetLocation());
	delete mLastWayPoint;
	mLastWayPoint = nullptr;

	mMapRenderer->Invalidate();
}

}