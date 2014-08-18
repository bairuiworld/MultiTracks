#include "MapViewport.h"
#include "EntityRenderer.h"
#include "Entity.h"

namespace mt
{

void Entity::Draw(Gdiplus::Graphics* g, MapViewport* viewport)
{
	if(mRenderer)
	{
		Gdiplus::GraphicsState state = g->Save();
		mRenderer->Draw(g, viewport, mComponent);
		g->Restore(state);
	}
}

}