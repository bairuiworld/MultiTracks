#ifndef __MULTITRACKS_ENTITYRENDERER_H__
#define __MULTITRACKS_ENTITYRENDERER_H__

#include <memory>
#include "Component.h"

namespace mt
{

class MapViewport;

class EntityRenderer
{
public:
	//virtual void Draw(Cairo::RefPtr<Cairo::Context> cr, MapViewport* viewport, Component* component) = 0;
};

class Section;
class SectionRenderer : public EntityRenderer
{
public:
	//virtual void Draw(Cairo::RefPtr<Cairo::Context> cr, MapViewport* viewport, Component* component);
};

}

#endif // !__MULTITRACKS_ENTITYRENDERER_H__