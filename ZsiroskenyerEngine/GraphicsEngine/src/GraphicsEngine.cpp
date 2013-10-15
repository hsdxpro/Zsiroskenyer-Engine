////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "../../CommonLib/src/Factory.h"

#include "ManagerScene.h"
#include "ManagerResource.h"
#include "ManagerShader.h"
#include "../../GraphicsCommon/src/IGraphicsApi.h"

#include "../../GraphicsCommon/src/Exception.h"


// construction of the graphics engine
cGraphicsEngine::cGraphicsEngine() {
	gApi = Factory.CreateGraphics();
	if (!gApi)
		throw UnknownErrorException("failed to create graphics api");

}


void cGraphicsEngine::RenderSceneForward() {
	// Set BackBuffer....
	gApi->SetRenderTargetDefault();

	// Set Effect...


	// Render each instanceGroup
	auto instanceGroups = managerScene->GetInstanceGroups();
}

// interface
IManagerScene* cGraphicsEngine::GetSceneManager() {
	return (IManagerScene*)&managerScene;
}