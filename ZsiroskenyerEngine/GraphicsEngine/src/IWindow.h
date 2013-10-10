// IWindow.h By Zsíroskenyér team 2013.10.10 11:01
// interface for different O.S specific windows

#include "..\..\CommonLib\src\common.h"
#include "..\..\CommonLib\src\zsString.h"

interface IWindow {
public:
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
		OVERLAPPED = WS_OVERLAPPEDWINDOW,
		POPUP = WS_POPUPWINDOW,
	};

	enum eMessage {
		QUIT	= WM_QUIT,
		DESTROY = WM_DESTROY,
		CLOSE	= WM_CLOSE,
		KEY_DOWN = WM_KEYDOWN,
		KEY_UP = WM_KEYUP,
		WINDOW_SIZE_CHANGED = WM_SIZE,
		MOUSE_MOVE = WM_MOUSEMOVE,
		MOUSE_LBUTTONDOWN = WM_LBUTTONDOWN,
		MOUSE_LBUTTONUP = WM_LBUTTONUP,
		MOUSE_LBUTTONDBCLK = WM_LBUTTONDBLCLK,
		MOUSE_RBUTTONDOWN = WM_RBUTTONDOWN,
		MOUSE_RBUTTONUP = WM_RBUTTONUP,
		MOUSE_RBUTTONDBCLK = WM_RBUTTONDBLCLK,
		MOUSE_MBUTTONDOWN = WM_MBUTTONDOWN,
		MOUSE_MBUTTONUP = WM_MBUTTONUP,
		MOUSE_MBUTTONDBCLK = WM_MBUTTONDBLCLK,
		MOUSE_WHEEL = WM_MOUSEWHEEL,
	};

	struct tDesc {
		static tDesc defaultRenderWindow;

		zsString captionName;
		HINSTANCE appInstance;
		uint32 clientWidth;
		uint32 clientHeight;
		IWindow::eBrush brush;
		IWindow::eStyle style;

		tDesc(const zsString& captionName = _T("default_window_name"), HINSTANCE appInstance = NULL, uint32 clientWidth = 0, uint32 clientHeight = 0, cWindow::eBrush brush = cWindow::eBrush::RENDER_, cWindow::eStyle style = cWindow::eStyle::OVERLAPPED)
			:captionName(captionName), appInstance(appInstance = NULL), clientWidth(clientWidth), clientHeight(clientHeight), brush(brush), style(style) {
		}
	};

	// process all of the messages that comes from the system
	void moveCenter();
	void peekAllMessages();
	void close();

	void releaseDC(const HDC& dc);

	static void moveToBottomRight(const HWND& windowHandle);

	void setCaptionText(const zsString& str);

	bool isOpened() const;
	bool isFullscreen() const;

	eiString getCaptionText();
	eiFloat2 getCenter() const;

	uint32 getClientWidth() const;
	uint32 getClientHeight() const;
	float getClientAspectRatio() const;;

	const HWND& getHandle() const;
	const HDC getDC() const;

	static Point getSize(const HWND& windowHandle);
	static uint32 getClientWidth(const HWND& windowHandle);
	static uint32 getClientHeight(const HWND& windowHandle);
	static const HDC getDC(const HWND& windowHandle);

protected:
	cWindow(const cWindow::tDesc& winDesc);
	~cWindow();
};