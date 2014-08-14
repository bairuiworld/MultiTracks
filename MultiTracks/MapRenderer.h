#ifndef __MULTITRACKS_MAPRENDERER_H__
#define __MULTITRACKS_MAPRENDERER_H__

#include <vector>

namespace mt
{

class Entity;
class Map;

class MapRenderer/* : public Gtk::DrawingArea*/
{
public:
	MapRenderer(Map* map);

protected:

protected:
	Map* mMap;
	std::vector<Entity*> mEntites;
};

}

#endif // !__MULTITRACKS_MAPRENDERER_H__