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
#include "../../GraphicsCommon/src/IConstantBuffer.h"
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

void cGraphicsEngine::Release() {
	delete managerScene;
	delete managerResource;
	delete managerShader;
	gApi->Release();
}

void cGraphicsEngine::RenderSceneForward() {
	
	// Set BackBuffer
	gApi->SetRenderTargetDefault();

	// Begin scene
	gApi->Clear(true, true);

	// Set Effect...
	auto shader = managerShader->GetShaderByName(L"test.cg");
	gApi->SetShaderProgram(shader);
	

	/*
	Vec3 vertices[4];
	vertices[0] = Vec3(-1, 1, 0.2f); // bal fent
	vertices[1] = Vec3(1, 1, 0.2f); // jobb fent
	vertices[2] = Vec3(1, -1, 0.2f); // jobb lent
	vertices[3] = Vec3(-1, -1, 0.2f); // bal lent

	int indices[6];
	indices[0] = 
		*/
	
	IConstantBuffer* wvpBuffer = NULL;
	cCamera* cam = managerScene->GetActiveCamera();
	Matrix44 tmpObjTrans;
	tmpObjTrans.Translation(0.0f, 0.0f, 0.0f);
	Matrix44 viewMat = cam->GetViewMatrix();
	Matrix44 projMat = cam->GetProjMatrix();
	Matrix44 wvp = tmpObjTrans * viewMat * projMat;
	wvp.Transpose();
	Matrix44 scale;
	scale.Scale(Vec3(1.0f/60, 1.0f/60, 1.0f/60));
	scale.Transpose();
	wvpBuffer = gApi->CreateConstantBuffer(sizeof(Matrix44), eBufferUsage::DEFAULT, &scale);
	
	gApi->LoadConstantBuffer(wvpBuffer, 0);
	
	
	// Render each instanceGroup
	auto instanceGroups = managerScene->GetInstanceGroups();
	for (auto& group : instanceGroups) {
		// set geometry and mtl
		const IVertexBuffer* vb = group->geom->GetVertexBuffer();
		const IIndexBuffer* ib = group->geom->GetIndexBuffer();
		
		gApi->SetIndexData(ib);
		size_t vertexStride = shader->GetVertexFormatSize();
		gApi->SetVertexData(vb, vertexStride);
		
		size_t nIndices = 36;
		for (auto& entity : group->entities) {
			// draw entity
			gApi->DrawIndexed(nIndices);
		}	
	}
	wvpBuffer->Release();
}

// interface
IManagerScene* cGraphicsEngine::GetSceneManager() {
	return managerScene;
}

IGraphicsApi* cGraphicsEngine::GetGraphicsApi() {
	return gApi;
}
