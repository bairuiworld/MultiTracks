#include "Section.h"
#include "MapViewport.h"
#include "EntityRenderer.h"

namespace mt
{

void SectionRenderer::Draw(Gdiplus::Graphics* g, MapViewport* viewport, Component* component)
{
	Section* section = static_cast<Section*>(component);
	Vector2d last;
	bool haslast = false;
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0), 4);
	pen.SetDashStyle(Gdiplus::DashStyle::DashStyleDashDot);
	pen.SetDashCap(Gdiplus::DashCap::DashCapTriangle);

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