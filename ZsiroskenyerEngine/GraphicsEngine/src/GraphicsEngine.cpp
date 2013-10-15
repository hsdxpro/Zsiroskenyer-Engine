////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "../../CommonLib/src/Factory.h"

#include "ManagerScene.h"
#include "ManagerResource.h"
#include "ManagerShader.h"
#include "Geometry.h"
#include "../../GraphicsCommon/src/IGraphicsApi.h"
#include "../../GraphicsCommon/src/IShaderProgram.h"
#include "../../GraphicsCommon/src/Exception.h"
#include "../../GraphicsCommon/src/Camera.h"

#include "../../CommonLib/src/math/Matrix44.h"

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

	IShaderProgram* sh = managerShader->LoadShader(L"shaders/",L"test.cg");
	if (!sh) {
		MessageBoxA(NULL, "nincsen shader bazmeg", "F*ck", MB_OK|MB_ICONERROR);
	}
}


void cGraphicsEngine::RenderSceneForward() {
	// Set BackBuffer
	gApi->SetRenderTargetDefault();
	
	// Set Effect...
	auto shader = managerShader->GetShaderByName(L"test.cg");
	gApi->SetShaderProgram(shader);
	
	// Begin scene
	gApi->Clear();

	/*
	// FUCK YOU HARDCODED WVP
	IConstantBuffer* wvpBuffer = NULL;
	delete wvpBuffer;
	cCamera* cam = managerScene->GetActiveCamera();
	Matrix44 tmpObjTrans;
	tmpObjTrans.Translation(0.0f, 50.0f, 0.0f);
	Matrix44 wvp = tmpObjTrans * cam->GetViewMatrix() * cam->GetProjMatrix();
	wvp.Transpose();
	gApi->CreateConstantBuffer(sizeof(Matrix44), eBufferUsage::DYNAMIC, &wvp);

	gApi->LoadConstantBuffer(wvpBuffer, 0);

	// Render each instanceGroup
	auto instanceGroups = managerScene->GetInstanceGroups();

	for (auto& group : instanceGroups) {
		// set geometry and mtl
		const IVertexBuffer* vb = group->geom->GetVertexBuffer();
		const IIndexBuffer* ib = group->geom->GetIndexBuffer();
		gApi->SetIndexData(ib);
		gApi->SetVertexData(vb, shader->GetVertexFormatSize());

		size_t nIndices;
		for (auto& entity : group->entities) {
			// draw entity
			gApi->DrawIndexed(nIndices, 0);
		}
	}
	*/
}

// interface
IManagerScene* cGraphicsEngine::GetSceneManager() {
	return managerScene;
}