#ifndef __MULTITRACKS_TILE_H__
#define __MULTITRACKS_TILE_H__

#include <Windows.h>
#include <gdiplus.h>
#include <mutex>
#include "SimpleSignal.h"
#include "ThreadPool.h"
#include "Vector.h"

namespace mt
{

class MapSource;
class Image;

class Tile
{
public:
	Tile(const Vector3i& coord, MapSource* mapSource);
	Tile(const Tile& rhs) = delete;
	~Tile();

	bool Download(bool background);
	void Wait();

	const Vector3i& GetCoordinates() const { return mCoordinates; }
	Gdiplus::Image* GetImage() const { return mImage; }
	
	sig::Signal<void(Tile*)> SignalReady;

private:
	void DownloadTask();
	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

private:
	Vector3i mCoordinates;
	MapSource* mMapSource;
	Gdiplus::Image* mImage;
	bool mLoaded;
	std::shared_ptr<TaskResult<void>> mTask;
	std::mutex signal_lock;
};

}

#endif // !__MULTITRACKS_TILE_H__
