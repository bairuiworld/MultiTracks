#include "Map.h"
#include "MapViewport.h"
#include "Entity.h"
#include "EntityRenderer.h"
#include "MapRenderer.h"

namespace mt
{

void f() {}

MapRenderer::MapRenderer(Map* map) :
	mMap(map)
{
	//mMap->signal_new_tile.connect([this]() { get_window()->invalidate(true); });
	//mMap->signal_new_tile.connect(sigc::ptr_fun(&f));
}

/*bool MapRenderer::on_expose_event(GdkEventExpose* event)
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(!window) return true;
	
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	Cairo::Context cr_(Cairo::Context(gdk_cairo_create(window->gobj())));
	Cairo::RefPtr<Cairo::Context> cr(&cr_);
	(*cr.refcount_())++;

	cr->save();
	mMap->Draw(cr);
	cr->restore();

	for(Entity* entity : mEntites)
		entity->Draw(cr, mMap->GetViewport());

	return true;
}

bool MapRenderer::on_configure_event(GdkEventConfigure* evt)
{
	mMap->GetViewport()->SetViewDimension(evt->width, evt->height);
	return true;
}
*/
}