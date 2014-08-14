#ifndef __MULTITRACKS_ENTITY_H__
#define __MULTITRACKS_ENTITY_H__

#include <memory>
#include "Component.h"

namespace mt
{

class EntityRenderer;

class Entity
{
public:
	void SetRenderer(std::shared_ptr<EntityRenderer> renderer) { mRenderer = renderer; }
	//void Draw(Cairo::RefPtr<Cairo::Context> cr, MapViewport* viewport);

private:
	Component* mComponent;
	std::shared_ptr<EntityRenderer> mRenderer;
};

}

#endif // !__MULTITRACKS_ENTITY_H__