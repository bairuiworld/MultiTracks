#include "Map.h"
#include "MapViewport.h"
#include "Entity.h"
#include "EntityRenderer.h"
#include "Widget.h"
#include "MapRenderer.h"

namespace mt
{

void f() {}

MapRenderer::MapRenderer(Map* map) :
	mMap(map)
{
	mMap->SignalNewTile += [this]()
	{ 
		if(mParent)
			mParent->Invalidate();
	};
}

void MapRenderer::OnPaint(Gdiplus::Graphics* g)
{
	mMap->Draw(g);

	/*for(Entity* entity : mEntites)
		entity->Draw(cr, mMap->GetViewport());*/
}

void MapRenderer::OnResize(int width, int height)
{
	mMap->GetViewport()->SetViewDimension(width, height);
}

}