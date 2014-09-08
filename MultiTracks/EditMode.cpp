#include <Windows.h>
#include <gdiplus.h>
#include "Track.h"
#include "Section.h"
#include "MapRenderer.h"
#include "EditMode.h"

namespace mt
{

EditMode::EditMode(WindowMapRenderer* renderer) :
mMapRenderer(renderer)
{

}

TrackEditMode::TrackEditMode(WindowMapRenderer* renderer, Track* track) :
EditMode(renderer), mTrack(track)
{
	mTrack->GetProperties().Push()
		.Set<int>("color", Gdiplus::Color::Magenta)
		.Set("linewidth", 3.f);
	const Location* end = mTrack->GetLastLocation();
	mMapRenderer->AddComponent(mTrack);
	mMapRenderer->AddComponent(end);
	mMapRenderer->Invalidate();
}

TrackEditMode::~TrackEditMode()
{
	mTrack->GetProperties().Pop();
	mMapRenderer->RemoveComponent(mTrack);
	mMapRenderer->RemoveComponent(mTrack->GetLastLocation());
	mMapRenderer->Invalidate();
}

void TrackEditMode::AppendLocation(const Location& location)
{
	mMapRenderer->RemoveComponent(mTrack->GetLastLocation());
	mTrack->GetLastSection()->Add(location);
	mMapRenderer->AddComponent(mTrack->GetLastLocation());
	mMapRenderer->Invalidate();
}

}