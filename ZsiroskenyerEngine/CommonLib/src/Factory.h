// Factory.h By Zsíroskenyér Team 2013.10.11 11:01
// Most important class for multiplatforming
// This class create class instances based on the following defines for platforming
// ZS_IOS
// ZS_WIN32
// ZS_LINUX
// ZS_XBOX
// ZS_PSP
// ZS_ANDROID
// ZS_PS
// etc
#pragma once

#include "..\..\GraphicsCore\src\IGraphicsApi.h"
#include "..\..\GraphicsEngine\src\IGraphicsEngine.h"
#include "..\..\GraphicsCore\src\IWindow.h"
#include "IDLLLoader.h"

// Dll function pointers...
typedef IDLLLoader* (*funcDLLLoader)();
typedef IGraphicsApi* (*funcGraphicsApi)();
typedef IGraphicsEngine* (*funcEngineGraphics)();
typedef IWindow* (*funcWindow)(const IWindow::tDesc& winDesc);

class cFactory {
public:
	IGraphicsApi* CreateGraphics();
	IWindow* CreateWindow(const IWindow::tDesc& winDesc);
	IGraphicsEngine* CreateEngineGraphics();

	cFactory();

protected:
	IDLLLoader* dllLoader;
	funcEngineGraphics ptrCreateEngineGraphics;
	funcGraphicsApi  ptrCreateGraphicsD3D11;
	funcGraphicsApi  ptrCreateGraphicsGL;
	funcWindow		 ptrCreateWindowWin32;
};

extern cFactory Factory;