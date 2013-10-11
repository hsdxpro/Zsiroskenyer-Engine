// GraphicsGL.h By Zsíroskenyér team
#pragma once

#include "..\..\GraphicsCore\src\IGraphicsApi.h"
#include "..\..\GraphicsCore\src\IWindow.h"
#include "..\..\CommonLib\src\common.h"

class cGraphicsGL : public IGraphicsApi {
public:
	void BBClear(bool clearOnlyDepth = false) override;
	void BBPresent() override;
	void SetWindow(IWindow *renderWindow) override;
protected:
};


// DLL accessor
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsGL() {
		return new cGraphicsGL();
}