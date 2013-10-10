// IWindow.h By Zsíroskenyér team 2013.10.10 11:01
// interface for different O.S specific windows

#include "..\..\CommonLib\src\common.h"
#include "..\..\CommonLib\src\math\Point.h"
#include "..\..\CommonLib\src\math\Vec2.h"
#include "..\..\CommonLib\src\zsString.h"

class IWindow {
public:
	typedef int32* cAppInstance;

	enum eBrush {
		WHITE,
		LTGRAY,
		GRAY,
		DKGRAY,
		BLACK,
		RENDER_,
		HOLLOW,
	};

	enum eStyle {
		OVERLAPPED,
		POPUP,
	};

	enum eMessage {
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

		//@TODO unresolved external for destruction of zsString
		//zsString captionName;
		cAppInstance appInstance;
		uint32 clientWidth;
		uint32 clientHeight;
		IWindow::eBrush brush;
		IWindow::eStyle style;

		tDesc( cAppInstance appInstance = NULL, uint32 clientWidth = 0, uint32 clientHeight = 0, IWindow::eBrush brush = IWindow::eBrush::RENDER_, IWindow::eStyle style = IWindow::eStyle::OVERLAPPED)
			: appInstance(NULL), clientWidth(clientWidth), clientHeight(clientHeight), brush(brush), style(style) {
		}
	};

	virtual void MoveCenter() = 0;
	virtual void PeekAllMessages() = 0;
	virtual void Close() = 0;
	virtual void setCaptionText(const zsString& str) = 0;

	virtual bool isOpened() const = 0;
	virtual bool isFullscreen() const = 0;

	virtual zsString getCaptionText() const = 0;
	virtual Vec2 getCenter() const = 0;

	virtual uint32 getClientWidth() const = 0;
	virtual uint32 getClientHeight() const = 0;
	virtual float getClientAspectRatio() const = 0;
};