////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "../../CommonLib/src/Factory.h"

// construction of the graphics engine
cGraphicsEngine::cGraphicsEngine()
	:
	managerResource(),
	managerScene(managerResource)
{
	gApi = Factory.CreateGraphics();
}


void cGraphicsEngine::RenderSceneForward() {
	// Set BackBuffer....
	gApi->SetRenderTargetDefault();

	// Set Effect...


	// Render each instanceGroup
	auto instanceGroups = managerScene.GetInstanceGroups();
}

// interface
IManagerScene* cGraphicsEngine::GetSceneManager() {
	return (IManagerScene*)&managerScene;
}