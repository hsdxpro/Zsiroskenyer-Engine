////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "../../CommonLib/src/Factory.h"

// construction of the graphics engine
cGraphicsEngine::cGraphicsEngine()
	:
	mgrResource(),
	mgrScene(mgrResource)
{

	mgrGApi = Factory.CreateGraphics();
	mgrShader = new cManagerShader(mgrGApi);


	mgrShader->LoadShader(L"shaders/", L"test.cg");
}


void cGraphicsEngine::RenderSceneForward() {
	// Set BackBuffer....
	mgrGApi->SetRenderTargetDefault();

	// Set Effect...


	// Render each instanceGroup
	auto instanceGroups = mgrScene.GetInstanceGroups();
}

// interface
IManagerScene* cGraphicsEngine::GetSceneManager() {
	return (IManagerScene*)&mgrScene;
}