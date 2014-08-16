#include <Windows.h>
#include "Window.h"
#include "Application.h"

namespace ww
{

Application::Application()
{

}

int Application::Run()
{
	MSG msg;
	while(GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

}