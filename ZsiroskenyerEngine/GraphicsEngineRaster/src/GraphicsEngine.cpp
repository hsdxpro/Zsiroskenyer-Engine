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
//	Static members

// in short: depth is fully enabled, object further are overdrawn
const tDepthStencilDesc cGraphicsEngine::depthStencilDefault = [](){
	tDepthStencilDesc depthStencilDefault;
	depthStencilDefault.depthCompare = eComparisonFunc::LESS_EQUAL;
	depthStencilDefault.depthEnable = true;
	depthStencilDefault.depthWriteEnable = true;
	depthStencilDefault.stencilEnable = false;
	return depthStencilDefault;
}();
// in short: no blending, just overwrite
const tBlendDesc cGraphicsEngine::blendDefault = [](){
	tBlendDesc blendDefault;
	blendDefault.alphaToCoverageEnable = false;
	blendDefault.independentBlendEnable = false;
	blendDefault[0].enable = false;
	return blendDefault;
}();


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



////////////////////////////////////////////////////////////////////////////////
//	Update scene
eGraphicsResult cGraphicsEngine::Update(float elapsed) {
	if (elapsed < 1e-8f) {
		elapsed = 1e-8f;
	}
	//gApi->Clear(true, true);

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

	
	// --- --- post-process --- --- //
	static ITexture2D* compBuf_Check = NULL; // TODO: Remove this or I kill myself
	ITexture2D* composedBuffer = deferredRenderer->GetCompositionBuffer();

	// HDR
	if (scene.state.hdr.enabled) {
		if (composedBuffer != compBuf_Check) {
			compBuf_Check = composedBuffer;
			hdrProcessor->SetSource(composedBuffer, screenWidth, screenHeight);
		}
		hdrProcessor->adaptedLuminance = scene.luminanceAdaptation; // copy luminance value
		hdrProcessor->SetDestination(gApi->GetDefaultRenderTarget());	// set destination as backbuffer
		hdrProcessor->Update(elapsed);									// update hdr
		scene.luminanceAdaptation = hdrProcessor->adaptedLuminance; // copy luminance value
	}
	else {
		gApi->SetRenderTargetDefault();
		gApi->SetShaderProgram(screenCopyShader);
		//gApi->SetTexture(L"texture0", composedBuffer);
		gApi->SetTexture(composedBuffer, 0);
		gApi->Draw(3);
	}
}

//	Get sub-components where allowed
cResourceManager* cGraphicsEngine::GetResourceManager() {
	return resourceManager;
}

IGraphicsApi* cGraphicsEngine::GetGraphicsApi() {
	return gApi;
}

IShaderManager* cGraphicsEngine::GetShaderManager() {
	return shaderManager;
}