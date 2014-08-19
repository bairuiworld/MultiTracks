#ifndef __MULTITRACKS_ENTITY_H__
#define __MULTITRACKS_ENTITY_H__

#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include "Component.h"
#include "EntityRenderer.h"

#include <iostream>

namespace mt
{

class EntityRenderer;

class Entity
{
public:
	template <class T>
	Entity(T* component);
	template <class T>
	Entity(const T* component);

	void SetRenderer(std::shared_ptr<EntityRenderer> renderer) { mRenderer = renderer; }
	void Draw(Gdiplus::Graphics* g, MapViewport* viewport);

private:
	const Component* mComponent;
	std::shared_ptr<EntityRenderer> mRenderer;
};

template <class T>
Entity::Entity(T* component) :
mComponent(component)
{
	mRenderer = RendererExists<T>::value ? DefaultRenderer<T>::value : nullptr;
}

template <class T>
Entity::Entity(const T* component) :
mComponent(component)
{
	mRenderer = RendererExists<T>::value ? DefaultRenderer<T>::value : nullptr;
}

}

#endif // !__MULTITRACKS_ENTITY_H__