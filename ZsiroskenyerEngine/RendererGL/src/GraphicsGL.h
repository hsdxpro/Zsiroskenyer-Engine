// GraphicsGL.h By Zsíroskenyér team
#pragma once

#include "..\..\GraphicsEngine\src\IGraphicsApi.h"
#include "..\..\GraphicsEngine\src\IWindow.h"
#include "..\..\CommonLib\src\common.h"

class cGraphicsGL : public IGraphicsApi {
public:
	void Present();
	void SetWindow(IWindow *renderWindow);
protected:
};


// DLL accessor
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsGL() {
		return new cGraphicsGL();
}