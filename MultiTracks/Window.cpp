#include <Windows.h>
#include "Window.h"

namespace ww
{

const char* Window::ClassName = "wwWindow";

Window::Window(const char* title) : Register(ClassName), Widget(ClassName, WS_OVERLAPPEDWINDOW)
{
	SetWindowText(mhWnd, title);
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

LRESULT CALLBACK Window::GlobalWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

}