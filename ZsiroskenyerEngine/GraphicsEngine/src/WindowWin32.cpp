// Implementation
#include "WindowWin32.h"

#include <windows.h>

cWindowWin32::cWindowWin32(const IWindow::tDesc& winDesc) {
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
		wC.hInstance = winDesc.appInstance;
		wC.lpfnWndProc = wndProc;
		wC.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClassEx(&wC);

		// check for full screen client sizes, need POPUP window for full screen
		eStyle resultingStyle = winDesc.style;
		if(ManagerSystem.isFullScreen(winDesc.clientWidth, winDesc.clientHeight)) {
			resultingStyle = eStyle::POPUP;
		}

		RECT adjustedsize = {0};
		AdjustWindowRect(&adjustedsize, resultingStyle,0);

		int wWidth = winDesc.clientWidth  - adjustedsize.left  + adjustedsize.right;
		int wHeight = winDesc.clientHeight - adjustedsize.top	+ adjustedsize.bottom;

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
	return zsString();
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