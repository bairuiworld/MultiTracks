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

#ifdef _DEBUG
#	include <iostream>
#endif



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
/*		ww::Application app;
		ww::Window win;
		mt::Map map(&mt::MapSource::MAPS);
		map.GetViewport()->SetZoom(4);
		mt::WindowMapRenderer* renderer = new mt::WindowMapRenderer(&map);
		win.SetLayout(new ww::FillLayout);
		win.Add(renderer);

		win.SignalClose += []() { std::cout << "closing..." << std::endl; };

		app.Run();
		renderer->Save(L"a.png", mt::ImageFormat::png);*/

		int count = 0;

		mt::Map map(&mt::MapSource::IGN);
		mt::MapRenderer renderer(&map);
		mt::MapViewport* view = map.GetViewport();
		view->SetZoom(4);
		view->SetViewDimension(2000, 2000);
		map.SignalNewTile += [&count]() { count++; std::cout << count << std::endl; };
		renderer.Save(L"b.png", mt::ImageFormat::png);
		std::cout << "done" << std::endl;
	}

	// Cleanup
	Gdiplus::GdiplusShutdown(gdiplusToken);
	mt::DownloadManager::CleanUp();
	curl_global_cleanup();
	system("pause");
	return 0;
}

