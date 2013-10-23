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

#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IGraphicsEngine.h"

// Dll function pointers...
typedef IGraphicsApi* (*funcGraphicsApi)();
typedef IGraphicsEngine* (*funcEngineGraphics)();

class cFactory {
public:
	IGraphicsApi* CreateGraphics();
	IGraphicsEngine* CreateEngineGraphics();

	cFactory();

protected:
	funcEngineGraphics ptrCreateEngineGraphics;
	funcGraphicsApi  ptrCreateGraphicsD3D11;
	funcGraphicsApi  ptrCreateGraphicsGL;
};

extern cFactory Factory;