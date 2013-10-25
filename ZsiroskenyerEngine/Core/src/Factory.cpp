// Factor.cpp implementation
#include "Factory.h"

cFactory Factory;

// DLL loader
#include "../../CommonOS/src/DLLLoader.h"

cFactory::cFactory() {

	IDLLLoader* dllLoader = IDLLLoader::Create();

	// Load DLL's and assign function PTR's to make Factory work after init
	IDLLLoader::DLLMODULE hGraphicsD3D11		= dllLoader->LoadDLL(L"GraphicsApiD3D11.dll");
	IDLLLoader::DLLMODULE hGraphicsGL			= dllLoader->LoadDLL(L"GraphicsApiGL.dll");
	IDLLLoader::DLLMODULE hGraphicsEngine		= dllLoader->LoadDLL(L"GraphicsEngineRaster.dll");
	IDLLLoader::DLLMODULE hPhysicsEngineBullet  = dllLoader->LoadDLL(L"PhysicsEngineBullet.dll");

	// Engine modules
	ptrCreateGraphicsD3D11 = (funcGraphicsApi)dllLoader->GetDLLFunction(hGraphicsD3D11, L"CreateGraphicsApiD3D11");
	ptrCreateGraphicsGL = (funcGraphicsApi)dllLoader->GetDLLFunction(hGraphicsGL, L"CreateGraphicsApiGL");
	ptrCreateGraphicsEngine = (funcGraphicsEngine)dllLoader->GetDLLFunction(hGraphicsEngine, L"CreateGraphicsEngineRaster");
	ptrCreatePhysicsEngineBullet = (funcPhysicsEngine)dllLoader->GetDLLFunction(hPhysicsEngineBullet, L"CreatePhysicsEngineBullet");
}

IGraphicsApi* cFactory::CreateGraphicsD3D11() {
	return ptrCreateGraphicsD3D11();
}

IGraphicsApi* cFactory::CreateGraphicsGL() {
	return ptrCreateGraphicsGL();
}

IGraphicsEngine* cFactory::CreateGraphicsEngine() {
	return ptrCreateGraphicsEngine();
}

IPhysicsEngine* cFactory::CreatePhysicsEngineBullet() {
	return ptrCreatePhysicsEngineBullet();
}
