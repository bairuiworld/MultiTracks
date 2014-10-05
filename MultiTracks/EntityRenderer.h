#ifndef __MULTITRACKS_ENTITYRENDERER_H__
#define __MULTITRACKS_ENTITYRENDERER_H__

#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include <iostream>
#include "Component.h"

namespace mt
{

namespace Shape
{
static const int Circle = 1;
static const int FilledCircle = 2;
}


class EntityRenderer;
struct NullRenderer {};
template <class T> struct RendererSelector { typedef NullRenderer type; };
template <class T> struct RendererExists { static const bool value = !std::is_same<RendererSelector<T>::type, NullRenderer>::value; };
template <class T, class R = typename RendererSelector<T>::type> struct DefaultRenderer { static const std::shared_ptr<R> value; };
template <class T, class R> const std::shared_ptr<R> DefaultRenderer<T, R>::value = std::make_shared<R>();
template <class T> struct DefaultRenderer<T, NullRenderer> { static const std::shared_ptr<EntityRenderer> value; };
template <class T> const std::shared_ptr<EntityRenderer> DefaultRenderer<T, NullRenderer>::value = nullptr;

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

class Location;
class LocationRenderer : public EntityRenderer
{
public:
	virtual void Draw(Gdiplus::Graphics* g, MapViewport* viewport, const Component* component);
};
template <> struct RendererSelector<Location> { typedef LocationRenderer type; };

}

#endif // !__MULTITRACKS_ENTITYRENDERER_H__