#ifndef __MULTITRACKS_CONFIGMANAGER_H__
#define __MULTITRACKS_CONFIGMANAGER_H__

#include "DataFile.h"

namespace mt
{

class Config
{
public:
	CDataFile* operator->() { return &mConfig; }

	static Config g;

private:
	CDataFile mConfig;
};

}

#endif // !__MULTITRACKS_CONFIGMANAGER_H__