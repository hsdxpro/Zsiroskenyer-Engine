// WindowWin32.h By Zsíroskenyér Team 2013.10.10 11:28
// Win32 implementation of the IWindow interface
#pragma once

#include "IWindow.h"
#include <windows.h>

class cWindowWin32 : public IWindow {
public:
	cWindowWin32(const IWindow::tDesc& winDesc);

	void MoveCenter();
	void PeekAllMessages();
	void Close();
	void SetCaptionText(const zsString& str);

	bool IsOpened() const;
	bool IsFullscreen() const;

	zsString GetCaptionText() const;
	Vec2 GetCenter() const;
	IWindow::Handle GetHandle() const;
	uint32 GetClientWidth() const;
	uint32 GetClientHeight() const;
	float GetClientAspectRatio() const;

protected:
	bool opened;
	HWND handle;
};

// DLL accessor
extern "C"
	__declspec(dllexport)
	IWindow* CreateWindowWin32(const IWindow::tDesc& winDesc) {
		return new cWindowWin32(winDesc);
}