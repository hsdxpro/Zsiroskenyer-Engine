////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsEngine.h"
#include "../../Core/src/Factory.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "ShaderManager.h"

#include "Geometry.h"
#include "..\..\Core\src\GraphicsEntity.h"
#include "..\..\Core\src\Camera.h"

#include "..\..\Core\src\IGraphicsApi.h"
#include "..\..\Core\src\IShaderProgram.h"
#include "..\..\Core\src\IConstantBuffer.h"
#include "..\..\Core\src\IVertexBuffer.h"
#include "..\..\Core\src\IIndexBuffer.h"

#include "..\..\Core\src\Exception.h"
#include "..\..\Core\src\math/Matrix44.h"
#include "..\..\Core\src\common.h"

#include <string>
#include <stdexcept>
#include <iostream>


// DLL pure C interface
extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config) {
	cGraphicsEngine* engine = NULL;
	try {
		engine = new cGraphicsEngine(targetWindow, screenWidth, screenHeight, config);
	}
	catch (std::exception& e) {
		std::cerr << "[fatal] graphics engine creation failed with message: " << e.what() << std::endl;
		delete engine;
	}
	return engine;
}


//	Construction of the graphics engine
cGraphicsEngine::cGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config) {
	// Create graphics api
	gApi = Factory.CreateGraphicsD3D11();
	if (!gApi)
		throw std::runtime_error("failed to create graphics api");
	shaderManager = new cShaderManager(gApi);
	resourceManager = new cResourceManager(gApi);
	sceneManager = new cSceneManager();

	// Create shader
		// Basic 3D geom rendering
	shaderManager->LoadShader(L"shaders/test.cg");
		// For debugging
	shaderManager->LoadShader(L"shaders/LINE_RENDERER.cg");

	// Create deferred renderer
	try {
		deferredRenderer = new cDeferredRenderer(*this);
	}
	catch (std::exception& e) {
		std::cerr << "[non-fatal error (yet)] deferred renderer failed with message: " << e.what() << std::endl;
		delete deferredRenderer;
	}

	// Create hdr post-processor
	try {
		hdrProcessor = new cHDRProcessor(*this);
	}
	catch (std::exception& e) {
		std::cerr << "[non-fatal error (yet)] HDR post-processor failed with message. " << e.what() << std::endl;
	}
}
cGraphicsEngine::~cGraphicsEngine() {
	SAFE_DELETE(sceneManager);
	SAFE_DELETE(resourceManager)
	SAFE_DELETE(shaderManager);
	SAFE_RELEASE(gApi);
	SAFE_DELETE(deferredRenderer);
}

void cGraphicsEngine::Release() {
	delete this;
}


//	Utility & Settings
//	TODO: Reload all resources not only shaders
eGraphicsResult cGraphicsEngine::ReloadResources() {
	if(!shaderManager->ReloadShader(L"shaders/test.cg"))
		return eGraphicsResult::ERROR_UNKNOWN;

	if (!shaderManager->ReloadShader(L"shaders/LINE_RENDERER.cg"))
		return eGraphicsResult::ERROR_UNKNOWN;

	return eGraphicsResult::OK;
}

void cGraphicsEngine::SetActiveCamera(cCamera* cam) {
	sceneManager->SetActiveCamera(cam);
}

eGraphicsResult cGraphicsEngine::SetConfig(tGraphicsConfig config) {
	return eGraphicsResult::OK; // no config, no work
}

eGraphicsResult cGraphicsEngine::Resize(unsigned width, unsigned height) {
	eGraphicsResult result = eGraphicsResult::OK;

	screenWidth = width;
	screenHeight = height;

	gApi->SetBackBufferSize(screenWidth, screenHeight);
	if (deferredRenderer) {
		result = deferredRenderer->Resize(screenWidth, screenHeight);
	}
	return result;
}

