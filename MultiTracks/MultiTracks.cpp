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
#include <windows.h>

#include "Application.h"
#include "Window.h"
#include "Layout.h"

#include <il/il.h>
#include <il/ilu.h>
#include <il/ilut.h>

#ifdef _DEBUG
#	include <iostream>
#endif

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
	ilInit();
	iluInit();
	ilutRenderer(ILUT_WIN32);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//
	{ // Force Gdi+ object cleanup before shutting down
		/*mt::Tile t(mt::Vector3i(4, 4, 3), &mt::MapSource::IGN);
		t.Download(false);
		CLSID clsid;
		GetEncoderClsid(L"image/png", &clsid);
		Gdiplus::Image* im = t.GetImage();
		Gdiplus::Bitmap* bm = new Gdiplus::Bitmap(im->GetWidth(), im->GetHeight());
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(bm);
		g->DrawImage(im, 0, 0);
		delete g;
		bm->Save(L"c.png", &clsid, nullptr);
		return 0;*/
		ww::Application app;
		ww::Window win;
		mt::Map map(&mt::MapSource::IGN);
		map.GetViewport()->SetZoom(4);
		mt::WindowMapRenderer* renderer = new mt::WindowMapRenderer(&map);
		win.SetLayout(new ww::FillLayout);
		win.Add(renderer);

		win.SignalClose += []() { std::cout << "closing..." << std::endl; };

		app.Run();
		renderer->Save(L"a.png", mt::ImageFormat::png);

		/*int count = 0;

		mt::Map map(&mt::MapSource::IGN);
		mt::MapRenderer renderer(&map);
		mt::MapViewport* view = map.GetViewport();
		view->SetZoom(4);
		view->SetViewDimension(2000, 2000);
		map.SignalNewTile += [&count]() { count++; std::cout << count << std::endl; };
		renderer.Save(L"b.png", mt::ImageFormat::png);
		std::cout << "done" << std::endl;*/
	}

	// Cleanup
	Gdiplus::GdiplusShutdown(gdiplusToken);
	mt::DownloadManager::CleanUp();
	curl_global_cleanup();
	system("pause");
	return 0;
}

