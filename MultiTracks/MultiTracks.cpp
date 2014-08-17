#include <thread>
#include "tinyxml2.h"
#include "Location.h"
#include "Section.h"
#include "Track.h"
#include "TopographicObjectContainer.h"
#include "MapSource.h"
#include "DownloadManager.h"
#include "Tile.h"
#include "Map.h"
#include "MapViewport.h"
#include "MapRenderer.h"
#include "Vector.h"
#include "SimpleSignal.h"
#include "Properties.h"

#include <curl/curl.h>
#include <fstream>

#include "Application.h"
#include "Window.h"
#include "Layout.h"

#ifdef _DEBUG
#	include <iostream>
#	include <windows.h>
#endif

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::ios::sync_with_stdio();
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif
	// Init
	mt::DownloadManager::Init();
	curl_global_init(CURL_GLOBAL_ALL);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//
	{ // Force Gdi+ object cleanup before shutting down
		/*mt::Tile tile(mt::Vector3i(5, 5, 5), &mt::MapSource::MAPS);
		tile.Download(true);
		tile.Wait();
		Gdiplus::Image* im = tile.GetImage();
		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);
		im->Save(L"a.png", &pngClsid, NULL);*/

		ww::Application app;
		ww::Window win;
		mt::Map map(&mt::MapSource::MAPS);
		map.GetViewport()->SetZoom(4);
		mt::MapRenderer* renderer = new mt::MapRenderer(&map);
		win.SetLayout(new ww::FillLayout);
		win.Add(renderer);

		win.SignalClose += []() { std::cout << "closing..." << std::endl; };

		app.Run();
	}

	// Cleanup
	Gdiplus::GdiplusShutdown(gdiplusToken);
	mt::DownloadManager::CleanUp();
	curl_global_cleanup();
	system("pause");
	return 0;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}