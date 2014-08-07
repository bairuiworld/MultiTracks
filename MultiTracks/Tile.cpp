#include <gdkmm\pixbufloader.h>
#include "DownloadManager.h"
#include "MapSource.h"
#include "Tile.h"

#include <iostream>

namespace mt
{
namespace map
{

Tile::Tile(const Point3D& coord, MapSource* mapSource) :
	mCoordinates(coord), mMapSource(mapSource), mPixbuf(), mLoaded(false)
{

}

bool Tile::Download(bool background)
{
	if(!mFuture.valid())
	{
		mFuture = DownloadManager::GetPool()->enqueue(std::bind(&Tile::DownloadTask, this));
		if(!background)
			mFuture.wait();
	}
	return mLoaded;
}

void Tile::Wait()
{
	if(mFuture.valid())
		mFuture.wait();
}

size_t Tile::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size*nmemb;
	Glib::RefPtr<Gdk::PixbufLoader>* loader = static_cast<Glib::RefPtr<Gdk::PixbufLoader>*>(userp);
	(*loader)->write((guint8*)contents, realsize);
	return realsize;
}

void Tile::DownloadTask()
{
	std::cout << "Download " << mCoordinates.x << ", " << mCoordinates.y << ", " << mCoordinates.z << std::endl;
	CURL* curl;
	CURLcode res;
 
	Glib::RefPtr<Gdk::PixbufLoader> loader = Gdk::PixbufLoader::create();
 
	curl = curl_easy_init();
	mMapSource->InitSession(curl, mCoordinates);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Tile::WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&loader);
	res = curl_easy_perform(curl);
	
	int res_code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);

	if(res == CURLE_OK && res_code == 200)
	{
		loader->close();

		mPixbuf = loader->get_pixbuf();
		if(mPixbuf)
		{
			mLoaded = true;
			signal_ready.emit(this);
		}
	}
 
	curl_easy_cleanup(curl);
}

}
}