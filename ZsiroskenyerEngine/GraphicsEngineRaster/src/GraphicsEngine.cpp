////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsEngine.h"
#include "../../Common/src/Factory.h"

#include "ManagerScene.h"
#include "ManagerResource.h"
#include "ManagerShader.h"
#include "Geometry.h"
#include "../../GraphicsApi/src/IGraphicsApi.h"
#include "../../GraphicsApi/src/IShaderProgram.h"
#include "../../GraphicsEngine/src/Exception.h"
#include "../../GraphicsEngine/src/Camera.h"
#include "../../GraphicsApi/src/IConstantBuffer.h"
#include "../../GraphicsApi/src/IIndexBuffer.h"
#include "../../Common/src/math/Matrix44.h"

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
		ILog::GetInstance()->MsgBox(L"nincsen shader bazmeg");
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

	// Set Effect...
	IShaderProgram* shaderP = managerShader->GetShaderByName(L"test.cg");
	gApi->SetShaderProgram(shaderP);
	
	// Begin scene
	gApi->Clear(true, true);

	// Get camera params
	cCamera* cam = managerScene->GetActiveCamera();
	Matrix44 viewMat = cam->GetViewMatrix();
	Matrix44 projMat = cam->GetProjMatrix();
	
	// Render each instanceGroup
	for (auto& group : managerScene->GetInstanceGroups()) {
		// Set Geometry
		const IIndexBuffer* ib = group->geom->GetIndexBuffer();
		gApi->SetIndexData(ib);
		gApi->SetVertexData(group->geom->GetVertexBuffer(), shaderP->GetVertexFormatSize());

		// Set SubMaterials
		for(size_t i = 0; i < (*(group->mtl)).GetNSubMaterials(); i++) {
			gApi->SetTexture((*(group->mtl))[i].textureDiffuse, 0);
		}
		
		// Draw each entity
		for (auto& entity : group->entities) {

			// Entity world matrix
			Matrix44 world;
			world.RotationQuat(entity->rotation);
			world.Scale(entity->scale);
			world.Translate(entity->position);

			// WorldViewProj matrix
			Matrix44 wvp = world * viewMat * projMat;

			// Create and load Constant buffers WorldViewProj
			IConstantBuffer* wvpBuffer = NULL;
			wvpBuffer = gApi->CreateBufferConstant(sizeof(Matrix44), eBufferUsage::DEFAULT, &wvp);
			gApi->LoadConstantBuffer(wvpBuffer, 0);

			// Draw entity..
			gApi->DrawIndexed(ib->GetSize() / sizeof(unsigned));	

			// Free up constantBuffer
			wvpBuffer->Release();
		}
	}
}

IManagerScene* cGraphicsEngine::GetManagerScene() {
	return managerScene;
}

cManagerResource* cGraphicsEngine::GetManagerResource() {
	return managerResource;
}

IGraphicsApi* cGraphicsEngine::GetGraphicsApi() {
	return gApi;
}
