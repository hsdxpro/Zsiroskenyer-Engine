// IWindow.h By Zsíroskenyér team 2013.10.10 11:01
// interface for different O.S specific windows
#pragma once

#include "..\..\CommonLib\src\common.h"
#include "..\..\CommonLib\src\math\Point.h"
#include "..\..\CommonLib\src\math\Vec2.h"
#include "..\..\CommonLib\src\zsString.h"

class IWindow {
public:
	typedef int32* Handle;

	enum class eBrush {
		WHITE,
		LTGRAY,
		GRAY,
		DKGRAY,
		BLACK,
		RENDER_,
		HOLLOW,
	};

	enum class eStyle {
		OVERLAPPED,
		POPUP,
	};

	enum class eMessage {
		QUIT,
		DESTROY,
		CLOSE,
		KEY_DOWN,
		KEY_UP,
		WINDOW_SIZE_CHANGED,
		MOUSE_MOVE,
		MOUSE_LBUTTONDOWN,
		MOUSE_LBUTTONUP,
		MOUSE_LBUTTONDBCLK,
		MOUSE_RBUTTONDOWN,
		MOUSE_RBUTTONUP,
		MOUSE_RBUTTONDBCLK,
		MOUSE_MBUTTONDOWN,
		MOUSE_MBUTTONUP,
		MOUSE_MBUTTONDBCLK,
		MOUSE_WHEEL,
	};

	struct tDesc {
		static tDesc defaultRenderWindow;

		zsString captionName;
		uint32 clientWidth;
		uint32 clientHeight;
		IWindow::eBrush brush;
		IWindow::eStyle style;

		tDesc(const zsString& captionName = L"default_window_caption", uint32 clientWidth = 0, uint32 clientHeight = 0, IWindow::eBrush brush = IWindow::eBrush::RENDER_, IWindow::eStyle style = IWindow::eStyle::OVERLAPPED)
			:captionName(captionName), clientWidth(clientWidth), clientHeight(clientHeight), brush(brush), style(style) {
		}
	};

	virtual void MoveCenter() = 0;
	virtual void PeekAllMessages() = 0;
	virtual void Close() = 0;
	virtual void SetCaptionText(const zsString& str) = 0;

	virtual bool IsOpened() const = 0;
	virtual bool IsFullscreen() const = 0;

	virtual zsString GetCaptionText() const = 0;
	virtual Vec2 GetCenter() const = 0;
	virtual Handle GetHandle() const = 0;
	virtual uint32 GetClientWidth() const = 0;
	virtual uint32 GetClientHeight() const = 0;
	virtual float GetClientAspectRatio() const = 0;
};