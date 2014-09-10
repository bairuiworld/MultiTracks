#ifndef __MULTITRACKS_ENTITYSELECTOR_H__
#define __MULTITRACKS_ENTITYSELECTOR_H__

#include <Windows.h>
#undef min
#undef max
#include <vector>
#include <memory>
#include "Vector.h"
#include "BoundingBox.h"
#include "SimpleSignal.h"
#include "Track.h"

namespace mt
{

class Component;
class MapViewport;
class Entity;
class WayPoint;

class Selector;
class SelectionTracker
{
public:
	SelectionTracker(MapViewport* viewport, const Vector2d& point, double threshold);

	MapViewport* GetViewport() const { return mViewport; }
	const Vector2d& GetPoint() const { return mPoint; }
	bool Validate(double distance, Selector* selector);
	void EmitResult();

private:
	MapViewport* mViewport;
	Vector2d mPoint;
	double mThreshold;
	double mDistance;
	Selector* mSelector;
};

class Selector
{
public:
	virtual ~Selector() = default;

	virtual void Select(SelectionTracker* tracker) = 0;
	virtual void EmitResult() = 0;
	virtual void Invalidate() {};
};

class SectionSelectorBase : public Selector
{
public:
	SectionSelectorBase() : mValid(false) {}
	virtual ~SectionSelectorBase() = default;

	void Add(std::initializer_list<Section*> sections)
	{
		Add(sections.begin(), sections.end());
	}

	template <class Itr> void Add(Itr begin, Itr end)
	{
		for(Itr it = begin; it != end; it++)
			mSections.emplace_back(*it);
	}

	void Add(std::initializer_list<Track*> tracks)
	{
		for(Track* track : tracks)
			Add(track->GetSections().begin(), track->GetSections().end());
	}

	virtual void Invalidate() { mValid = false; }
	void Compile(MapViewport* viewport);

private:
	struct SectionInfo
	{
		SectionInfo(Section* section_) : section(section_) {}
		Section* section;
		std::vector<Vector2d> pixels;
		BoundingBox<double> bb;
	};

protected:
	std::vector<SectionInfo> mSections;
	bool mValid;
};

class WayPointSelector : public SectionSelectorBase
{
public:
	WayPointSelector();
	virtual ~WayPointSelector();

	virtual void Select(SelectionTracker* tracker);
	virtual void EmitResult();

private:
	void SelectWayPoint(SelectionTracker* tracker, Section* section, const std::vector<Vector2d>& pixels);

private:
	WayPoint* mWayPoint;

public:
	sig::Signal<void(WayPoint*)> SignalSelection;
};

class SectionSelector : public SectionSelectorBase
{
public:
	virtual ~SectionSelector() = default;

	virtual void Select(SelectionTracker* tracker);
	virtual void EmitResult();

private:
	Section* mSection;
	Vector2d mNearest;

public:
	sig::Signal<void(Section*, const Vector2d&)> SignalSelection;
};

/**/
namespace Selectable
{
static const int None = 0;
static const int Section = 1 << 0;
static const int SectionEnd = 1 << 1;
static const int Location = 1 << 2;
static const int WayPoint = 1 << 3;
}

class EntitySelector;
template <class T> struct SelectorBuilder { static std::shared_ptr<EntitySelector> Make(const Component*) { return nullptr; } };

class EntitySelector;
class ComponentSelector
{
public:
	ComponentSelector(const MapViewport* viewport, double threshold);

	template <class Itr>
	typename std::enable_if<std::is_same<Entity*, typename Itr::value_type>::value>::type Select(Itr begin, Itr end);
	template <class Itr>
	typename std::enable_if<std::is_base_of<EntitySelector, typename std::remove_pointer<typename Itr::value_type>::type>::value>::type Select(Itr begin, Itr end);
	void Select(const ComponentSelector& selector);

	const Vector2d& GetPoint() const { return mPoint; }
	void SetPoint(const Vector2d& point) { mPoint = point; }

	const MapViewport* GetViewport() const { return mViewport; }
	
	int GetSelectable() const { return mSelectable; }
	void SetSelectable(int selectable) { mSelectable = selectable; }

	double GetDistance() const { return mDistance; }
	bool Validate(double distance) { return distance <= mThreshold && distance <= mDistance; }
	Component* GetComponent() const { return mComponent; }
	int GetSelected() const { return mSelected; }

	void SetSelected(Component* component, int selected, double distance, const Vector2d& nearest);
	void SetSelected(int selected) { mSelected = selected; }

	const Vector2d& GetNearest() const { return mNearestPoint; }

	void Clear() { mSelected = Selectable::None; mComponent = nullptr; mDistance = std::numeric_limits<double>::max(); }

private:
	Vector2d mPoint;
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
/*class SectionSelector : public EntitySelector
{
public:
	SectionSelector(Section* section) : mSection(section), mBoundingBox(10) {}

	virtual void Select(ComponentSelector* selector);

protected:
	friend class MapObjectSelector;
	virtual void Compile(ComponentSelector* selector);

	bool SelectSection(ComponentSelector* selector);
	bool SelectSectionEnd(ComponentSelector* selector);
	void SelectWayPoint(ComponentSelector* selector);

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
template <> struct SelectorBuilder<Track>
{
	static std::shared_ptr<MapObjectSelector> Make(const MapObjectContainer* container)
	{
		return std::make_shared<MapObjectSelector>(container);
	}
};
*/
}

#endif // !__MULTITRACKS_ENTITYSELECTOR_H__