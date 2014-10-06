#ifndef __MULTITRACKS_VIEWMANAGER_H__
#define __MULTITRACKS_VIEWMANAGER_H__

#include "Application.h"
#include "TreeView.h"
#include "CheckBox.h"
#include "Window.h"
#include "PropertyGrid.h"
#include "Location.h"

namespace mt
{

class Map;
class WindowMapRenderer;
class MapObjectContainer;
class Track;
class ProjectManager;
class EditMode;

class ViewManager
{
public:
	ViewManager(ww::Application* app);
	~ViewManager();

	void AddMapObjectContainerProperties(MapObjectContainer* container, const std::string& catalog);

private:
	void OnTrackSelect(Track* track);
	bool OnEditTrack(Track* track);

	void OpenEditMode(EditMode* editMode);
	void CloseEditMode();

private:
	ww::Application* mApp;
	ww::Window* mWindow;
	Map* mMap;
	WindowMapRenderer* mMapRenderer;
	ProjectManager* mProjectManager;

	ww::TreeView* mProjectTree;
	ww::PropertyGrid* mPropertyGrid;
	ww::CheckBox* mEditButton;
	ww::CheckBox* mReviewButton;

	Track* mDisplayedTrack;
	EditMode* mEditMode;
};

}

#endif // !__MULTITRACKS_VIEWMANAGER_H__