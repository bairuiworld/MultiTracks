#ifndef __MULTITRACKS_ENTITYRENDERER_H__
#define __MULTITRACKS_ENTITYRENDERER_H__

#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include "Component.h"

namespace mt
{

struct NullRenderer {};
template <class T>
struct RendererSelector
{
	typedef NullRenderer type;
};

template <class T>
struct RendererExists
{
	static const bool value = !std::is_same<RendererSelector<T>::type, NullRenderer>::value;
};

class MapViewport;

class EntityRenderer
{
public:
	virtual void Draw(Gdiplus::Graphics* g, MapViewport* viewport, Component* component) = 0;
};

class Section;
class SectionRenderer : public EntityRenderer
{
public:
	virtual void Draw(Gdiplus::Graphics* g, MapViewport* viewport, Component* component);
};

template <>
struct RendererSelector<Section>
{
	typedef SectionRenderer type;
};

}

#endif // !__MULTITRACKS_ENTITYRENDERER_H__