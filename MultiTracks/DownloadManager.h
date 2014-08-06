#ifndef __MULTITRACKS_DOWNLOADMANAGER_H__
#define __MULTITRACKS_DOWNLOADMANAGER_H__

#include "ThreadPool.h"

namespace mt
{
namespace map
{

class DownloadManager
{
	DownloadManager() = delete;

public:
	static void Init() { mPool = new util::ThreadPool(6); }
	static void CleanUp() { delete mPool; }

	static util::ThreadPool* GetPool() { return mPool; }

private:
	static util::ThreadPool* mPool;
};

}
}

#endif // !__MULTITRACKS_DOWNLOADMANAGER_H__
