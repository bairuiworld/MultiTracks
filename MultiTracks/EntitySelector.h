#ifndef __MULTITRACKS_ENTITYSELECTOR_H__
#define __MULTITRACKS_ENTITYSELECTOR_H__

#include <Windows.h>
#undef min
#undef max
#include <vector>
#include <set>
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

	void Select(Selector* selector);

	MapViewport* GetViewport() const { return mViewport; }
	double GetThreshold() const { return mThreshold; }
	const Vector2d& GetPoint() const { return mPoint; }
	bool Validate(double distance, Selector* selector);
	void EmitResult();

private:
	MapViewport* mViewport;
	Vector2d mPoint;
	double mThreshold;
	double mDistance;
	Selector* mCurrentSelector;
	std::set<Selector*> mSelectors;
};

class Selector
{
public:
	Selector() : mPriority(0) {}
	virtual ~Selector() = default;

	void SetPriority(int priority) { mPriority = priority; }
	int GetPriority() const { return mPriority; }

	virtual void Select(SelectionTracker* tracker) = 0;
	virtual void EmitResult() = 0;
	virtual void ClearResult() = 0;
	virtual void Invalidate() {};

protected:
	int mPriority;
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
		mValid = false;
	}

	void Add(std::initializer_list<MapObjectContainer*> tracks)
	{
		for(MapObjectContainer* track : tracks)
			Add(track->GetSections().begin(), track->GetSections().end());
	}

	virtual void Invalidate() { mValid = false; }
	void Compile(SelectionTracker* tracker);

protected:
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
	virtual void ClearResult();

	WayPoint* GetCurrentWayPoint() const { return mLastWayPoint; }

private:
	void SelectWayPoint(SelectionTracker* tracker, const SectionInfo& si);

private:
	WayPoint* mWayPoint;
	WayPoint* mLastWayPoint;

public:
	sig::Signal<void(WayPoint*)> SignalSelection;
	sig::Signal<void(WayPoint*)> SignalDeselection;
};

class SectionSelector : public SectionSelectorBase
{
public:
	SectionSelector();
	virtual ~SectionSelector() = default;

	virtual void Select(SelectionTracker* tracker);
	virtual void EmitResult();
	virtual void ClearResult();

	Section* GetCurrentSection() const { return mLastSection; }

private:
	void SelectSection(SelectionTracker* tracker, const SectionInfo& si);

private:
	Section* mSection;
	Vector2d mNearest;

	Section* mLastSection;
	Vector2d mLastNearest;

public:
	sig::Signal<void(Section*, const Vector2d&)> SignalSelection;
	sig::Signal<void(Section*, const Vector2d&)> SignalDeselection;
};

}

#endif // !__MULTITRACKS_ENTITYSELECTOR_H__