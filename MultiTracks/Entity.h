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
	Entity(const T* component);

	const Component* GetComponent() const { return mComponent; }

	void SetRenderer(std::shared_ptr<EntityRenderer> renderer) { mRenderer = renderer; }
	void Draw(Gdiplus::Graphics* g, MapViewport* viewport);

	int IncRef() { mRef++; return mRef; }
	int DecRef() { mRef--; return mRef; }
	int GetRef() { return mRef; }

private:
	const Component* mComponent;
	std::shared_ptr<EntityRenderer> mRenderer;
	int mRef;
};

template <class T>
Entity::Entity(const T* component) :
mComponent(component), mRef(1)
{
	mComponent = component;
	mRenderer = DefaultRenderer<T>::value;
}

}

#endif // !__MULTITRACKS_ENTITY_H__