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
#include "../../GraphicsCommon/src/IShaderProgram.h"

#include "../../GraphicsCommon/src/Exception.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // töröld ki de gecigyorsan amint nem kell debughoz


// construction of the graphics engine
cGraphicsEngine::cGraphicsEngine() {
	gApi = Factory.CreateGraphics();
	if (!gApi)
		throw UnknownErrorException("failed to create graphics api");
	managerShader = new cManagerShader(gApi);
	managerResource = new cManagerResource(gApi);
	managerScene = new cManagerScene(*managerResource);

	IShaderProgram* sh = managerShader->LoadShader(L"../GraphicsEngine/shaders/",L"test");
	if (!sh) {
		MessageBoxA(NULL, "nincsen shader bazmeg", "F*ck", MB_OK|MB_ICONERROR);
	}
}


void cGraphicsEngine::RenderSceneForward() {
	// Set BackBuffer....
	gApi->SetRenderTargetDefault();

	// Set Effect...
	auto shader = managerShader->GetShaderByName(L"test.cg");
	

	// Render each instanceGroup
	auto instanceGroups = managerScene->GetInstanceGroups();

	for (auto& group : instanceGroups) {
		for (auto& entity : group->entities) {
			// draw entity
		}
	}
}

// interface
IManagerScene* cGraphicsEngine::GetSceneManager() {
	return managerScene;
}