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

void SelectionTracker::Select(Selector* selector)
{
	mSelectors.insert(selector);
	selector->Select(this);
}

bool SelectionTracker::Validate(double distance, Selector* selector)
{
	bool have_priority = !mCurrentSelector || mCurrentSelector->GetPriority() <= selector->GetPriority();
	if(!have_priority) return false;
	if(distance <= mThreshold && (mCurrentSelector != selector || distance <= mDistance))
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

void SectionSelectorBase::Compile(SelectionTracker* tracker)
{
	if(mValid) return;

	mCompiledSections.clear();
	for(Section* s : mSectionSources)
		mCompiledSections.emplace_back(s);
	for(MapObjectContainer* c : mContainerSources)
		for(Section* s : c->GetSections())
			mCompiledSections.emplace_back(s);

	for(SectionInfo& s : mCompiledSections)
	{
		s.pixels.clear();
		s.bb.Clear();
		for(const Location& loc : s.section->GetLocations())
		{
			Vector2d v = tracker->GetViewport()->LocationToPixel(loc);
			s.pixels.emplace_back(v);
			s.bb.Add(v);
		}
		s.bb.SetPadding(tracker->GetThreshold());
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
	Compile(tracker);
	for(auto it : mCompiledSections)
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
			mWayPoint = new WayPoint(si.section, it - 1, tracker->GetViewport()->PixelToLocation(nearest));
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

SectionSelector::~SectionSelector()
{
	ClearResult();
}

void SectionSelector::Select(SelectionTracker* tracker)
{
	Compile(tracker);
	for(auto it : mCompiledSections)
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
	if(mSection == mLastSection)
	{
		mSection = nullptr;
		return;
	}
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

SectionEndSelector::SectionEndSelector() :
mSectionEnd(nullptr), mLastSectionEnd(nullptr), mSection(nullptr), mLastSection(nullptr)
{

}

SectionEndSelector::~SectionEndSelector()
{
	ClearResult();
}

void SectionEndSelector::Select(SelectionTracker* tracker)
{
	Compile(tracker);
	for(auto it : mCompiledSections)
		SelectSectionEnd(tracker, it);
}

void SectionEndSelector::SelectSectionEnd(SelectionTracker* tracker, const SectionInfo& si)
{
	bool found = false;
	const Vector2d& first = si.pixels.front();
	double d = first.GetDistance(tracker->GetPoint());
	if(tracker->Validate(d, this))
	{
		mSection = si.section;
		mSectionEnd = mSection->GetFirstLocation();
	}

	if(si.pixels.size() < 1) return;
	const Vector2d& last = si.pixels.back();
	d = last.GetDistance(tracker->GetPoint());
	if(tracker->Validate(d, this))
	{
		mSection = si.section;
		mSectionEnd = mSection->GetLastLocation();
	}
}

void SectionEndSelector::EmitResult()
{
	if(!mSection) return;
	if(mSectionEnd == mLastSectionEnd)
	{
		mSection = nullptr;
		mSectionEnd = nullptr;
		return;
	}
	ClearResult();
	mLastSection = mSection;
	mLastSectionEnd = mSectionEnd;
	mSection = nullptr;
	mSectionEnd = nullptr;
	SignalSelection.emit(mLastSection, mLastSectionEnd);
}

void SectionEndSelector::ClearResult()
{
	if(!mLastSectionEnd) return;
	SignalDeselection.emit(mLastSection, mLastSectionEnd);
	mLastSection = nullptr;
	mLastSectionEnd = nullptr;
}

}