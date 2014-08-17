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

enum class ImageFormat { png, jpeg, bmp, gif, tiff };

class MapRenderer
{
public:
	MapRenderer(Map* map);
	virtual ~MapRenderer() = default;

	std::shared_ptr<Gdiplus::Bitmap> Draw() const;
	void Draw(Gdiplus::Graphics* g) const;
	void Save(const wchar_t* filename, ImageFormat imformat = ImageFormat::jpeg) const;

protected:
	void InternalDraw(Gdiplus::Graphics* g) const;

protected:
	Map* mMap;
	std::vector<Entity*> mEntites;
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

}

#endif // !__MULTITRACKS_MAPRENDERER_H__