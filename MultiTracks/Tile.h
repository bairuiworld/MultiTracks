#ifndef __MULTITRACKS_TILE_H__
#define __MULTITRACKS_TILE_H__

#include <gdkmm\pixbuf.h>
#include <glibmm\refptr.h>
#include <sigc++\sigc++.h>
#include "ThreadPool.h"
#include "Vector.h"

namespace mt
{

class MapSource;

class Tile
{
public:
	Tile(const Vector3i& coord, MapSource* mapSource);

	bool Download(bool background);
	void Wait();

	const Vector3i& GetCoordinates() const { return mCoordinates; }
	Glib::RefPtr<Gdk::Pixbuf> GetPixbuf() const { return mPixbuf; }

	sigc::signal<void, Tile*> signal_ready;

private:
	void DownloadTask();
	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

private:
	Vector3i mCoordinates;
	MapSource* mMapSource;
	Glib::RefPtr<Gdk::Pixbuf> mPixbuf;
	bool mLoaded;
	std::future<void> mFuture;
};

}

#endif // !__MULTITRACKS_TILE_H__
