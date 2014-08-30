#include "Layout.h"
#include "MapRenderer.h"
#include "Map.h"
#include "MapSource.h"
#include "MapViewport.h"
#include "ViewManager.h"

#include <iostream>
#include "ProjectManager.h"

namespace mt
{

ViewManager::ViewManager()
{
	mWindow = new ww::Window("MultiTracks");
	mWindow->SetLayout(new ww::FillLayout);

	mMap = new Map(&MapSource::IGN);
	mMap->GetViewport()->SetZoom(4);
	mMapRenderer = new WindowMapRenderer(mMap);
	mWindow->Add(mMapRenderer);
	
	mProjectTree = new ww::TreeView;
	mMapRenderer->Add(mProjectTree);
	mProjectTree->SetBounds({0, 0, 200, 200});

	ProjectManager* pm = new ProjectManager(mMapRenderer, mProjectTree);
	pm->LoadProject("projet.txt");

	mProjectTree->SignalItemClick += [](ww::TreeNode* node, ww::MouseEvent ev) {
		std::cout << "ok" << std::endl;
	};
}

ViewManager::~ViewManager()
{
	delete mWindow;
	delete mMap;
}

}