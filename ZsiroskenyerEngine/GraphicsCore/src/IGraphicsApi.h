// IGraphicsApi.h By Zsíroskenyér Team 2013.10.10 1:37
// interface for different 3D graphics libraries like DirectX11 or OpenGL ..
#pragma once

#include "IWindow.h"

class IGraphicsApi {
public:
	virtual void BBClear(bool clearOnlyDepth = false) = 0;
	virtual void BBPresent() = 0;
	virtual void SetWindow(IWindow *renderWindow) = 0;
};