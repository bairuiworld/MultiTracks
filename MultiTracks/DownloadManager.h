#ifndef __MULTITRACKS_DOWNLOADMANAGER_H__
#define __MULTITRACKS_DOWNLOADMANAGER_H__

#include "ThreadPool.h"

namespace mt
{

class DownloadManager
{
	DownloadManager() = delete;

public:
	static void Init() { mPool = new ThreadPool(6); }
	static void CleanUp() { delete mPool; }

	static ThreadPool* GetPool() { return mPool; }

private:
	static ThreadPool* mPool;
};

}

#endif // !__MULTITRACKS_DOWNLOADMANAGER_H__
