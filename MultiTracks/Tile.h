#ifndef __MULTITRACKS_TILE_H__
#define __MULTITRACKS_TILE_H__

#include <gdkmm\pixbuf.h>
#include <glibmm\refptr.h>
#include <sigc++\sigc++.h>
#include "Point3D.h"
#include "ThreadPool.h"

namespace mt
{
namespace map
{

class MapSource;

class Tile
{
public:
	Tile(const Point3D& coord, MapSource* mapSource);

	bool Download(bool background);
	void Wait();

	const Point3D& GetCoordinates() const { return mCoordinates; }
	Glib::RefPtr<Gdk::Pixbuf> GetPixbuf() const { return mPixbuf; }

	sigc::signal<void, Tile*> signal_ready;

private:
	void DownloadTask();
	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

private:
	Point3D mCoordinates;
	MapSource* mMapSource;
	Glib::RefPtr<Gdk::Pixbuf> mPixbuf;
	bool mLoaded;
	std::future<void> mFuture;
};

}
}

#endif // !__MULTITRACKS_TILE_H__
