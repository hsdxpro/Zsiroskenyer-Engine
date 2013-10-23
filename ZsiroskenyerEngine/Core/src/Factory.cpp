// Factor.cpp implementation
#include "Factory.h"

cFactory Factory;

// DLL loader
#include "../../CommonOS/src/DLLLoader.h"

cFactory::cFactory() {

	IDLLLoader* dllLoader = IDLLLoader::Create();

	// Load DLL's and assign function PTR's to make Factory work after init
	IDLLLoader::DLLMODULE hGraphicsD3D11 = dllLoader->LoadDLL(L"GraphicsApiD3D11.dll");
	IDLLLoader::DLLMODULE hGraphicsGL = dllLoader->LoadDLL(L"GraphicsApiGL.dll");
	IDLLLoader::DLLMODULE hEngineGraphics = dllLoader->LoadDLL(L"GraphicsEngineRaster.dll");

	ptrCreateGraphicsD3D11 = (funcGraphicsApi)dllLoader->GetDLLFunction(hGraphicsD3D11, L"CreateGraphicsD3D11");
	ptrCreateGraphicsGL = (funcGraphicsApi)dllLoader->GetDLLFunction(hGraphicsGL, L"CreateGraphicsGL");
	ptrCreateEngineGraphics = (funcEngineGraphics)dllLoader->GetDLLFunction(hEngineGraphics, L"CreateGraphicsEngine");
}

IGraphicsApi* cFactory::CreateGraphics() {
	return ptrCreateGraphicsD3D11();
}

IGraphicsEngine* cFactory::CreateEngineGraphics() {
	return ptrCreateEngineGraphics();
}

