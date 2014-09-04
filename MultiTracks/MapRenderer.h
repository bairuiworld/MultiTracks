#ifndef __MULTITRACKS_MAPRENDERER_H__
#define __MULTITRACKS_MAPRENDERER_H__

#include <vector>
#include <windows.h>
#include <gdiplus.h>
#include "Event.h"
#include "DrawingArea.h"
#include "Entity.h"

namespace mt
{

class Map;

enum class ImageFormat { png, jpeg, bmp, gif, tiff };

class MapRenderer
{
public:
	MapRenderer(Map* map);
	virtual ~MapRenderer();

	Map* GetMap() { return mMap; }

	std::shared_ptr<Gdiplus::Bitmap> Draw() const;
	void Draw(Gdiplus::Graphics* g) const;
	void Save(std::string filename, ImageFormat imformat = ImageFormat::jpeg) const;

	template <class T>
	void AddComponent(T* component);
	void RemoveComponent(Component* component);

protected:
	void InternalDraw(Gdiplus::Graphics* g, bool synchronous) const;

protected:
	Map* mMap;
	std::vector<Entity*> mEntities;
};

class WindowMapRenderer : public MapRenderer, public ww::DrawingArea
{
public:
	WindowMapRenderer(Map* map);
	virtual ~WindowMapRenderer() = default;

protected:
	void InvalidateEntities();

	virtual void OnPaint(Gdiplus::Graphics* g, const RECT& clip);
	virtual void OnResize(int width, int height);
	virtual void OnMouseDown(ww::MouseEvent ev);
	virtual void OnMouseDrag(ww::MouseEvent ev);
	virtual void OnMouseWheel(ww::MouseEvent ev);
	virtual void OnMouseMove(ww::MouseEvent ev);
	virtual void OnMouseClick(ww::MouseEvent ev);

protected:
	ComponentSelector mSelector;
	Component* mHoverComponent;
};

template <class T>
void MapRenderer::AddComponent(T* component)
{
	mEntities.push_back(new Entity(component));
}

}

#endif // !__MULTITRACKS_MAPRENDERER_H__