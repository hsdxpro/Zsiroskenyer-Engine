// WindowWin32.h By Zsíroskenyér Team 2013.10.10 11:28
// Win32 implementation of the IWindow interface
#pragma once

#include "../../Core/src/IWindow.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class cWindow : public IWindow {
public:
	cWindow(const IWindow::tDesc& winDesc);

	bool HandleMessage(IWindow::eMessage* msg) override;

	void MoveCenter() override;
	void Close() override;

	void SetCaptionText(const zsString& str) override;

	bool IsOpened()		const override;

	float			GetClientAspectRatio()	const override;
	size_t			GetClientHeight()		const override;
	size_t			GetClientWidth()		const override;
	zsString		GetCaptionText()		const override;
	Vec2			GetCenter()				const override;
	IWindow::Handle GetHandle()				const override;

protected:
	IWindow::eMessage ProcessMessage(const MSG& msg);

protected:
	HWND handle;
	bool opened;
};