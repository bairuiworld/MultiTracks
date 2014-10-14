#ifndef __MULTITRACKS_CONFIGMANAGER_H__
#define __MULTITRACKS_CONFIGMANAGER_H__

#include "DataFile.h"

namespace mt
{

class Config
{
public:
	static bool Load(const char* file) { return g.mConfig.Load(file); }

	static int MultiTracksVersion() { return g.mConfig.GetInt("version", "MultiTracks", -1); }
	static std::string CurrentProjectPath() { return g.mConfig.GetValue("path", "Project", ""); }

	static float TrackEditLineWidth() { return g.mConfig.GetFloat("linewidth", "TrackEdit", 2); }
	static int TrackEditLineColor() { return g.mConfig.GetInt("linecolor", "TrackEdit", -65536); }
	
	static std::string ProxyAddress() { return g.mConfig.GetValue("address", "Proxy", ""); }
	static bool UseProxy() { return g.mConfig.GetBool("use", "Proxy", false); }

private:
	static Config g;
	CDataFile mConfig;
};

}

#endif // !__MULTITRACKS_CONFIGMANAGER_H__