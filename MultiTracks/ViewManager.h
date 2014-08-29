#ifndef __MULTITRACKS_VIEWMANAGER_H__
#define __MULTITRACKS_VIEWMANAGER_H__

#include "TreeView.h"
#include "Window.h"

namespace mt
{

class Map;
class WindowMapRenderer;

class ViewManager
{
public:
	ViewManager();
	~ViewManager();

private:
	ww::Window* mWindow;
	Map* mMap;
	WindowMapRenderer* mMapRenderer;
	ww::TreeView* mProjectTree;
};

}

#endif // !__MULTITRACKS_VIEWMANAGER_H__