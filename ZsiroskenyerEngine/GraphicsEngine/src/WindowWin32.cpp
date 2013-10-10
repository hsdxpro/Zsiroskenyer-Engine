// Implementation
#include "WindowWin32.h"
#include <tchar.h>
#include "..\..\CommonLib\src\zsString.h"

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
	case IWindow::eMessage::MOUSE_MBUTTONDOWN:
		{
			//ManagerInput.onMouseMiddleDown();
		} break;
	case IWindow::eMessage::MOUSE_MBUTTONUP:
		{
			//ManagerInput.onMouseMiddleUp();
		} break;
	case IWindow::eMessage::WINDOW_SIZE_CHANGED:
		{
			//ManagerOpenGL.setRenderWindow(hwnd);
		} break;
	case IWindow::eMessage::MOUSE_LBUTTONUP:
		{
			//ManagerInput.onMouseLeftUp();
		} break;
	case IWindow::eMessage::MOUSE_LBUTTONDOWN:
		{
			//ManagerInput.onMouseLeftDown();
		} break;
	case IWindow::eMessage::MOUSE_MOVE:
		{
			// These are window client relative coords
			//ManagerInput.onMouseMove(LOWORD(lParam), HIWORD(lParam));
		} break;
	case IWindow::eMessage::KEY_UP:
		{
			//ManagerInput.onKeyUp(wParam);
		} break;
	case IWindow::eMessage::KEY_DOWN:
		{
			//ManagerInput.onKeyDown(wParam);
		}break;
	case IWindow::eMessage::DESTROY:
		{
			//cWindow *window = ManagerWindow.getWindowByHandle(hwnd);

			// ASSERT the window doesn't created with ManagerWindow singleton, dont use simple cWindow constructor yourself
			//ASSERT( window != NULL);
			//window->close();
		} break;
	default:
		{

		}
		break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

cWindowWin32::cWindowWin32(const IWindow::tDesc& winDesc)
:opened(true) {
		// register our new window class
		WNDCLASSEX wC;
		memset(&wC,0,sizeof(WNDCLASSEX));
		wC.cbSize = sizeof(WNDCLASSEX);
		wC.cbClsExtra = 0;
		wC.cbWndExtra = 0;
		wC.hbrBackground = (HBRUSH)GetStockObject(winDesc.brush);
		wC.hCursor = LoadCursor(NULL, IDC_ARROW);
		wC.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wC.hIconSm = NULL;
		wC.lpszClassName = _T("windowclass");
		wC.lpszMenuName = NULL;
		wC.hInstance = (HINSTANCE)winDesc.appInstance;
		wC.lpfnWndProc = wndProc;
		wC.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClassEx(&wC);

		RECT adjustedsize = {0};
		AdjustWindowRect(&adjustedsize, winDesc.style, 0);

		int wWidth = winDesc.clientWidth  - adjustedsize.left  + adjustedsize.right;
		int wHeight = winDesc.clientHeight - adjustedsize.top	+ adjustedsize.bottom;
		/*
		handle = CreateWindow(	_T("windowclass"),
			winDesc.captionName.strConst(),
			resultingStyle,
			0,
			0,
			wWidth,
			wHeight,
			GetDesktopWindow(),
			0,
			winDesc.appInstance,
			0);
		*/
		ShowWindow(handle,SW_SHOW);
		UpdateWindow(handle);
}

void cWindowWin32::MoveCenter() {
	uint16 screenCenterX = GetSystemMetrics(SM_CXSCREEN)/2;
	uint16 screenCenterY = GetSystemMetrics(SM_CXSCREEN)/2;
	RECT rect; GetWindowRect(handle,&rect);
	SetWindowPos(handle, 0, screenCenterX - rect.right/2, screenCenterY - rect.bottom/2, rect.right, rect.bottom, 0);
}

void cWindowWin32::PeekAllMessages() {

}

void cWindowWin32::Close() {

}

void cWindowWin32::setCaptionText(const zsString& str) {

}

bool cWindowWin32::isOpened() const {
	return true;
}

bool cWindowWin32::isFullscreen() const {
	return false;
}

zsString cWindowWin32::getCaptionText() const {
	return zsString("bvazdmeg");
}

Vec2 cWindowWin32::getCenter() const {
	return Vec2();
}

uint32 cWindowWin32::getClientWidth() const {
	return 0;
}

uint32 cWindowWin32::getClientHeight() const {
	return 0;
}

float cWindowWin32::getClientAspectRatio() const {
	return 1.0f;
}