#include "MapSource.h"
#include "Tile.h"
#include "DownloadManager.h"
#include <iostream>

namespace mt
{

Tile::Tile(const Vector3i& coord, MapSource* mapSource) :
	mCoordinates(coord), mMapSource(mapSource), mImage(nullptr), mLoaded(false)
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
	std::lock_guard<std::mutex> lock(loaded_mutex);
	if(mLoaded) return;
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
	res = curl_easy_perform(curl);
	
	int res_code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
	curl_easy_cleanup(curl);

	if(res == CURLE_OK && res_code == 200)
	{
		HGLOBAL hBlock = GlobalAlloc(GHND, data.size());
		if(hBlock != NULL)
		{
			LPVOID pBlock = GlobalLock(hBlock);
			if(pBlock != NULL)
			{
				memmove(pBlock, data.data(), data.size());
				IStream* pStream;
				if(CreateStreamOnHGlobal(hBlock, FALSE, &pStream) == S_OK)
					mImage = new Gdiplus::Image(pStream);
				GlobalUnlock(pBlock);
			}
			GlobalFree(hBlock);
		}

		if(mImage)
		{
			std::lock_guard<std::mutex> lock(loaded_mutex);
			mLoaded = true;
		}
	}

	SignalReady.emit(this);
}

}