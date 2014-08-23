#ifndef __MULTITRACKS_ENTITY_H__
#define __MULTITRACKS_ENTITY_H__

#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include "Component.h"
#include "EntityRenderer.h"
#include "EntitySelector.h"

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
	std::shared_ptr<EntitySelector> GetSelector() { return mSelector; }

private:
	const Component* mComponent;
	std::shared_ptr<EntityRenderer> mRenderer;
	std::shared_ptr<EntitySelector> mSelector;
};

template <class T>
Entity::Entity(T* component) :
mComponent(component)
{
	mRenderer = RendererExists<T>::value ? DefaultRenderer<T>::value : nullptr;
	mSelector = std::is_base_of<MapObjectContainer, T>::value ? std::make_shared<MapObjectSelector>(component) : nullptr;
}

template <class T>
Entity::Entity(const T* component) :
mComponent(component)
{
	mRenderer = RendererExists<T>::value ? DefaultRenderer<T>::value : nullptr;
	mSelector = std::is_base_of<MapObjectContainer, T>::value ? std::make_shared<MapObjectSelector>(component) : nullptr;
}

}

#endif // !__MULTITRACKS_ENTITY_H__