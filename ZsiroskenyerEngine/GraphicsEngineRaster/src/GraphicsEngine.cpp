////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsEngine.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "ShaderManager.h"

#include "Geometry.h"
#include "..\..\Core\src\GraphicsEntity.h"
#include "..\..\Core\src\Camera.h"

#include "..\..\Core\src\GAPI.h"

#include "..\..\Core\src\math/Matrix44.h"
#include "..\..\Core\src\common.h"

#include <string>
#include <stdexcept>
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
//	Constructor of the graphics engine
cGraphicsEngine::cGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config) 
	:
	screenWidth(screenWidth),
	screenHeight(screenHeight),
	deferredRenderer(NULL),
	hdrProcessor(NULL)
{
	// Create graphics api
	switch (config.rasterEngine.gxApi) {
		case tGraphicsConfig::eGraphicsApi::D3D11:
			gApi = CreateGraphicsApi(targetWindow, screenWidth, screenHeight, eGraphicsApiType::GRAPHICS_API_D3D11);
			break;
		case tGraphicsConfig::eGraphicsApi::OPENGL_43:
			gApi = CreateGraphicsApi(targetWindow, screenWidth, screenHeight, eGraphicsApiType::GRAPHICS_API_OPENGL43);
			break;
		default:
			gApi = NULL;
	}
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
		throw std::runtime_error(std::string("failed to create deferred renderer: ") + e.what());
	}

	// Create hdr post-processor
	try {
		hdrProcessor = new cHDRProcessor(*this);
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create hdr post-processor: ") + e.what());
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


////////////////////////////////////////////////////////////////////////////////
//	Utility & Settings

// reload all resourcess. TODO: not only shaders
eGraphicsResult cGraphicsEngine::ReloadResources() {
	if(!shaderManager->ReloadShader(L"shaders/test.cg"))
		return eGraphicsResult::ERROR_UNKNOWN;

	if (!shaderManager->ReloadShader(L"shaders/LINE_RENDERER.cg"))
		return eGraphicsResult::ERROR_UNKNOWN;

	deferredRenderer->ReloadShaders();

	return eGraphicsResult::OK;
}

// set global configuration
eGraphicsResult cGraphicsEngine::SetConfig(tGraphicsConfig config) {
	return eGraphicsResult::OK; // no config, no work
}

// resize screen
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

// create/delete scenes
IGraphicsScene* cGraphicsEngine::CreateScene(tRenderState state) {
	try {
		cGraphicsScene* newScene = new cGraphicsScene(*this, state);
		graphicsScenes.insert(newScene);
		graphicsSceneOrder.push_back(newScene);
		return newScene;
	}
	catch (std::exception&) {
		return NULL;
	}
}
void cGraphicsEngine::DeleteScene(const IGraphicsScene* scene) {
	auto it = graphicsScenes.find((cGraphicsScene*)scene);
	if (it != graphicsScenes.end()) {
		for (auto ito = graphicsSceneOrder.begin(); ito != graphicsSceneOrder.end(); ito++) {
			if (*ito == *it) {
				graphicsSceneOrder.erase(ito);
				break;
			}
		}
		delete *it;
		graphicsScenes.erase(it);
	}
}

// DEPRECATED
/*
void cGraphicsEngine::SetActiveCamera(cCamera* cam) {
	//sceneManager->SetActiveCamera(cam);
}
*/
// DEPRECATED
/*
cGraphicsEntity* cGraphicsEngine::CreateEntity(const zsString& geomPath, const zsString& mtlPath) {
	cGeometryRef geom = resourceManager->GetGeometry(geomPath);
	cMaterialRef mtl = resourceManager->GetMaterial(mtlPath);

	if (!geom || !mtl)
		return NULL;

	return sceneManager->AddEntity(std::move(geom), std::move(mtl));
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Update scene
eGraphicsResult cGraphicsEngine::Update(float elapsed) {
	if (elapsed < 1e-8) {
		elapsed = 1e-8;
	}
	gApi->Clear(true, true);

	/*
	//RenderSceneForward();
	RenderSceneDeferred();
	return eGraphicsResult::OK;
	*/
	for (auto& scene : graphicsSceneOrder) {
		RenderScene(*scene, elapsed);
	}
	return eGraphicsResult::OK;
}


// Render a graphics scene
void cGraphicsEngine::RenderScene(cGraphicsScene& scene, float elapsed) {
	// load settings from scene
	camera = &scene.camera;
	camera->SetAspectRatio((double)screenWidth / (double)screenHeight);
	sceneManager = &scene.sceneManager;
	this->elapsed = elapsed;

	// --- --- composition w/ deferred --- --- //
	ASSERT(deferredRenderer);
	deferredRenderer->RenderComposition();

	/*
	// --- --- post-process --- --- //
	static ITexture2D* compBuf_Check = NULL; // TODO: Remove this or I kill myself
	ITexture2D* composedBuffer = deferredRenderer->GetCompositionBuffer();

	// HDR
	if (composedBuffer != compBuf_Check) {
		compBuf_Check = composedBuffer;
		hdrProcessor->SetSource(composedBuffer, screenWidth, screenHeight);
	}
	hdrProcessor->adaptedLuminance = scene.luminanceAdaptation; // copy luminance value
	hdrProcessor->SetDestination(gApi->GetDefaultRenderTarget());	// set destination as backbuffer
	hdrProcessor->Update(elapsed);									// update hdr
	scene.luminanceAdaptation = hdrProcessor->adaptedLuminance; // copy luminance value

	gApi->GetDefaultRenderTarget();
	*/
}

////////////////////////////////////////////////////////////////////////////////
//	Internal private functionality
/*
void cGraphicsEngine::RenderForward() {

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
*/

/*
// Render scene in deferred mode w/ post-processing
void cGraphicsEngine::RenderDeferred() {
	ASSERT(deferredRenderer);

	// --- --- composition w/ deferred --- --- //
	deferredRenderer->RenderComposition();

	// --- --- post-process --- --- //
	static ITexture2D* compBuf_Check = NULL; // TODO: Remove this or i kill myself
	ITexture2D* composedBuffer = deferredRenderer->GetCompositionBuffer();

	// HDR
	if (composedBuffer != compBuf_Check) {
		compBuf_Check = composedBuffer;
		hdrProcessor->SetSource(composedBuffer, screenWidth, screenHeight);
	}
	hdrProcessor->SetDestination(gApi->GetDefaultRenderTarget());
	hdrProcessor->Update();

	gApi->GetDefaultRenderTarget();
}
*/

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