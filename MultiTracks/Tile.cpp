#include <il/il.h>
#include <il/ilut.h>
#include "MapSource.h"
#include "Tile.h"
#include "DownloadManager.h"
#include <iostream>
#include <fstream>
namespace mt
{

std::mutex Tile::devil_mutex;

Tile::Tile(const Vector3i& coord, MapSource* mapSource) :
mCoordinates(coord), mMapSource(mapSource), mImage(nullptr), mLoaded(false), mDispose(false)
{

}

Tile::~Tile()
{
	if(!mTask->Cancel())
		mTask->GetFuture().wait();
	delete mImage;
}

bool Tile::Download(bool background)
{
	if(!mTask)
	{
		mTask = DownloadManager::GetPool()->enqueue(std::bind(&Tile::DownloadTask, this));
		if(!background)
			Wait();
	}
	return mLoaded;
}

void Tile::Wait()
{
	{
		std::lock_guard<std::mutex> lock(loaded_mutex);
		if(mLoaded) return;
	}
	if(mTask)
		mTask->GetFuture().wait();
}

size_t Tile::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size*nmemb;
	std::string* data = static_cast<std::string*>(userp);
	data->append((const char*)contents, realsize);
	return realsize;
}

void Tile::DownloadTask()
{
	CURL* curl;
	CURLcode res;
	
	std::string data;
 
	curl = curl_easy_init();
	mMapSource->InitSession(curl, mCoordinates);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Tile::WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&data);
	curl_easy_setopt(curl, CURLOPT_PROXY, "http://193.54.120.35:3128");
	res = curl_easy_perform(curl);
	
	int res_code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
	curl_easy_cleanup(curl);

	if(res == CURLE_OK && res_code == 200)
	{
		std::lock_guard<std::mutex> lock(devil_mutex);
		unsigned int imid;
		ilGenImages(1, &imid);
		ilBindImage(imid);
		if(ilLoadL(IL_TYPE_UNKNOWN, data.data(), data.size()))
		{
			HBITMAP hBitmap = ilutConvertToHBitmap(GetDC(NULL));
			mImage = new Gdiplus::Bitmap(hBitmap, NULL);
		}
		ilDeleteImages(1, &imid);
	}

	std::lock_guard<std::mutex> lock(loaded_mutex);
	mLoaded = true;
	SignalReady.emit(this);
	if(mDispose)
		delete this;
}

}