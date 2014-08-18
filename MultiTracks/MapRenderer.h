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

	std::shared_ptr<Gdiplus::Bitmap> Draw() const;
	void Draw(Gdiplus::Graphics* g) const;
	void Save(const wchar_t* filename, ImageFormat imformat = ImageFormat::jpeg) const;

	template <class T>
	void AddComponent(T* component);

protected:
	void InternalDraw(Gdiplus::Graphics* g) const;

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
	virtual void OnPaint(Gdiplus::Graphics* g);
	virtual void OnResize(int width, int height);
	virtual void OnMouseDown(ww::MouseEvent ev);
	virtual void OnMouseDrag(ww::MouseEvent ev);
	virtual void OnMouseWheel(ww::MouseEvent ev);
};

template <class T>
void MapRenderer::AddComponent(T* component)
{
	mEntities.push_back(new Entity(component));
}

}

#endif // !__MULTITRACKS_MAPRENDERER_H__