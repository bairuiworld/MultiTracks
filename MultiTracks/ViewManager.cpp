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
	if(mDisplayedTrack)
		mMapRenderer->RemoveComponent(mDisplayedTrack);
	mDisplayedTrack = track;
	mMapRenderer->AddComponent(mDisplayedTrack);
	mMapRenderer->Invalidate();
}

bool ViewManager::OnEditTrack(Track* track)
{
	delete mEditMode;
	if(mDisplayedTrack == track)
	{
		mMapRenderer->RemoveComponent(mDisplayedTrack);
		mDisplayedTrack = nullptr;
	}
	mEditMode = new TrackEditMode(mMapRenderer, track);
	return true;
}

void ViewManager::OnMapClick(ww::MouseEvent ev, const Location& location)
{
	if(ev.GetButton() != ww::MouseButton::Left || ev.GetClicks() != 1) return;
	if(mEditMode)
		mEditMode->AppendLocation(location);

}

}