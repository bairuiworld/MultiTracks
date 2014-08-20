#include <thread>
#include "tinyxml2.h"
#include "Location.h"
#include "Section.h"
#include "Track.h"
#include "Project.h"
#include "GPX.h"
#include "MapObjectContainer.h"
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
#include <CommCtrl.h>

#include "Application.h"
#include "Window.h"
#include "Layout.h"
#include "TreeView.h"

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
	InitCommonControls();

	//
	{ // Force Gdi+ object cleanup before shutting down
		ww::Application app;
		ww::Window win;
		mt::Map map(&mt::MapSource::IGN);
		map.GetViewport()->SetZoom(4);
		mt::WindowMapRenderer* renderer = new mt::WindowMapRenderer(&map);
		mt::Project p;
		p.LoadXML("projet.txt");
		renderer->AddComponent(&p.GetDatabase());
		win.SetLayout(new ww::FillLayout);
		//win.Add(renderer);

		/*TV_INSERTSTRUCT tviis;
		HTREEITEM hitem;

		InitCommonControls();

		HWND hTreeView = CreateWindowEx(0, WC_TREEVIEW, "",
								   WS_CHILD | WS_VISIBLE  | TVS_HASLINES | TVS_LINESATROOT
								   | TVS_HASBUTTONS | TVS_SHOWSELALWAYS
								   , 0, 0, 200, 200, win.GetHandle(), NULL, GetModuleHandle(0), NULL);

		//TreeView_SetImageList(hTreeView, himgList, TVSIL_NORMAL);

		tviis.hInsertAfter = TVI_LAST;
		ZeroMemory(&(tviis.item), sizeof(TV_ITEM));
		tviis.item.mask = TVIF_TEXT | 	TVIF_PARAM;
		tviis.hParent = TVI_ROOT;
		tviis.item.iImage = 0;
		tviis.item.iSelectedImage = 1;
		tviis.item.lParam = 1;
		tviis.item.pszText = "Option 1";
		hitem = TreeView_InsertItem(hTreeView, &tviis);

		tviis.hParent = hitem;
		tviis.item.iImage = 2;
		tviis.item.iSelectedImage = 2;
		tviis.item.lParam = 3;
		tviis.item.pszText = "Sous Option A";
		TreeView_InsertItem(hTreeView, &tviis);

		tviis.item.iImage = 3;
		tviis.item.iSelectedImage = 3;
		tviis.item.lParam = 4;
		tviis.item.pszText = "Sous Option B";
		TreeView_InsertItem(hTreeView, &tviis);

		tviis.hParent = TVI_ROOT;
		tviis.item.iImage = 0;
		tviis.item.iSelectedImage = 1;
		tviis.item.lParam = 2;
		tviis.item.pszText = "Option 2";
		hitem = TreeView_InsertItem(hTreeView, &tviis);

		tviis.hParent = hitem;
		tviis.item.iImage = 3;
		tviis.item.iSelectedImage = 3;
		tviis.item.lParam = 5;
		tviis.item.pszText = "Sous Option C";
		TreeView_InsertItem(hTreeView, &tviis);

		tviis.item.iSelectedImage = 2;
		tviis.item.iImage = 2;
		tviis.item.lParam = 6;
		tviis.item.pszText = "Sous Option D";
		TreeView_InsertItem(hTreeView, &tviis);*/

		ww::TreeView* tv = new ww::TreeView;
		tv->AddNode(new ww::TreeNode("test"));
		win.Add(tv);


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

