#include "Layout.h"
#include "MapRenderer.h"
#include "Map.h"
#include "MapSource.h"
#include "MapViewport.h"
#include "Track.h"
#include "ViewManager.h"

#include <iostream>
#include "ProjectManager.h"

namespace mt
{

ViewManager::ViewManager() :
mDisplayedTrack(nullptr)
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
}

ViewManager::~ViewManager()
{
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

}