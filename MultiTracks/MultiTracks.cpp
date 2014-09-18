#include "DownloadManager.h"
#include "ViewManager.h"
#include "Application.h"
#include "ConfigManager.h"

#include <windows.h>
#include <CommCtrl.h>
#include <iostream>
#include <curl\curl.h>

#include <il\il.h>
#include <il\ilu.h>
#include <il\ilut.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
//#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::ios::sync_with_stdio();
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
//#endif
	// Init
	if(!mt::Config::g->Load("config.ini"))
	{
		std::cout << "File not found" << std::endl;
		mt::Config::g->SetFileName("config.ini");
		mt::Config::g->SetInt("version", 1, "", "MultiTracks");
		std::cout << mt::Config::g->GetValue("path", "Project", "shiiiit") << std::endl;
	}

	mt::DownloadManager::Init();
	curl_global_init(CURL_GLOBAL_ALL);
	ilInit();
	iluInit();
	ilutRenderer(ILUT_WIN32);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	InitCommonControls();

	//
	{ // Force Gdi+ object cleanup before shutting down
		ww::Application app(ww::PropertyGridSupport);
		mt::ViewManager view;
		app.Run();
	}

	// Cleanup
	Gdiplus::GdiplusShutdown(gdiplusToken);
	mt::DownloadManager::CleanUp();
	curl_global_cleanup();
#ifdef _DEBUG
	system("pause");
#endif
	return 0;
}