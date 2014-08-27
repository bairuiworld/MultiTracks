#include <Windows.h>
#include "Window.h"

namespace ww
{

const char* Window::ClassName = "wwWindow";

Window::Window(const char* title) : Register(ClassName), Widget(ClassName, title, WS_OVERLAPPEDWINDOW)
{
	Create(nullptr);
}

Window::~Window()
{
	DestroyWindow(mhWnd);
}

void Window::Show()
{
	ShowWindow(mhWnd, SW_SHOW);
}

void Window::Hide()
{
	ShowWindow(mhWnd, SW_HIDE);
}

void Window::OnClose()
{
	PostQuitMessage(0);
}

void Window::OnResize(int width, int height)
{

}

}