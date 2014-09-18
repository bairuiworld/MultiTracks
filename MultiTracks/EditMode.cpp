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
		.Set<prop::Color>(Gdiplus::Color::Magenta)
		.Set<prop::LineWidth>(3);
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
	mMapRenderer->RemoveComponent(mTrack);
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
EditMode(renderer), mTrack(track), mHoverSection(nullptr), mSelectionSection(nullptr), mLastWayPoint(nullptr), mPropGrid(nullptr)
{
	mWayPointSelector = new WayPointSelector;
	mWayPointSelector->Add({track});
	mWayPointSelector->SignalSelection += sig::slot(this, &TrackReviewMode::OnSelectWayPoint);
	mWayPointSelector->SignalDeselection += sig::slot(this, &TrackReviewMode::OnDeselectWayPoint);

	MapObjectContainer* review = mTrack->GetReview();
	review->GetProperties()
		.Set<prop::Color>(Gdiplus::Color::Red)
		.Set<prop::LineWidth>(3);
	mSectionSelector = new SectionSelector; 
	mCurrentSectionSelector = new SectionSelector;
	mSectionSelector->SetPriority(1);
	mSectionSelector->Add({review});
	mCurrentSectionSelector->Add({review});

	mSectionSelector->SignalSelection += sig::slot(this, &TrackReviewMode::OnSelectSection);
	mSectionSelector->SignalDeselection += sig::slot(this, &TrackReviewMode::OnDeselectSection);
	mCurrentSectionSelector->SignalSelection += sig::slot(this, &TrackReviewMode::OnSelectCurrentSection);
	mCurrentSectionSelector->SignalDeselection += sig::slot(this, &TrackReviewMode::OnDeselectCurrentSection);

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

void TrackReviewMode::OnSelectWayPoint(WayPoint* wp)
{
	mMapRenderer->AddComponent(wp->GetLocation());
	mMapRenderer->Invalidate();
}

void TrackReviewMode::OnDeselectWayPoint(WayPoint* wp)
{
	mMapRenderer->RemoveComponent(wp->GetLocation());
	mMapRenderer->Invalidate();
}

void TrackReviewMode::OnSelectSection(Section* section, const Vector2d& nearest)
{
	mHoverSection = new Section(*section);
	mHoverSection->GetProperties()
		.Set<prop::Color>(0xffffffff)
		.Set<prop::LineWidth>(section->GetProperties().Get<prop::LineWidth>(2) + 3);
	mMapRenderer->AddComponent(mHoverSection);
	mMapRenderer->AddComponent(section);
	mMapRenderer->Invalidate();
}

void TrackReviewMode::OnDeselectSection(Section* section, const Vector2d& nearest)
{
	if(mHoverSection)
	{
		mMapRenderer->RemoveComponent(mHoverSection);
		mMapRenderer->RemoveComponent(section);
		delete mHoverSection;
		mHoverSection = nullptr;
		mMapRenderer->Invalidate();
	}
}

void TrackReviewMode::OnSelectCurrentSection(Section* section, const Vector2d& nearest)
{
	if(!mHoverSection) return;
	mSelectionSection = mHoverSection;
	mHoverSection = nullptr;

	mPropGrid = new ww::PropertyGrid;
	mMapRenderer->Add(mPropGrid);
	mPropGrid->SetBounds({10, 510, 250, 650});
	
	ww::ColorProperty* color = new ww::ColorProperty("Couleur", "Section", section->GetProperties().Get<prop::Color>(0));
	ww::DropDownProperty* ddp = new ww::DropDownProperty("Type", "Section");
	*ddp << " " << "Plat" << "Descente/Montée" << "Descente uniquement";
	ddp->SetSelected(section->GetProperties().Get<prop::Type>(0));
	ww::DropDownProperty* diffp = new ww::DropDownProperty("Difficulté", "Section");
	*diffp << " " << "Facile" << "Moyenne" << "Difficile";
	diffp->SetSelected(section->GetProperties().Get<prop::Difficulty>(0));
	ww::DropDownProperty* interestp = new ww::DropDownProperty("Intérêt", "Section");
	*interestp << " " << "x" << "x x" << "x x x";
	interestp->SetSelected(section->GetProperties().Get<prop::Interest>(0));

	mPropGrid->AddProperty(color);
	mPropGrid->AddProperty(ddp);
	mPropGrid->AddProperty(diffp);
	mPropGrid->AddProperty(interestp);
	mPropGrid->ExpandAllCatalogs();

	color->SignalPropertyChanged += [this, section](int color) { section->GetProperties().Set<prop::Color>(color); mMapRenderer->Invalidate(); };
	ddp->SignalPropertyChanged += [this, section](int index) { section->GetProperties().Set<prop::Type>(index); };
	diffp->SignalPropertyChanged += [this, section](int index) { section->GetProperties().Set<prop::Difficulty>(index); };
	interestp->SignalPropertyChanged += [this, section](int index) { section->GetProperties().Set<prop::Interest>(index); };
	
	mMapRenderer->Invalidate();
}

void TrackReviewMode::OnDeselectCurrentSection(Section* section, const Vector2d& nearest)
{
	if(mPropGrid)
	{
		mMapRenderer->Remove(mPropGrid);
		mMapRenderer->RemoveComponent(mSelectionSection);
		delete mSelectionSection;
		delete mPropGrid;
		mPropGrid = nullptr;
	}
	section->GetProperties().Pop();
	mMapRenderer->RemoveComponent(section);
	mMapRenderer->Invalidate();
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