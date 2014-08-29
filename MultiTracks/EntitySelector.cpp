#include "Track.h"
#include "Section.h"
#include "MapViewport.h"
#include "Algorithm.h"
#include "EntitySelector.h"

namespace mt
{

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

void SectionSelector::Select(ComponentSelector* selector)
{
	if(!mValid)	Compile(selector);
	if(!mBoundingBox.IsInside(selector->GetPoint())) return;
	if(selector->GetSelectable() & Selectable::SectionEnd)
		if(SelectSectionEnd(selector))
			return;
	if(selector->GetSelectable() & Selectable::Section)
		SelectSection(selector);
}

void SectionSelector::SelectSection(ComponentSelector* selector)
{
	const Vector2d* last = nullptr;
	for(const Vector2d& p : mCompiledLocations)
	{
		if(last == nullptr) { last = &p; continue; }
		Vector2d nearest;
		double distanceSeg = DistanceToSegment(selector->GetPoint(), *last, p, &nearest);
		if(selector->Validate(distanceSeg))
			selector->SetSelected(mSection, Selectable::Section, distanceSeg, nearest);
		last = &p;
	}
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

}