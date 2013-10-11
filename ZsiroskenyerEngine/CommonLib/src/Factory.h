// Factory.h By Zsíroskenyér Team 2013.10.11 11:01
// Most important class for multiplatforming
#pragma once

#include "..\..\GraphicsEngine\src\IGraphicsApi.h"
#include "..\..\GraphicsEngine\src\IGraphicsEngine.h"
#include "..\..\GraphicsEngine\src\IWindow.h"
#include "IDLLLoader.h"

// Dll function pointers...
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
	funcEngineGraphics ptrCreateEngineGraphics;
	funcGraphicsApi  ptrCreateGraphicsD3D11;
	funcGraphicsApi  ptrCreateGraphicsGL;
	funcWindow		 ptrCreateWindowWin32;
};

extern cFactory Factory;