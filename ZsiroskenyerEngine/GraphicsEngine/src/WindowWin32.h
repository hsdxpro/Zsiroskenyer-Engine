// WindowWin32.h By Zsíroskenyér Team 2013.10.10 11:28
// Win32 implementation of the IWindow interface
#pragma once

#include "IWindow.h"

class cWindowWin32 : public IWindow {
public:
	void MoveCenter();
	void PeekAllMessages();
	void Close();
	void setCaptionText(const zsString& str);

	bool isOpened() const;
	bool isFullscreen() const;

	zsString getCaptionText() const;
	Vec2 getCenter() const;

	uint32 getClientWidth() const;
	uint32 getClientHeight() const;
	float getClientAspectRatio() const;
};