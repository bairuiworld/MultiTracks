#include "Section.h"
#include "MapViewport.h"
#include "EntityRenderer.h"

namespace mt
{

/*void SectionRenderer::Draw(Cairo::RefPtr<Cairo::Context> cr, MapViewport* viewport, Component* component)
{
	Section* section = static_cast<Section*>(component);
	const Section::LocationList& locations = section->GetLocations();
	for(Location* location : locations)
	{
		Vector2d point = viewport->LocationToPixel(*location);
		if(!cr->has_current_point())
			cr->move_to(point.GetX(), point.GetY());
		else
			cr->move_to(point.GetX(), point.GetY());
	}
	cr->stroke();
}*/

}