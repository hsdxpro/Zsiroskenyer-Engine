// Factor.cpp implementation
#include "Factory.h"

cFactory Factory;

// Win32 DLL loader
#include "DLLLoaderWin32.h"

// Include headers based on O.S.
#ifdef WIN32
#include "..\..\GraphicsEngine\src\WindowWin32.h"
#undef CreateWindow
#define WINDOW_IMPLEMENTATION cWindowWin32
#else
#include "..\..\GraphicsEngine\src\WindowWin32.h"
#undef CreateWindow
#define WINDOW_IMPLEMENTATION cWindowWin32
#endif

cFactory::cFactory() {
	cDLLLoaderWin32 loader;

	// Load DLL's and assign function PTR's to make Factory work after init
	IDLLLoader::DLLMODULE hGraphicsD3D11 = loader.LoadDLL(L"GraphicsD3D11.dll");
	IDLLLoader::DLLMODULE hGraphicsGL= loader.LoadDLL(L"GraphicsGL.dll");
	IDLLLoader::DLLMODULE hEngineGraphics= loader.LoadDLL(L"GraphicsEngine.dll");

	ptrCreateGraphicsD3D11 = (funcGraphicsApi)loader.GetDLLFunction(hGraphicsD3D11, L"CreateGraphicsD3D11");
	ptrCreateGraphicsGL = (funcGraphicsApi)loader.GetDLLFunction(hGraphicsGL, L"CreateGraphicsGL");
	ptrCreateEngineGraphics = (funcEngineGraphics)loader.GetDLLFunction(hEngineGraphics, L"CreateGraphicsEngine");
}

IGraphicsApi* cFactory::CreateGraphics() {
	return ptrCreateGraphicsGL();
}

IWindow* cFactory::CreateWindow(const IWindow::tDesc& winDesc) {
	return new WINDOW_IMPLEMENTATION(winDesc);
}


IGraphicsEngine* cFactory::CreateEngineGraphics() {
	return ptrCreateEngineGraphics();
}
