#ifndef __MULTITRACKS_ENTITYRENDERER_H__
#define __MULTITRACKS_ENTITYRENDERER_H__

#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include <iostream>
#include "Component.h"

namespace mt
{

struct NullRenderer {};
template <class T> struct RendererSelector { typedef NullRenderer type; };
template <class T> struct RendererExists { static const bool value = !std::is_same<RendererSelector<T>::type, NullRenderer>::value; };
template <class T> struct DefaultRenderer { static const std::shared_ptr<typename RendererSelector<T>::type> value; };
template <class T> const std::shared_ptr<typename RendererSelector<T>::type>
	DefaultRenderer<T>::value = std::make_shared<typename RendererSelector<T>::type>();

class MapViewport;

class EntityRenderer
{
public:
	virtual void Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component) = 0;
};

class Section;
class SectionRenderer : public EntityRenderer
{
public:
	virtual void Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component);
};
template <> struct RendererSelector<Section> { typedef SectionRenderer type; };

class MapObjectContainer;
class MapObjectContainerRenderer : public EntityRenderer
{
public:
	virtual void Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component);
};
template <> struct RendererSelector<MapObjectContainer> { typedef MapObjectContainerRenderer type; };

class Track;
class TrackRenderer : public EntityRenderer
{
public:
	virtual void Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component);
};
template <> struct RendererSelector<Track> { typedef TrackRenderer type; };

}

#endif // !__MULTITRACKS_ENTITYRENDERER_H__