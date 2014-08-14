#include "Window.h"
#include "Register.h"

namespace ww
{

Register::Register(const char* className)
{
	WNDCLASSEX wcex;

	if(!GetClassInfoEx(GetModuleHandle(nullptr), className, &wcex))
	{
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = 0;
		wcex.lpfnWndProc = Widget::GlobalWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandle(nullptr);
		wcex.hIcon = NULL;
		wcex.hCursor = NULL;
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = className;
		wcex.hIconSm = NULL;

		RegisterClassEx(&wcex);
	}
}

}