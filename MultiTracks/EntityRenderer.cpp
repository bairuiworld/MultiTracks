#include "Section.h"
#include "MapViewport.h"
#include "Component.h"
#include "Properties.h"
#include "EntityRenderer.h"

namespace mt
{

void SectionRenderer::Draw(Gdiplus::Graphics* g, MapViewport* viewport, Component* component)
{
	Section* section = static_cast<Section*>(component);
	Vector2d last;
	bool haslast = false;
	
	Properties prop = component->GetProperties();
	Gdiplus::Pen pen(Gdiplus::Color(prop.Get<int>("color", Gdiplus::Color::Black)),
									prop.Get<int>("linewidth", 1));
	pen.SetDashStyle((Gdiplus::DashStyle)prop.Get<int>("dashstyle", Gdiplus::DashStyle::DashStyleSolid));

	const Section::LocationList& locations = section->GetLocations();
	for(Location* location : locations)
	{
		Vector2d point = viewport->LocationToPixel(*location);
		if(haslast)
			g->DrawLine(&pen, (float)last.GetX(), (float)last.GetY(), (float)point.GetX(), (float)point.GetY());
		last = point;
		haslast = true;
	}
}

}