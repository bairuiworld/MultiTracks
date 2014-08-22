#include "Location.h"
#include "Section.h"
#include "Track.h"
#include "MapViewport.h"
#include "Component.h"
#include "Properties.h"
#include "EntityRenderer.h"

namespace mt
{

void SectionRenderer::Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component)
{
	const Section* section = static_cast<const Section*>(component);
	Vector2d last;
	bool haslast = false;
	
	const Properties& prop = component->GetProperties();
	Gdiplus::Pen pen(Gdiplus::Color(prop.Get<int>("color", Gdiplus::Color::Black)),
									prop.Get<float>("linewidth", 1));
	pen.SetDashStyle((Gdiplus::DashStyle)prop.Get<int>("dashstyle", Gdiplus::DashStyle::DashStyleSolid));
	pen.SetLineJoin(Gdiplus::LineJoin::LineJoinRound);

	Gdiplus::GraphicsPath path;

	const Section::LocationList& locations = section->GetLocations();
	for(const Location& location : locations)
	{
		Vector2d point = viewport->LocationToPixel(location);
		if(haslast)
			path.AddLine((float)last.GetX(), (float)last.GetY(), (float)point.GetX(), (float)point.GetY());
		last = point;
		haslast = true;
	}
	g->DrawPath(&pen, &path);
}

void TrackRenderer::Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component)
{
	const Track* track = static_cast<const Track*>(component);
	std::shared_ptr<MapObjectContainerRenderer> containerRenderer = DefaultRenderer<MapObjectContainer>::value;
	containerRenderer->Draw(g, viewport, track);
}

void MapObjectContainerRenderer::Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component)
{
	const MapObjectContainer* container = static_cast<const MapObjectContainer*>(component);
	std::shared_ptr<SectionRenderer> sectionRenderer = DefaultRenderer<Section>::value;

	Track::SectionList sections = container->GetSections();
	for(Section* section : sections)
		sectionRenderer->Draw(g, viewport, section);
}

}