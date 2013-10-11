// Factor.cpp implementation
#include "Factory.h"

cFactory Factory;

// Win32 DLL loader
#include "DLLLoaderWin32.h"

cFactory::cFactory() 
:dllLoader(NULL) {
	// Factory starts, init DLL loader
#if ZS_WIN32
	//dllLoader = CreateDLLLoaderWin32();
#endif
	dllLoader = new cDLLLoaderWin32();

	// Load DLL's and assign function PTR's to make Factory work after init
	IDLLLoader::DLLMODULE hGraphicsD3D11 = dllLoader->LoadDLL("GraphicsD3D11.dll");
	IDLLLoader::DLLMODULE hGraphicsGL= dllLoader->LoadDLL("GraphicsGL.dll");
	IDLLLoader::DLLMODULE hEngineGraphics= dllLoader->LoadDLL("GraphicsEngine.dll");

	ptrCreateGraphicsD3D11 = (funcGraphicsApi)dllLoader->GetDLLFunction(hGraphicsD3D11, "CreateGraphicsD3D11");
	ptrCreateGraphicsGL = (funcGraphicsApi)dllLoader->GetDLLFunction(hGraphicsGL, "CreateGraphicsGL");
	ptrCreateWindowWin32 = (funcWindow)dllLoader->GetDLLFunction(hEngineGraphics, "CreateWindowWin32");
	ptrCreateEngineGraphics = (funcEngineGraphics)dllLoader->GetDLLFunction(hEngineGraphics, "CreateGraphicsEngine");
}

IGraphicsApi* cFactory::CreateGraphics() {
#if ZS_WIN32
	return ptrCreateGraphicsD3D11();
#else
	return ptrCreateGraphicsGL();
#endif
	return NULL;
}

IWindow* cFactory::CreateWindow(const IWindow::tDesc& winDesc) {
#if ZS_WIN32
	return ptrCreateWindowWin32(winDesc);
#else
#endif
	return NULL;
}

IGraphicsEngine* cFactory::CreateEngineGraphics() {
	return ptrCreateEngineGraphics();
}