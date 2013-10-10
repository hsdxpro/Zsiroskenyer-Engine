// IWindow.h By Zsíroskenyér team 2013.10.10 11:01
// interface for different O.S specific windows

#include "..\..\CommonLib\src\common.h"
#include "..\..\CommonLib\src\math\Point.h"
#include "..\..\CommonLib\src\math\Vec2.h"
#include "..\..\CommonLib\src\zsString.h"

class IWindow {
public:
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