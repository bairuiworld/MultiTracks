#include "Map.h"
#include "MapViewport.h"
#include "Entity.h"
#include "EntityRenderer.h"
#include "EntitySelector.h"
#include "Section.h"
#include "Location.h"
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
	mMap->PreloadTiles();
	InternalDraw(g);
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
	InternalDraw(&gMem);

	BitBlt(hDC, 0, 0, view->GetWidth(), view->GetHeight(), hMemDC, 0, 0, SRCCOPY);
	g->ReleaseHDC(hDC);

	DeleteObject(hMemBitmap);
	DeleteDC(hMemDC);
}

void MapRenderer::InternalDraw(Gdiplus::Graphics* g) const
{
	g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
	Gdiplus::GraphicsState state = g->Save();
	mMap->Draw(g);
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

void MapRenderer::Save(const wchar_t* filename, ImageFormat imformat) const
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

	CLSID clsid;
	GetEncoderClsid(format, &clsid);
	Draw()->Save(filename, &clsid, NULL);
}

void MapRenderer::RemoveComponent(Component* component)
{
	auto it = std::find_if(mEntities.begin(), mEntities.end(), [component](Entity* e) {
		return e->GetComponent() == component;
	});
	if(it != mEntities.end())
		mEntities.erase(it);
}

WindowMapRenderer::WindowMapRenderer(Map* map) :
MapRenderer(map), mSelector(map->GetViewport(), 5), mHoverComponent(nullptr)
{
	mMap->SignalNewTile += [this]()
	{ 
		if(mParent)
			mParent->Invalidate();
	};
}

void WindowMapRenderer::InvalidateEntities()
{
	for(Entity* entity : mEntities)
		if(entity->GetSelector())
			entity->GetSelector()->Invalidate();
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

}

void WindowMapRenderer::OnMouseDrag(ww::MouseEvent ev)
{
	mMap->GetViewport()->MoveOrigin(static_cast<float>(ev.GetPrevPoint().x - ev.GetPoint().x), static_cast<float>(ev.GetPrevPoint().y - ev.GetPoint().y));
	if(mParent)
		mParent->Invalidate();
	InvalidateEntities();
}

void WindowMapRenderer::OnMouseWheel(ww::MouseEvent ev)
{
	MapViewport* view = mMap->GetViewport();
	POINT pt = ev.GetPoint();
	view->SetZoom(view->GetZoom() + ev.GetWheelRotation(), Vector2d(pt.x, pt.y));
	if(mParent)
		mParent->Invalidate();
	InvalidateEntities();
}

void WindowMapRenderer::OnResize(int width, int height)
{
	mMap->GetViewport()->SetViewDimension(width, height);
}

void WindowMapRenderer::OnMouseMove(ww::MouseEvent ev)
{
	mSelector.SetPoint(Vector2d(ev.GetPoint().x, ev.GetPoint().y));
	/**/mSelector.SetSelectable(Selectable::Section | Selectable::SectionEnd);
	mSelector.Select(mEntities.begin(), mEntities.end());

	if(mSelector.GetSelected() == Selectable::None && mHoverComponent)
	{
		mHoverComponent->GetProperties().Pop();
		mHoverComponent = nullptr;
		if(mParent)
			mParent->Invalidate();
	}
	else if(mSelector.GetSelected() == Selectable::Section && mSelector.GetComponent() != mHoverComponent)
	{
		if(mHoverComponent)
			mHoverComponent->GetProperties().Pop();
		mHoverComponent = mSelector.GetComponent();
		mHoverComponent->GetProperties().Push().Set("color", (int)Gdiplus::Color::Red);
		if(mParent)
			mParent->Invalidate();
	}
	else if(mSelector.GetSelected() == Selectable::SectionEnd)
	{
		Component* c = mSelector.GetComponent();
		c->GetProperties().Push();
		AddComponent(reinterpret_cast<Location*>(c));
		if(mParent)
			mParent->Invalidate();
	}
}

}