// Implementation
#include "Window.h"
#include <tchar.h>
#include "../../Core/src/zsString.h"

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

cWindow::cWindow(const IWindow::tDesc& winDesc)
:opened(true) {
	
	// Description holds OS independent style code, so interpret it to win32
	IWindow::eStyle interpretedStyle = (IWindow::eStyle)WS_OVERLAPPEDWINDOW;
	switch(winDesc.style) {
		case IWindow::eStyle::OVERLAPPED:
				interpretedStyle = (IWindow::eStyle)WS_OVERLAPPEDWINDOW;
				break;
	}
	
	// Description holds OS independent brush code, so interpret it to win32
	IWindow::eBrush interpretedBrush = (IWindow::eBrush)NULL_BRUSH;
	switch(winDesc.brush) {
		case IWindow::eBrush::RENDER_:
			interpretedBrush = (IWindow::eBrush)NULL_BRUSH;
			break;
	}
	
	// Application ID for window class registration, and window creation
	HINSTANCE appID = GetModuleHandle(NULL);

	// Register our new window class
	WNDCLASSEX wC;
	memset(&wC,0,sizeof(WNDCLASSEX));
	wC.cbSize = sizeof(WNDCLASSEX);
	wC.cbClsExtra = 0;
	wC.cbWndExtra = 0;
	wC.hbrBackground = (HBRUSH)GetStockObject((int)interpretedBrush);
	wC.hCursor = LoadCursor(NULL, IDC_ARROW);
	wC.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wC.hIconSm = NULL;
	wC.lpszClassName = _T("windowclass");
	wC.lpszMenuName = NULL;
	wC.hInstance = appID;
	wC.lpfnWndProc = wndProc;
	wC.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wC);

	RECT adjustedsize = {0};
	AdjustWindowRect(&adjustedsize, (int)interpretedStyle, 0);

	int wWidth  = winDesc.clientWidth  - adjustedsize.left + adjustedsize.right;
	int wHeight = winDesc.clientHeight - adjustedsize.top  + adjustedsize.bottom;
		
	handle = CreateWindow(
		_T("windowclass"),
		winDesc.captionName.c_str(),
		(int)interpretedStyle,
		0,
		0,
		wWidth,
		wHeight,
		GetDesktopWindow(),
		0,
		appID,
		0);
		
	ShowWindow(handle,SW_SHOW);
	UpdateWindow(handle);
}

void cWindow::MoveCenter() {
	size_t screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
	size_t screenCenterY = GetSystemMetrics(SM_CXSCREEN) / 2;
	RECT rect; GetWindowRect(handle,&rect);
	SetWindowPos(handle, 0, screenCenterX - rect.right/2, screenCenterY - rect.bottom/2, rect.right, rect.bottom, 0);
}

bool cWindow::HandleMessage(IWindow::eMessage* msg) {
	static MSG winMsg;
	// Get window message from OS
	if (PeekMessage(&winMsg, NULL, 0, 0, PM_REMOVE))
	{
		// Translate, dispatch, process message 
		TranslateMessage(&winMsg);
		DispatchMessage(&winMsg);
		*msg = ProcessMessage(winMsg);

		return *msg != IWindow::eMessage::QUIT;
	}
	return false;
}

IWindow::eMessage cWindow::ProcessMessage(const MSG& msg) {
	switch (msg.message) {
		case WM_QUIT:
		{
			// Close window
			Close();
			return IWindow::eMessage::QUIT;
		}
		case WM_DESTROY:
		{
			//@TODO TMP CODE Fuck... REMOVE THAT
			PostQuitMessage(0);
			return IWindow::eMessage::DESTROY;
		}
		case WM_SIZE:
				return IWindow::eMessage::SIZE_CHANGED;
		case WM_SYSKEYDOWN:
			// ALT + ENTER = SIZE_CHANGED
			if (LOWORD(msg.wParam) == VK_RETURN && ((msg.lParam >> 29 & 0x1) == 1))
				return IWindow::eMessage::SIZE_CHANGED;
	}
	return IWindow::eMessage(95934292); // TODO xD
}

void cWindow::Close() {
	opened = false;
}

void cWindow::SetCaptionText(const zsString& str) {
	SetWindowText(handle, str.c_str());
}

bool cWindow::IsOpened() const {
	return opened;
}

zsString cWindow::GetCaptionText() const {
	return L"asd";
}

Vec2 cWindow::GetCenter() const {
	return Vec2();
}

size_t cWindow::GetClientWidth() const {
	RECT rect; GetClientRect(handle, &rect);
	return rect.right - rect.left;
}

size_t cWindow::GetClientHeight() const {
	RECT rect; GetClientRect(handle,&rect);
	return rect.bottom - rect.top;
}

float cWindow::GetClientAspectRatio() const {
	RECT rect; GetClientRect(handle, &rect);
	return (float)(rect.right - rect.left) / (rect.bottom - rect.top);
}

IWindow::Handle cWindow::GetHandle() const {
	return (Handle)handle;
}
