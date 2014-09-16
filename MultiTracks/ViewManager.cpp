#include "Layout.h"
#include "PropertyGrid.h"
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

	mPropertyGrid = new ww::PropertyGrid;
	mMapRenderer->Add(mPropertyGrid);
	mPropertyGrid->SetBounds({10, 360, 250, 500});

	mEditButton = new ww::CheckBox("Edit", ww::CheckBoxStyle::PushLike);
	mReviewButton = new ww::CheckBox("Review", ww::CheckBoxStyle::PushLike);
	mMapRenderer->Add(mEditButton);
	mMapRenderer->Add(mReviewButton);
	mEditButton->Enable(false);
	mReviewButton->Enable(false);
	mEditButton->SetBounds({260, 10, 330, 70});
	mReviewButton->SetBounds({340, 10, 410, 70});
	mEditButton->SignalClicked += [this]() { 
		if(mEditButton->IsChecked())
		{
			if(mReviewButton->IsChecked())
			{
				Track* track = mProjectManager->GetCurrentTrack();
				if(track)
					OpenEditMode(new TrackReviewMode(mMapRenderer, track));
			}
			else
				mProjectManager->EditCurrentTrack();
		}
		else CloseEditMode();
	};
	mReviewButton->SignalClicked += [this]() {
		Track* track = mProjectManager->GetCurrentTrack();
		if(mReviewButton->IsChecked())
			mMapRenderer->AddComponent(track->GetReview());
		else
			mMapRenderer->RemoveComponent(track->GetReview());
		mMapRenderer->Invalidate();
	};

	mProjectManager = new ProjectManager(mMapRenderer, mProjectTree);
	mProjectManager->LoadProject("C:\\Users\\Eisenheim\\Projets\\cpp\\MultiTracks\\MultiTracks\\projet.txt");
	mProjectManager->SignalSelectTrack += sig::slot(this, &ViewManager::OnTrackSelect);
	mProjectManager->SignalEditTrack += sig::slot(this, &ViewManager::OnEditTrack);
}

ViewManager::~ViewManager()
{
	delete mEditMode;
	delete mWindow;
	delete mMap;
	delete mProjectManager;
}

void ViewManager::AddMapObjectContainerProperties(MapObjectContainer* container, const std::string& catalog)
{
	Properties& prop = container->GetProperties();
	ww::ColorProperty* colorprop = new ww::ColorProperty("Couleur", catalog, prop.Get<int>("color", 0));
	colorprop->SignalPropertyChanged += [this, &prop](int color) { prop.Set("color", color); mMapRenderer->Invalidate(); };
	mPropertyGrid->AddProperty(colorprop);
}

void ViewManager::OnTrackSelect(Track* track)
{
	if(!mEditButton->IsChecked() && !mReviewButton->IsChecked())
	{
		mEditButton->Enable(true);
		mReviewButton->Enable(true);
	}
	
	if(mDisplayedTrack)
	{
		mMapRenderer->RemoveComponent(mDisplayedTrack);
		mPropertyGrid->Reset();
	}
	mDisplayedTrack = track;
	AddMapObjectContainerProperties(track, track->GetName());
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

}