#include "Track.h"
#include "Section.h"
#include "WayPoint.h"
#include "MapViewport.h"
#include "Algorithm.h"
#include "EntitySelector.h"

namespace mt
{

SelectionTracker::SelectionTracker(MapViewport* viewport, const Vector2d& point, double threshold) :
mViewport(viewport), mPoint(point), mThreshold(threshold), mDistance(std::numeric_limits<double>::max()), mCurrentSelector(nullptr)
{

}

bool SelectionTracker::Validate(double distance, Selector* selector)
{
	mSelectors.insert(selector);
	if(distance <= mThreshold && distance <= mDistance)
	{
		mCurrentSelector = selector;
		mDistance = distance;
		return true;
	}
	return false;
}

void SelectionTracker::EmitResult()
{
	for(Selector* s : mSelectors)
	{
		if(mCurrentSelector == s)
			s->EmitResult();
		else
			s->ClearResult();
	}
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
mWayPoint(nullptr), mLastWayPoint(nullptr)
{

}

WayPointSelector::~WayPointSelector()
{
	delete mWayPoint;
	if(mLastWayPoint)
		ClearResult();
}

void WayPointSelector::Select(SelectionTracker* tracker)
{
	Compile(tracker->GetViewport());
	for(auto it : mSections)
		SelectWayPoint(tracker, it);
}

void WayPointSelector::SelectWayPoint(SelectionTracker* tracker, const SectionInfo& si)
{
	if(!si.bb.IsInside(tracker->GetPoint())) return;
	const Vector2d* last = nullptr;
	Section::LocationList::const_iterator it = si.section->GetLocations().begin();
	for(const Vector2d& p : si.pixels)
	{
		if(last == nullptr) { last = &p; it++; continue; }
		Vector2d nearest;
		double distanceSeg = DistanceToSegment(tracker->GetPoint(), *last, p, &nearest);
		if(tracker->Validate(distanceSeg, this))
		{
			delete mWayPoint;
			mWayPoint = new WayPoint(si.section, it, tracker->GetViewport()->PixelToLocation(nearest));
		}
		last = &p;
		it++;
	}
}

void WayPointSelector::EmitResult()
{
	if(!mWayPoint) return;
	ClearResult();
	mLastWayPoint = mWayPoint;
	mWayPoint = nullptr;
	SignalSelection.emit(mLastWayPoint);
}

void WayPointSelector::ClearResult()
{
	if(!mLastWayPoint) return;
	SignalDeselection.emit(mLastWayPoint);
	delete mLastWayPoint;
	mLastWayPoint = nullptr;
}

SectionSelector::SectionSelector() :
mSection(nullptr), mLastSection(nullptr)
{

}

void SectionSelector::Select(SelectionTracker* tracker)
{
	Compile(tracker->GetViewport());
	for(auto it : mSections)
		SelectSection(tracker, it);
}

void SectionSelector::SelectSection(SelectionTracker* tracker, const SectionInfo& si)
{
	if(!si.bb.IsInside(tracker->GetPoint())) return;
	const Vector2d* last = nullptr;
	for(const Vector2d& p : si.pixels)
	{
		if(last == nullptr) { last = &p; continue; }
		Vector2d nearest;
		double distanceSeg = DistanceToSegment(tracker->GetPoint(), *last, p, &nearest);
		if(tracker->Validate(distanceSeg, this))
		{
			mSection = si.section;
			mNearest = nearest;
		}
		last = &p;
	}
}

void SectionSelector::EmitResult()
{
	if(!mSection) return;
	ClearResult();
	mLastSection = mSection;
	mLastNearest = mNearest;
	mSection = nullptr;
	SignalSelection.emit(mLastSection, mLastNearest);
}

void SectionSelector::ClearResult()
{
	if(!mLastSection) return;
	SignalDeselection.emit(mLastSection, mLastNearest);
	mLastSection = nullptr;
}

/*
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
*/
}