cGraphicsEntity* cGraphicsEngine::CreateEntity(const zsString& geomPath, const zsString& mtlPath) {
	cGeometryRef geom = resourceManager->GetGeometry(geomPath);
	cMaterialRef mtl = resourceManager->GetMaterial(mtlPath);

	if (!geom || !mtl)
		return NULL;

	return sceneManager->AddEntity(std::move(geom), std::move(mtl));
}



//	Rendering
eGraphicsResult cGraphicsEngine::Update() {
	//RenderSceneForward();
	RenderSceneDeferred();
	return eGraphicsResult::OK;
}


//	Internal private functionality
void cGraphicsEngine::RenderSceneForward() {

	ASSERT(sceneManager->GetActiveCamera() != NULL);

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
		const IIndexBuffer* ib = (*group->geom).GetIndexBuffer();
		gApi->SetIndexBuffer(ib);
		gApi->SetVertexBuffer((*(group->geom)).GetVertexBuffer(), shaderP->GetVertexFormatSize());

		// Set SubMaterials
		auto& mtl = *group->mtl;
		for(size_t i = 0; i < mtl.GetNSubMaterials(); i++) {
			ITexture2D* diffuse = mtl[i].textureDiffuse.get();
			ITexture2D* normal = mtl[i].textureNormal.get();
			ITexture2D* specular = mtl[i].textureSpecular.get();
			ITexture2D* displace = mtl[i].textureDisplace.get();

			if (diffuse != NULL)
				gApi->SetTexture(diffuse, 0); // TODO NEVET KELLJEN BEÍRNI NE INDEXET WAWOOSOSOSO
			if (normal != NULL)
				gApi->SetTexture(normal, 1); // TODO NEVET KELLJEN BEÍRNI NE INDEXET, ÜDV :)
			if (specular != NULL)
				gApi->SetTexture(specular, 2); // TODO NEVET KELLJEN BEÍRNI NE INDEXET
			if (displace != NULL)
				gApi->SetTexture(displace, 3); // TODO NEVET KELLJEN BEÍRNI NE INDEXET
		}
		
		// Draw each entity
		for (auto& entity : group->entities) {

			// Entity world matrix
			Matrix44 world = entity->GetWorldMatrix();

			// WorldViewProj matrix
			Matrix44 wvp = world * viewMat * projMat;

			struct myBuffer
			{
				Matrix44 wvp;
				Matrix44 world;
				Vec3 camPos;

			} buff;
			buff.wvp = wvp;
			buff.world = world;
			buff.camPos = cam->GetPos();

			IConstantBuffer* buffer;
			gApi->CreateConstantBuffer(&buffer, sizeof(buff), eUsage::DEFAULT, &buff);
			gApi->SetVSConstantBuffer(buffer, 0);

			// Draw entity..
			gApi->DrawIndexed(ib->GetSize() / sizeof(unsigned));

			// Free up constantBuffer
			buffer->Release();
		}
	}
}


// Render scene in deferred mode w/ post-processing
void cGraphicsEngine::RenderSceneDeferred() {
	ASSERT(deferredRenderer);

	// render the scene to the composition buffer w/ deferredRenderer
	deferredRenderer->RenderComposition();
	ITexture2D* composedBuffer = deferredRenderer->GetCompositionBuffer();

	// post-processing will be run here
	/* post-process */

	// for now post-process & further rendering equals to copying the composed texture to BB
	gApi->SetRenderTargetDefault();
	IShaderProgram* screenCopyShader = shaderManager->GetShaderByName(L"screen_copy.cg");
	gApi->SetShaderProgram(screenCopyShader);
	gApi->SetTexture(composedBuffer, 0);
	gApi->Draw(3);
}


//	Get sub-components where allowed
cSceneManager* cGraphicsEngine::GetSceneManager()  {
	return sceneManager;
}

cResourceManager* cGraphicsEngine::GetResourceManager() {
	return resourceManager;
}

IGraphicsApi* cGraphicsEngine::GetGraphicsApi() {
	return gApi;
}

IShaderManager* cGraphicsEngine::GetShaderManager() {
	return shaderManager;
}

cCamera* cGraphicsEngine::GetActiveCamera() {
	return sceneManager->GetActiveCamera();
}