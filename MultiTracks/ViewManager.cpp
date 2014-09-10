#include "Layout.h"
#include "MapRenderer.h"
#include "Map.h"
#include "MapSource.h"
#include "MapViewport.h"
#include "Track.h"
#include "EditMode.h"
#include "ViewManager.h"

#include <iostream>
#include "ProjectManager.h"

namespace mt
{

ViewManager::ViewManager() :
mDisplayedTrack(nullptr), mEditMode(nullptr)
{
	mWindow = new ww::Window("MultiTracks");
	mWindow->SetLayout(new ww::FillLayout);

	mMap = new Map(&MapSource::IGN);
	mMap->GetViewport()->SetZoom(4);
	mMapRenderer = new WindowMapRenderer(mMap);
	mWindow->Add(mMapRenderer);
	
	mProjectTree = new ww::TreeView;
	mMapRenderer->Add(mProjectTree);
	mProjectTree->SetBounds({10, 10, 250, 350});

	mEditButton = new ww::CheckBox("Edit", ww::CheckBoxStyle::PushLike);
	mReviewButton = new ww::CheckBox("Review", ww::CheckBoxStyle::PushLike);
	mMapRenderer->Add(mEditButton);
	mMapRenderer->Add(mReviewButton);
	mEditButton->Enable(false);
	mReviewButton->Enable(false);
	mEditButton->SetBounds({260, 10, 330, 70});
	mReviewButton->SetBounds({340, 10, 410, 70});
	mEditButton->SignalClicked += [this]() { 
		if(mEditButton->IsChecked()) mProjectManager->EditCurrentTrack();
		else CloseEditMode();
	};
	mReviewButton->SignalClicked += [this]() {
		if(mReviewButton->IsChecked())
		{
			Track* track = mProjectManager->GetCurrentTrack();
			if(track)
				OpenEditMode(new TrackReviewMode(mMapRenderer, track));
		}
		else CloseEditMode();
	};

	mProjectManager = new ProjectManager(mMapRenderer, mProjectTree);
	mProjectManager->LoadProject("projet.txt");
	mProjectManager->SignalSelectTrack += sig::slot(this, &ViewManager::OnTrackSelect);
	mProjectManager->SignalEditTrack += sig::slot(this, &ViewManager::OnEditTrack);

	mMapRenderer->SignalMapClick += sig::slot(this, &ViewManager::OnMapClick);
}

ViewManager::~ViewManager()
{
	delete mEditMode;
	delete mWindow;
	delete mMap;
	delete mProjectManager;
}

void ViewManager::OnTrackSelect(Track* track)
{
	if(!mEditButton->IsChecked() && !mReviewButton->IsChecked())
	{
		mEditButton->Enable(true);
		mReviewButton->Enable(true);
	}
	if(mDisplayedTrack)
		mMapRenderer->RemoveComponent(mDisplayedTrack);
	mDisplayedTrack = track;
	mMapRenderer->AddComponent(mDisplayedTrack);
	mMapRenderer->Invalidate();
}

bool ViewManager::OnEditTrack(Track* track)
{
	if(mDisplayedTrack == track)
	{
		mMapRenderer->RemoveComponent(mDisplayedTrack);
		mDisplayedTrack = nullptr;
	}
	OpenEditMode(new TrackEditMode(mMapRenderer, track));
	return true;
}

void ViewManager::OpenEditMode(EditMode* editMode)
{
	mEditButton->SetCheck(true);
	mEditButton->SetText("Quitter edit");
	mReviewButton->SetCheck(false);
	mReviewButton->Enable(false);
	if(mEditMode)
		CloseEditMode();
	mEditMode = editMode;
}

void ViewManager::CloseEditMode()
{
	delete mEditMode;
	mEditMode = nullptr;
	mEditButton->SetCheck(false);
	mEditButton->SetText("Edit");
	mReviewButton->Enable(true);
	mProjectManager->SetActiveNode(nullptr);
}

void ViewManager::OnMapClick(ww::MouseEvent ev, const Location& location)
{
	if(ev.GetButton() != ww::MouseButton::Left || ev.GetClicks() != 1) return;
	if(mEditMode)
		mEditMode->AppendLocation(location);

}

}