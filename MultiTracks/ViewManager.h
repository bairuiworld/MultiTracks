#ifndef __MULTITRACKS_VIEWMANAGER_H__
#define __MULTITRACKS_VIEWMANAGER_H__

#include "TreeView.h"
#include "CheckBox.h"
#include "Window.h"
#include "Location.h"

namespace mt
{

class Map;
class WindowMapRenderer;
class Track;
class ProjectManager;
class EditMode;

class ViewManager
{
public:
	ViewManager();
	~ViewManager();

private:
	void OnTrackSelect(Track* track);
	bool OnEditTrack(Track* track);

	void OpenEditMode(EditMode* editMode);
	void CloseEditMode();

private:
	ww::Window* mWindow;
	Map* mMap;
	WindowMapRenderer* mMapRenderer;
	ww::TreeView* mProjectTree;
	ProjectManager* mProjectManager;

	ww::CheckBox* mEditButton;
	ww::CheckBox* mReviewButton;

	Track* mDisplayedTrack;
	EditMode* mEditMode;
};

}

#endif // !__MULTITRACKS_VIEWMANAGER_H__