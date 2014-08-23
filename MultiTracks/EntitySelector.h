#ifndef __MULTITRACKS_ENTITYSELECTOR_H__
#define __MULTITRACKS_ENTITYSELECTOR_H__

#include <Windows.h>
#undef min
#undef max
#include <vector>
#include <memory>
#include "Vector.h"
#include "BoundingBox.h"

namespace mt
{

class Component;
class MapViewport;
class Entity;

namespace Selectable
{
static const int None = 0;
static const int Section = 1 << 0;
static const int SectionEnd = 1 << 1;
static const int Location = 1 << 2;
}

class EntitySelector;
template <class T> struct SelectorBuilder { static std::shared_ptr<EntitySelector> Make(const Component*) { return nullptr; } };

class EntitySelector;
class ComponentSelector
{
public:
	ComponentSelector(const Vector2d& point, const MapViewport* viewport, int selectable, double threshold);

	template <class Itr>
	typename std::enable_if<std::is_same<Entity*, typename Itr::value_type>::value>::type Select(Itr begin, Itr end);
	template <class Itr>
	typename std::enable_if<std::is_base_of<EntitySelector, typename std::remove_pointer<typename Itr::value_type>::type>::value>::type Select(Itr begin, Itr end);
	void Select(const ComponentSelector& selector);

	const Vector2d& GetPoint() const { return mPoint; }
	const MapViewport* GetViewport() const { return mViewport; }
	int GetSelectable() const { return mSelectable; }
	double GetDistance() const { return mDistance; }
	bool Validate(double distance) { return distance <= mThreshold && distance <= mDistance; }
	Component* GetComponent() const { return mComponent; }
	int GetSelected() const { return mSelected; }

	void SetSelected(Component* component, int selected, double distance, const Vector2d& nearest);

private:
	const Vector2d& mPoint;
	const MapViewport* mViewport;
	int mSelectable;
	double mThreshold;
	Component* mComponent;
	int mSelected;
	double mDistance;
	Vector2d mNearestPoint;
};

template <class Itr>
typename std::enable_if<std::is_same<Entity*, typename Itr::value_type>::value>::type ComponentSelector::Select(Itr begin, Itr end)
{
	std::for_each(begin, end, [this](Entity* entity) {
		std::shared_ptr<EntitySelector> sel = entity->GetSelector();
		if(sel)
			sel->Select(this);
	});
}

template <class Itr>
typename std::enable_if<std::is_base_of<EntitySelector, typename std::remove_pointer<typename Itr::value_type>::type>::value>::type ComponentSelector::Select(Itr begin, Itr end)
{
	std::for_each(begin, end, [this](EntitySelector* selector) {
		selector->Select(this);
	});
}

class EntitySelector
{
public:
	EntitySelector() : mValid(false) {}
	virtual void Select(ComponentSelector* selector) = 0;
	void Invalidate() { mValid = false; }

protected:
	virtual void Compile(ComponentSelector* selector) = 0;

protected:
	bool mValid;
};

class Section;
class SectionSelector : public EntitySelector
{
public:
	SectionSelector(Section* section) : mSection(section), mBoundingBox(10) {}

	virtual void Select(ComponentSelector* selector);

protected:
	friend class MapObjectSelector;
	virtual void Compile(ComponentSelector* selector);

	void SelectSection(ComponentSelector* selector);
	bool SelectSectionEnd(ComponentSelector* selector);

private:
	Section* mSection;
	std::vector<Vector2d> mCompiledLocations;
	BoundingBox<double> mBoundingBox;
};
template <> struct SelectorBuilder<Section>
{
	static std::shared_ptr<SectionSelector> Make(Section* section)
	{
		return std::make_shared<SectionSelector>(section);
	}
};

class MapObjectSelector : public EntitySelector
{
public:
	using CompiledSectionList = std::vector<SectionSelector*>;

	MapObjectSelector(const MapObjectContainer* container);
	virtual ~MapObjectSelector();

	virtual void Select(ComponentSelector* selector);

protected:
	virtual void Compile(ComponentSelector* selector);

private:
	CompiledSectionList mCompiledSections;
};
template <> struct SelectorBuilder<MapObjectContainer>
{
	static std::shared_ptr<MapObjectSelector> Make(const MapObjectContainer* container)
	{
		return std::make_shared<MapObjectSelector>(container);
	}
};

}

#endif // !__MULTITRACKS_ENTITYSELECTOR_H__