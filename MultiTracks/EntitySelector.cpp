#include "Track.h"
#include "Section.h"
#include "MapViewport.h"
#include "Algorithm.h"
#include "EntitySelector.h"

namespace mt
{

ComponentSelector::ComponentSelector(const Vector2d& point, const MapViewport* viewport, int selectable) :
mPoint(point), mViewport(viewport), mSelectable(selectable), mComponent(nullptr), mDistance(std::numeric_limits<double>::max())
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
	if(selector->GetSelectable() & Selectable::Section)
		SelectSection(selector);
	if(selector->GetSelectable() & Selectable::SectionEnd)
		SelectSectionEnd(selector);
}

void SectionSelector::SelectSection(ComponentSelector* selector)
{
	const Vector2d* last = nullptr;
	for(const Vector2d& p : mCompiledLocations)
	{
		if(last == nullptr) { last = &p; continue; }
		Vector2d nearest;
		double distanceSeg = DistanceToSegment(selector->GetPoint(), *last, p, &nearest);
		if(distanceSeg < selector->GetDistance())
			selector->SetSelected(mSection, Selectable::Section, distanceSeg, nearest);
		last = &p;
	}
}

void SectionSelector::SelectSectionEnd(ComponentSelector* selector)
{
	// todo
}

void SectionSelector::Compile(ComponentSelector* selector)
{
	mCompiledLocations.clear();
	for(const Location& loc : mSection->GetLocations())
		mCompiledLocations.push_back(selector->GetViewport()->LocationToPixel(loc));
}

TrackSelector::TrackSelector(const Track* track)
{
	for(Section* section : track->GetSections())
		mCompiledSections.push_back(new SectionSelector(section));
}

TrackSelector::~TrackSelector()
{
	for(SectionSelector* selector : mCompiledSections)
		delete selector;
}

void TrackSelector::Select(ComponentSelector* selector)
{
	if(!mValid) Compile(selector);
	selector->Select(mCompiledSections.begin(), mCompiledSections.end());
}

void TrackSelector::Compile(ComponentSelector* selector)
{
	for(SectionSelector* secsel : mCompiledSections)
		secsel->Compile(selector);
}

}