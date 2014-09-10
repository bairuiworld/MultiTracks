#include "Track.h"
#include "Section.h"
#include "WayPoint.h"
#include "MapViewport.h"
#include "Algorithm.h"
#include "EntitySelector.h"

namespace mt
{

SelectionTracker::SelectionTracker(MapViewport* viewport, const Vector2d& point, double threshold) :
mViewport(viewport), mPoint(point), mThreshold(threshold), mDistance(std::numeric_limits<double>::max()), mSelector(nullptr)
{

}

bool SelectionTracker::Validate(double distance, Selector* selector)
{
	if(distance < mThreshold && distance < mDistance)
	{
		mSelector = selector;
		mDistance = distance;
		return true;
	}
	return false;
}

void SelectionTracker::EmitResult()
{
	if(mSelector)
		mSelector->EmitResult();
}

void SectionSelectorBase::Compile(MapViewport* viewport)
{
	if(mValid) return;
	for(SectionInfo& s : mSections)
	{
		s.pixels.clear();
		s.bb.Clear();
		for(const Location& loc : s.section->GetLocations())
		{
			Vector2d v = viewport->LocationToPixel(loc);
			s.pixels.emplace_back(v);
			s.bb.Add(v);
		}
	}
	mValid = true;
}

WayPointSelector::WayPointSelector() :
mWayPoint(nullptr)
{

}

WayPointSelector::~WayPointSelector()
{
	delete mWayPoint;
}

void WayPointSelector::Select(SelectionTracker* tracker)
{
	Compile(tracker->GetViewport());
	for(auto it : mSections)
	{
		SelectWayPoint(tracker, it.section, it.pixels);
	}
}

void WayPointSelector::SelectWayPoint(SelectionTracker* tracker, Section* section, const std::vector<Vector2d>& pixels)
{
	const Vector2d* last = nullptr;
	Section::LocationList::const_iterator it = section->GetLocations().begin();
	for(const Vector2d& p : pixels)
	{
		if(last == nullptr) { last = &p; it++; continue; }
		Vector2d nearest;
		double distanceSeg = DistanceToSegment(tracker->GetPoint(), *last, p, &nearest);
		if(tracker->Validate(distanceSeg, this))
		{
			delete mWayPoint;
			mWayPoint = new WayPoint(section, it, tracker->GetViewport()->PixelToLocation(nearest));
		}
		last = &p;
		it++;
	}
}

void WayPointSelector::EmitResult()
{
	if(mWayPoint)
		SignalSelection.emit(mWayPoint);
}

/**/

ComponentSelector::ComponentSelector(const MapViewport* viewport, double threshold) :
mViewport(viewport), mSelectable(0), mComponent(nullptr), mSelected(Selectable::None),
mDistance(std::numeric_limits<double>::max()), mThreshold(threshold)
{

}

void ComponentSelector::SetSelected(Component* component, int selected, double distance, const Vector2d& nearest)
{
	mComponent = component;
	mSelected = selected;
	mDistance = distance;
	mNearestPoint.Set(nearest);
}
/*
void SectionSelector::Select(ComponentSelector* selector)
{
	if(!mValid)	Compile(selector);
	if(!mBoundingBox.IsInside(selector->GetPoint())) return;

	if(selector->GetSelectable() & Selectable::WayPoint)
	{
		SelectWayPoint(selector);
		return;
	}
	if(selector->GetSelectable() & Selectable::SectionEnd)
		if(SelectSectionEnd(selector))
			return;
	if(selector->GetSelectable() & Selectable::Section)
		SelectSection(selector);
}

bool SectionSelector::SelectSection(ComponentSelector* selector)
{
	bool found = false;
	const Vector2d* last = nullptr;
	for(const Vector2d& p : mCompiledLocations)
	{
		if(last == nullptr) { last = &p; continue; }
		Vector2d nearest;
		double distanceSeg = DistanceToSegment(selector->GetPoint(), *last, p, &nearest);
		if(selector->Validate(distanceSeg))
		{
			found = true;
			selector->SetSelected(mSection, Selectable::Section, distanceSeg, nearest);
		}
		last = &p;
	}
	return found;
}

bool SectionSelector::SelectSectionEnd(ComponentSelector* selector)
{
	bool found = false;
	const Vector2d& first = mCompiledLocations.front();
	double d = first.GetDistance(selector->GetPoint());
	if(selector->Validate(d))
	{
		selector->SetSelected(const_cast<Location*>(mSection->GetFirstLocation()), Selectable::SectionEnd, d, first);
		found = true;
	}

	if(mCompiledLocations.size() < 1) return found;
	const Vector2d& last = mCompiledLocations.back();
	d = last.GetDistance(selector->GetPoint());
	if(selector->Validate(d))
	{
		selector->SetSelected(const_cast<Location*>(mSection->GetLastLocation()), Selectable::SectionEnd, d, last);
		found = true;
	}
	return found;
}

void SectionSelector::SelectWayPoint(ComponentSelector* selector)
{
	const Vector2d* last = nullptr;
	Section::LocationList::const_iterator it = mSection->GetLocations().begin();
	for(const Vector2d& p : mCompiledLocations)
	{
		if(last == nullptr) { last = &p; it++; continue; }
		Vector2d nearest;
		double distanceSeg = DistanceToSegment(selector->GetPoint(), *last, p, &nearest);
		if(selector->Validate(distanceSeg))
			selector->SetSelected(new WayPoint(mSection, it, selector->GetViewport()->PixelToLocation(nearest)), Selectable::WayPoint, distanceSeg, nearest);
		last = &p;
		it++;
	}
}

void SectionSelector::Compile(ComponentSelector* selector)
{
	mCompiledLocations.clear();
	mBoundingBox.Clear();
	for(const Location& loc : mSection->GetLocations())
	{
		Vector2d v = selector->GetViewport()->LocationToPixel(loc);
		mCompiledLocations.push_back(v);
		mBoundingBox.Add(v);
	}
	mValid = true;
}

MapObjectSelector::MapObjectSelector(const MapObjectContainer* container)
{
	for(Section* section : container->GetSections())
		mCompiledSections.push_back(new SectionSelector(section));
}

MapObjectSelector::~MapObjectSelector()
{
	for(SectionSelector* selector : mCompiledSections)
		delete selector;
}

void MapObjectSelector::Select(ComponentSelector* selector)
{
	if(!mValid) Compile(selector);
	selector->Select(mCompiledSections.begin(), mCompiledSections.end());
}

void MapObjectSelector::Compile(ComponentSelector* selector)
{
	for(SectionSelector* secsel : mCompiledSections)
		secsel->Compile(selector);
	mValid = true;
}
*/
}