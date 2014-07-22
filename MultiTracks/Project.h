#ifndef __MULTITRACKS_PROJECT_H__
#define __MULTITRACKS_PROJECT_H__

namespace mt
{
namespace track
{

class Project
{
private:
	std::string mFile;
	std::string mName;
	std::map<std::string, std::vector<Track>> mTracks;
};

}
}
#endif // !__MULTITRACKS_PROJECT_H__
