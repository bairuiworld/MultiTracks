#ifndef __MULTITRACKS_MAPRENDERER_H__
#define __MULTITRACKS_MAPRENDERER_H__

#include <vector>
#include <windows.h>
#include <gdiplus.h>
#include "Event.h"
#include "DrawingArea.h"

namespace mt
{

class Entity;
class Map;

class MapRenderer : public ww::DrawingArea
{
public:
	MapRenderer(Map* map);

protected:
	virtual void OnPaint(Gdiplus::Graphics* g);
	virtual void OnResize(int width, int height);
	virtual void OnMouseDrag(ww::MouseEvent ev);

protected:
	Map* mMap;
	std::vector<Entity*> mEntites;
};

}

#endif // !__MULTITRACKS_MAPRENDERER_H__