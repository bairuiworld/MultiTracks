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
	
	const Properties& props = component->GetProperties();
	Gdiplus::Pen pen(Gdiplus::Color(props.Get<prop::Color>(Gdiplus::Color::Black)),
									props.Get<prop::LineWidth>(2));
	pen.SetDashStyle((Gdiplus::DashStyle)props.Get<prop::DashStyle>(Gdiplus::DashStyle::DashStyleSolid));
	pen.SetLineJoin(Gdiplus::LineJoin::LineJoinRound);
	bool dispEnd = props.Get<prop::DisplaySectionEnd>(false);

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

	if(dispEnd)
	{
		Vector2d point = viewport->LocationToPixel(locations.front());
		g->FillEllipse(pen.GetBrush(), point.GetX() - 3.f, point.GetY() - 3.f, 6.f, 6.f);
		point.Set(viewport->LocationToPixel(locations.back()));
		g->FillEllipse(pen.GetBrush(), point.GetX() - 3.f, point.GetY() - 3.f, 6.f, 6.f);
	}
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

void LocationRenderer::Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component)
{
	const Properties& prop = component->GetProperties();
	Gdiplus::SolidBrush brush(prop.Get<prop::Color>(Gdiplus::Color::Red));
	Vector2d p = viewport->LocationToPixel(*static_cast<const Location*>(component));
	switch(prop.Get<prop::Shape>(Shape::FilledCircle))
	{
	case Shape::FilledCircle:
		g->FillEllipse(&brush, (float)p.GetX() - 5, (float)p.GetY() - 5, 10.f, 10.f);
		break;

	case Shape::Circle:
		Gdiplus::Pen pen(&brush, prop.Get<prop::LineWidth>(2));
		g->DrawEllipse(&pen, (float)p.GetX() - 5, (float)p.GetY() - 5, 10.f, 10.f);
		break;
	}
	
}

}