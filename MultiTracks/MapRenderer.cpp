#include "Map.h"
#include "MapViewport.h"
#include "Entity.h"
#include "EntityRenderer.h"
#include "EntitySelector.h"
#include "Section.h"
#include "Location.h"
#include "WayPoint.h"
#include "Widget.h"
#include "MapRenderer.h"

namespace mt
{

MapRenderer::MapRenderer(Map* map) :
mMap(map)
{

}

MapRenderer::~MapRenderer()
{
	for(Entity* entity : mEntities)
		delete entity;
}

std::shared_ptr<Gdiplus::Bitmap> MapRenderer::Draw() const
{
	MapViewport* view = mMap->GetViewport();
	std::shared_ptr<Gdiplus::Bitmap> bitmap = std::make_shared<Gdiplus::Bitmap>(view->GetWidth(), view->GetHeight());
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(bitmap.get());
	InternalDraw(g, true);
	delete g;
	return bitmap;
}

void MapRenderer::Draw(Gdiplus::Graphics* g) const
{
	MapViewport* view = mMap->GetViewport();
	HDC hDC = g->GetHDC();
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hMemBitmap = CreateCompatibleBitmap(hDC, view->GetWidth(), view->GetHeight());
	SelectObject(hMemDC, hMemBitmap);

	Gdiplus::Graphics gMem(hMemDC);
	InternalDraw(&gMem, false);

	BitBlt(hDC, 0, 0, view->GetWidth(), view->GetHeight(), hMemDC, 0, 0, SRCCOPY);
	g->ReleaseHDC(hDC);

	DeleteObject(hMemBitmap);
	DeleteDC(hMemDC);
}

void MapRenderer::InternalDraw(Gdiplus::Graphics* g, bool synchonous) const
{
	g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
	Gdiplus::GraphicsState state = g->Save();
	if(synchonous) mMap->SyncDraw(g);
	else mMap->Draw(g);
	g->Restore(state);

	for(Entity* entity : mEntities)
		entity->Draw(g, mMap->GetViewport());
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void MapRenderer::Save(std::string filename, ImageFormat imformat) const
{
	const wchar_t* format = nullptr;
	switch(imformat)
	{
	case ImageFormat::jpeg: format = L"image/jpeg"; break;
	case ImageFormat::png:  format = L"image/png"; break;
	case ImageFormat::bmp:  format = L"image/bmp"; break;
	case ImageFormat::gif:  format = L"image/gif"; break;
	case ImageFormat::tiff:  format = L"image/tiff"; break;
	}
	std::wstring wfilename(filename.begin(), filename.end());
	CLSID clsid;
	GetEncoderClsid(format, &clsid);
	Draw()->Save(wfilename.c_str(), &clsid, NULL);
}

void MapRenderer::RemoveComponent(const Component* component, RemovingPolicy policy)
{
	auto it = std::find_if(mEntities.begin(), mEntities.end(), [component](Entity* e) {
		return e->GetComponent() == component;
	});
	if(it != mEntities.end())
	{
		if(policy == RemovingPolicy::DecRef)
			(*it)->DecRef();
		if((*it)->GetRef() == 0 || policy == RemovingPolicy::Always)
		{
			delete *it;
			mEntities.erase(it);
		}
	}
}

WindowMapRenderer::WindowMapRenderer(Map* map) :
MapRenderer(map)
{
	mNewTileId = mMap->SignalNewTile += [this]()
	{ 
		if(mParent)
			mParent->Invalidate();
	};
}

WindowMapRenderer::~WindowMapRenderer()
{
	mMap->SignalNewTile -= mNewTileId;
}

void WindowMapRenderer::AddSelector(Selector* selector, SelectorAction action)
{
	mSelectors[action].push_back(selector);
}

void WindowMapRenderer::RemoveSelector(Selector* selector, SelectorAction action)
{
	auto it = mSelectors.find(action);
	if(it == mSelectors.end()) return;
	std::vector<Selector*>::const_iterator sit = std::find((*it).second.begin(), (*it).second.end(), selector);
	if(sit != (*it).second.end())
		(*it).second.erase(sit);
}

void WindowMapRenderer::InvalidateSelectors()
{
	for(auto it : mSelectors)
		for(Selector* s : it.second)
			s->Invalidate();
}

void WindowMapRenderer::OnPaint(Gdiplus::Graphics* g, const RECT& clip)
{
	Draw(g);
}

void WindowMapRenderer::OnMouseDown(ww::MouseEvent ev)
{
	SetFocus(mhWnd);
}

void WindowMapRenderer::OnMouseClick(ww::MouseEvent ev)
{
	Vector2d pixel{(double)ev.GetPoint().x, (double)ev.GetPoint().y};
	switch(ev.GetButton())
	{
	case ww::MouseButton::Left:
		Select(mSelectors[SelectorAction::MouseLeftClick], pixel);
		break;

	case ww::MouseButton::Right:
		Select(mSelectors[SelectorAction::MouseRightClick], pixel);
		break;
	}
	SignalMapClick.emit(ev, mMap->GetViewport()->PixelToLocation(pixel));
}

void WindowMapRenderer::OnMouseDrag(ww::MouseEvent ev)
{
	mMap->GetViewport()->MoveOrigin(static_cast<float>(ev.GetPrevPoint().x - ev.GetPoint().x), static_cast<float>(ev.GetPrevPoint().y - ev.GetPoint().y));
	if(mParent)
		mParent->Invalidate();
	InvalidateSelectors();
}

void WindowMapRenderer::OnMouseWheel(ww::MouseEvent ev)
{
	MapViewport* view = mMap->GetViewport();
	POINT pt = ev.GetPoint();
	view->SetZoom(view->GetZoom() + ev.GetWheelRotation(), Vector2d(pt.x, pt.y));
	if(mParent)
		mParent->Invalidate();
	InvalidateSelectors();
}

void WindowMapRenderer::OnResize(int width, int height)
{
	mMap->GetViewport()->SetViewDimension(width, height);
}

void WindowMapRenderer::OnMouseMove(ww::MouseEvent ev)
{
	Select(mSelectors[SelectorAction::MouseMove], Vector2d(ev.GetPoint().x, ev.GetPoint().y));
}

void WindowMapRenderer::Select(const std::vector<Selector*>& v, const Vector2d& p)
{
	SelectionTracker tracker(mMap->GetViewport(), p, 5);
	for(Selector* s : v)
		tracker.Select(s);
	tracker.EmitResult();
}

}