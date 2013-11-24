
#include <cassert>
#include "CreateGraphicsEngine.h"
#include "../../Core/src/DynamicLibrary.h"


static DllHandle hDllRaster = 0;
IGraphicsEngine*(*CreateGraphicsEngineRaster)(IWindow*, unsigned, unsigned, tGraphicsConfig);

IGraphicsEngine* CreateGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config) {
	if (hDllRaster) {
		if (CreateGraphicsEngineRaster)
			return CreateGraphicsEngineRaster(targetWindow, screenWidth, screenHeight, config);
		else
			return 0;
	}
	else {
		hDllRaster = LoadDynamicLibrary("GraphicsEngineRaster");
		if (!hDllRaster) {
			return NULL;
		}
		CreateGraphicsEngineRaster = (IGraphicsEngine*(*)(IWindow*, unsigned, unsigned, tGraphicsConfig))GetFunctionAddress(hDllRaster, "CreateGraphicsEngineRaster");
		if (!CreateGraphicsEngineRaster) {
			assert(false);
			return NULL;
		}
		return CreateGraphicsEngineRaster(targetWindow, screenWidth, screenHeight, config);
	}
}