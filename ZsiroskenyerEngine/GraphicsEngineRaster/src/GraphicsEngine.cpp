////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsEngine.h"
#include "../../Core/src/Factory.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "ShaderManager.h"

#include "Geometry.h"
#include "../../Core/src/Camera.h"

#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IShaderProgram.h"
#include "../../Core/src/IConstantBuffer.h"
#include "../../Core/src/IVertexBuffer.h"
#include "../../Core/src/IIndexBuffer.h"
#include "../../Core/src/Exception.h"
#include "../../Core/src/math/Matrix44.h"
#include "../../Core/src/common.h"

// construction of the graphics engine
cGraphicsEngine::cGraphicsEngine() {
	gApi = Factory.CreateGraphicsD3D11();
	if (!gApi)
		throw UnknownErrorException("failed to create graphics api");
	shaderManager = new cShaderManager(gApi);
	resourceManager = new cResourceManager(gApi);
	sceneManager = new cSceneManager(*resourceManager);


	shaderManager->LoadShader(L"shaders/",L"test.cg");
	shaderManager->LoadShader(L"shaders/",L"LINE_RENDERER.cg");
}

void cGraphicsEngine::Release() {
	SAFE_DELETE(sceneManager);
	SAFE_DELETE(resourceManager)
	SAFE_DELETE(shaderManager);
	SAFE_RELEASE(gApi);
}

void cGraphicsEngine::RenderSceneForward() {

	ZSASSERT(sceneManager->GetActiveCamera() != NULL);

	// Set BackBuffer
	gApi->SetRenderTargetDefault();

	// Set ShaderProgram
	IShaderProgram* shaderP = shaderManager->GetShaderByName(L"test.cg");
	gApi->SetShaderProgram(shaderP);
	
	// Get camera params
	cCamera* cam = sceneManager->GetActiveCamera();
	Matrix44 viewMat = cam->GetViewMatrix();
	Matrix44 projMat = cam->GetProjMatrix();
	
	// Render each instanceGroup
	for (auto& group : sceneManager->GetInstanceGroups()) {
		// Set Geometry
		const IIndexBuffer* ib = (*group->geom)->GetIndexBuffer();
		gApi->SetIndexBuffer(ib);
		gApi->SetVertexBuffer((*(group->geom))->GetVertexBuffer(), shaderP->GetVertexFormatSize());

		// Set SubMaterials
		for(size_t i = 0; i < (*group->mtl)->GetNSubMaterials(); i++) {
			gApi->SetTexture((*(*group->mtl))[i].textureDiffuse, 0);
		}
		
		// Draw each entity
		for (auto& entity : group->entities) {
			// Entity world matrix
			Matrix44 world = entity->GetWorldMatrix();

			// WorldViewProj matrix
			Matrix44 wvp = world * viewMat * projMat;

			// Create, load constant buffers, World and WorldViewProj
			IConstantBuffer* wvpBuffer = gApi->CreateBufferConstant(sizeof(Matrix44), eBufferUsage::DEFAULT, &wvp);
			IConstantBuffer* worldBuffer= gApi->CreateBufferConstant(sizeof(Matrix44), eBufferUsage::DEFAULT, &world);
				gApi->SetVSConstantBuffer(wvpBuffer, 0);
				gApi->SetVSConstantBuffer(worldBuffer, 1);
			
			// Draw entity..
			gApi->DrawIndexed(ib->GetSize() / sizeof(unsigned));

			// Free up constantBuffer
			wvpBuffer->Release();
			worldBuffer->Release();
		}
	}
}

void cGraphicsEngine::RenderLines(const Vec3* lines, size_t nLines, const Vec3& color /*= Vec3(1.0f, 1.0f, 1.0f)*/) {

	// Create, set VertexBuffer for lines
	IVertexBuffer* linesBuffer = gApi->CreateBufferVertex(nLines * 2, sizeof(Vec3), eBufferUsage::IMMUTABLE, (void*)lines);
	gApi->SetVertexBuffer(linesBuffer, sizeof(Vec3));

	// Set camera constants
	cCamera* cam = sceneManager->GetActiveCamera();
		Matrix44 viewProjMat = cam->GetViewMatrix() * cam->GetProjMatrix();

	IConstantBuffer* viewProjBuffer = gApi->CreateBufferConstant(sizeof(Matrix44), eBufferUsage::DEFAULT, &viewProjMat);
	gApi->SetVSConstantBuffer(viewProjBuffer, 0);

	IConstantBuffer* colorBuffer = gApi->CreateBufferConstant(sizeof(Vec3), eBufferUsage::DEFAULT, (void*)&color);
	gApi->SetPSConstantBuffer(colorBuffer, 0);

	// Set BackBuffer
	gApi->SetRenderTargetDefault();

	// Set Shader
	IShaderProgram* sh = shaderManager->GetShaderByName(L"LINE_RENDERER.cg");
	gApi->SetShaderProgram(sh);

	// Set Line primitives for pipeline
	gApi->SetPrimitiveTopology(ePrimitiveTopology::LINE_LIST);

	// Draw lines
	gApi->Draw(nLines * 2);

	// Free up buffers
	linesBuffer->Release();
	colorBuffer->Release();
	viewProjBuffer->Release();
}

ISceneManager* cGraphicsEngine::GetSceneManager()  {
	return sceneManager;
}

cResourceManager* cGraphicsEngine::GetResourceManager() {
	return resourceManager;
}

IGraphicsApi* cGraphicsEngine::GetGraphicsApi() {
	return gApi;
}