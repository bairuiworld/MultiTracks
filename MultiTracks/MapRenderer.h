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
enum class AddingPolicy { Ensure, IncRef };
enum class RemovingPolicy { DecRef, Always };

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
	void AddComponent(T* component, AddingPolicy policy = AddingPolicy::IncRef);
	void RemoveComponent(const Component* component, RemovingPolicy policy = RemovingPolicy::DecRef);

protected:
	void InternalDraw(Gdiplus::Graphics* g, bool synchronous) const;

protected:
	Map* mMap;
	std::vector<Entity*> mEntities;
};

enum class SelectorAction { MouseMove, MouseClick };

class WindowMapRenderer : public MapRenderer, public ww::DrawingArea
{
public:
	WindowMapRenderer(Map* map);
	virtual ~WindowMapRenderer();

	void AddSelector(Selector* selector, SelectorAction action);
	void RemoveSelector(Selector* selector, SelectorAction action);

	void InvalidateSelectors();

protected:
	virtual void OnPaint(Gdiplus::Graphics* g, const RECT& clip);
	virtual void OnResize(int width, int height);
	virtual void OnMouseDown(ww::MouseEvent ev);
	virtual void OnMouseDrag(ww::MouseEvent ev);
	virtual void OnMouseWheel(ww::MouseEvent ev);
	virtual void OnMouseMove(ww::MouseEvent ev);
	virtual void OnMouseClick(ww::MouseEvent ev);

	void Select(const std::vector<Selector*>& v, const Vector2d& p);

protected:
	int mNewTileId;
	std::map<SelectorAction, std::vector<Selector*>> mSelectors;

public:
	sig::Signal<void(ww::MouseEvent, const Location&)> SignalMapClick;
};

template <class T>
void MapRenderer::AddComponent(T* component, AddingPolicy policy)
{
	auto it = std::find_if(mEntities.begin(), mEntities.end(), [component](Entity* e) {
		return e->GetComponent() == component;
	});
	if(it != mEntities.end() && policy == AddingPolicy::IncRef)
		(*it)->IncRef();
	else
		mEntities.push_back(new Entity(component));
}

}

#endif // !__MULTITRACKS_MAPRENDERER_H__