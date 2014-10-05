#include <Windows.h>
#include <gdiplus.h>
#include "Track.h"
#include "Section.h"
#include "WayPoint.h"
#include "MapRenderer.h"
#include "Map.h"
#include "ConfigManager.h"
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

ContainerEditMode::ContainerEditMode(WindowMapRenderer* renderer, MapObjectContainer* container) :
EditMode(renderer), mContainer(container), mCurrentSection(nullptr), mCurrentLocation(nullptr)
{
	mSectionEndSelector = new SectionEndSelector;
	mSectionEndSelector->Add({container});
	mSectionEndSelector->SignalSelection += sig::slot(this, &ContainerEditMode::OnSelectSectionEnd);
	mSectionEndSelector->SignalDeselection += sig::slot(this, &ContainerEditMode::OnDeselectSectionEnd);
	mMapRenderer->AddSelector(mSectionEndSelector, SelectorAction::MouseMove);

	renderer->SignalMouseMove += sig::slot(this, &ContainerEditMode::OnMouseMove);

	mContainer->GetProperties().Push()
		.Set<prop::Color>(Config::TrackEditLineColor())
		.Set<prop::LineWidth>(Config::TrackEditLineWidth());
	mMapRenderer->AddComponent(mContainer);	
	mMapRenderer->Invalidate();
}

ContainerEditMode::~ContainerEditMode()
{
	mContainer->GetProperties().Pop();
	if(mCurrentLocation)
	{
		mMapRenderer->RemoveComponent(mCurrentLocation);
		mCurrentLocation->GetProperties().Pop();
	}
	mMapRenderer->RemoveComponent(mContainer);
	mMapRenderer->Invalidate();
	mMapRenderer->RemoveSelector(mSectionEndSelector, SelectorAction::MouseMove);
}

void ContainerEditMode::OnSelectSectionEnd(Section* section, Location* sectionEnd)
{
	std::cout << "sel" << std::endl;
	mMapRenderer->AddComponent(sectionEnd);
	sectionEnd->GetProperties()
		.Push()
		.Set<prop::Shape>(Shape::Circle)
		.Set<prop::LineWidth>(2);
	mMapRenderer->Invalidate();
}

void ContainerEditMode::OnDeselectSectionEnd(Section* section, Location* sectionEnd)
{
	mMapRenderer->RemoveComponent(sectionEnd);
	sectionEnd->GetProperties().Pop();
	mMapRenderer->Invalidate();
}

void ContainerEditMode::OnMouseMove(ww::MouseEvent ev)
{

}

void ContainerEditMode::OnMapClick(ww::MouseEvent ev, const Location& location)
{
	if(mCurrentLocation)
	{
		mMapRenderer->RemoveComponent(mCurrentLocation);
		mCurrentLocation->GetProperties().Pop();
	}
	if(!mCurrentSection)
	{
		mCurrentSection = mSectionEndSelector->GetCurrentSection();
		if(!mCurrentSection)
		{
			mCurrentSection = new Section(mContainer);
			mContainer->Add(mCurrentSection);
			mSectionEndSelector->Invalidate();
		}
	}
	mCurrentLocation = mSectionEndSelector->GetCurrentSectionEnd();
	if(!mCurrentLocation)
		mCurrentSection->Add(location);
	mCurrentLocation = mCurrentSection->GetLastLocation();
	mCurrentLocation->GetProperties()
		.Push()
		.Set<prop::Shape>(Shape::Circle)
		.Set<prop::LineWidth>(2);
	mMapRenderer->AddComponent(mCurrentLocation);
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
	delete mLastWayPoint;
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
	*ddp << " " << "Plat" << "Descente/Montée" << "Descente uniquement" << "Montée privilégiée";
	ddp->SetSelected(section->GetProperties().Get<prop::Type>(0));
	ww::DropDownProperty* diffp = new ww::DropDownProperty("Difficulté", "Section");
	*diffp << " " << "Aucune" << "Technique" << "Physique" << "Technique/Physique";
	diffp->SetSelected(section->GetProperties().Get<prop::Difficulty>(0));
	ww::DropDownProperty* interestp = new ww::DropDownProperty("Intérêt", "Section");
	*interestp << " " << "Aucun" << "Utilisable" << "Agréable" << "A utiliser si possible" << "Incontournable" << "Impraticable" << "A éviter";
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