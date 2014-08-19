#ifndef __MULTITRACKS_GPX_H__
#define __MULTITRACKS_GPX_H__

namespace mt
{

class Track;

class GPX
{
public:
	static Track* Load(const char* filename);
};

}

#endif // !__MULTITRACKS_GPX_H__