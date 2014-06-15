////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsEngine.h"

#include "SceneManager.h"
#include "ResourceManager.h"

#include "Geometry.h"
#include "UserGeometry.h"

#include "GraphicsEntity.h"
#include "..\..\Core\src\Camera.h"

#include "..\..\Core\src\GAPI.h"

#include "..\..\Core\src\math/Matrix44.h"
#include "..\..\Core\src\common.h"
#include "..\..\Core\src\StrUtil.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>

// FUCK YOU SHADER RELOAD FAILED
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////
// DLL pure C interface
extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config, const char** errorMessage) {
	cGraphicsEngine* engine = nullptr;
	static std::string errMsg;
	try {
		engine = new cGraphicsEngine(targetWindow, screenWidth, screenHeight, config);
	}
	catch (std::exception& e) {
		//std::cerr << "[fatal] graphics engine creation failed with message: " << e.what() << std::endl;
		errMsg = "graphics engine creation failed with message:\n";
		errMsg += e.what();
		if (errorMessage != nullptr) {
			*errorMessage = errMsg.c_str();
		}
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
deferredRenderer(nullptr),
hdrProcessor(nullptr),
shaderScreenCopy(nullptr),
curSceneBuffer(nullptr),
shadowRenderer(nullptr)
{
	for (auto& t : hdrTextures)
		t = nullptr;

	// Create graphics api
	switch (config.rasterEngine.gxApi) {
		case tGraphicsConfig::eGraphicsApi::D3D11:
			gApi = CreateGraphicsApi(targetWindow, screenWidth, screenHeight, eGraphicsApiType::GRAPHICS_API_D3D11);
			break;
		case tGraphicsConfig::eGraphicsApi::OPENGL_43:
			gApi = CreateGraphicsApi(targetWindow, screenWidth, screenHeight, eGraphicsApiType::GRAPHICS_API_OPENGL43);
			break;
		default:
			gApi = nullptr;
	}
	if (!gApi)
		throw std::runtime_error("failed to create graphics api");

	// Create resource manager
	resourceManager = new cResourceManager(gApi);

	// Create texture buffers
	try {
		ReloadBuffers();
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create buffers: ") + e.what());
	}

	// Create shaders
	try {
		LoadShaders();
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create shaders:\n") + e.what());
	}

	// Create deferred renderer
	try {
		deferredRenderer = new cDeferredRenderer(*this);
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create deferred renderer:\n") + e.what());
	}

	// Create hdr post-processor
	try {
		hdrProcessor = new cHDRProcessor(*this);
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create hdr post-processor:\n") + e.what());
	}

	// Create Post processor
	try {
		postProcessor = new cPostProcessor(*this);
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create hdr post-processor:\n") + e.what());
	}

	// Create shadow renderer
	try {
		shadowRenderer = new cShadowRenderer(*this);
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create shadow renderer:\n") + e.what());
	}
}

cGraphicsEngine::~cGraphicsEngine() {
	UnloadShaders();
	SAFE_RELEASE(gApi);

	for (auto& t : hdrTextures)
		SAFE_RELEASE(t);
	SAFE_RELEASE(curSceneBuffer);

	SAFE_DELETE(resourceManager);
	

	SAFE_DELETE(deferredRenderer);
	
	
	
}

void cGraphicsEngine::Release() {
	delete this;
}


////////////////////////////////////////////////////////////////////////////////
//	Utility & Settings

// set global configuration
eGraphicsResult cGraphicsEngine::SetConfig(tGraphicsConfig config) {
	return eGraphicsResult::OK; // no config, no work
}

const char* cGraphicsEngine::GetLastErrorMessage() {
	return lastErrorMessage.c_str();
}

// Reload shaders
eGraphicsResult cGraphicsEngine::ReloadShaders() {
	auto Reload = [this](IShaderProgram** prog, const wchar_t* name)->void {
		IShaderProgram* tmp = SafeLoadShader(gApi, name); // it throws on error!
		(*prog)->Release();
		*prog = tmp;
	};
	try {
		Reload(&shaderScreenCopy, L"shaders/screen_copy.cg");
		deferredRenderer->ReloadShaders();
		hdrProcessor->ReloadShaders();
		shadowRenderer->ReloadShaders();
		postProcessor->ReloadShaders();

		return eGraphicsResult::OK;
	}
	catch (std::exception& e) {
		lastErrorMessage = "";
		lastErrorMessage += "failed to reload shader:\n";
		lastErrorMessage += e.what();
		MessageBoxA(0, lastErrorMessage.c_str(), "FUCK", MB_OK);
		return eGraphicsResult::ERROR_UNKNOWN;
	}
}

// resize screen
eGraphicsResult cGraphicsEngine::Resize(unsigned width, unsigned height) {
	eGraphicsResult result = eGraphicsResult::OK;

	screenWidth = width;
	screenHeight = height;

	gApi->SetBackBufferSize(screenWidth, screenHeight);
	result = deferredRenderer->Resize(screenWidth, screenHeight);
	try {
		ReloadBuffers();
	}
	catch (...) {
		return eGraphicsResult::ERROR_OUT_OF_MEMORY;
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
		return nullptr;
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

// create curtom geometry builder
IGeometryBuilder* cGraphicsEngine::CreateCustomGeometry() {
	return new cUserGeometry(*resourceManager);
}


////////////////////////////////////////////////////////////////////////////////
//	Update scene
eGraphicsResult cGraphicsEngine::Update(float elapsed) {
	// handle incorrect elapsed times
	if (elapsed < 1e-8f) {
		elapsed = 1e-8f;
	}
	// is there anything to render at all...?
	if (graphicsSceneOrder.size() == 0) {
		return eGraphicsResult::OK;
	}

	// render first scene straight to backbuffer
	RenderScene(*graphicsSceneOrder[0], gApi->GetDefaultRenderTarget(), elapsed);

	// set blend state
	tBlendDesc blendScene;
	blendScene.alphaToCoverageEnable = false;
	blendScene.independentBlendEnable = false;

	blendScene[0].blendOp = eBlendOp::ADD; 
	blendScene[0].destBlend = eBlendFactor::INV_SRC_ALPHA;
	blendScene[0].srcBlend = eBlendFactor::SRC_ALPHA;

	blendScene[0].blendOpAlpha = eBlendOp::ADD;	
	blendScene[0].destBlendAlpha = eBlendFactor::ZERO;	
	blendScene[0].srcBlendAlpha = eBlendFactor::ONE;

	blendScene[0].writeMask = (int)eBlendWriteMask::ALL;
	blendScene[0].enable = true;

	// render remaining scenes
	for (size_t i = 1; i < graphicsSceneOrder.size(); ++i) {
		// render dat scene
		RenderScene(*graphicsSceneOrder[i], curSceneBuffer, elapsed);

		// accumulate to backbuffer
		gApi->SetBlendState(blendScene);
		
		gApi->SetRenderTargetDefault();
		gApi->SetShaderProgram(shaderScreenCopy);
		gApi->SetTexture(L"inputTexture", curSceneBuffer);
		gApi->Draw(3);
	}

	/*
	// Draw fucking jpg to backbuffer
	static ITexture2D* gammaTex;
	static bool firstRun = true;
	if (firstRun)
	{
		auto errCode = gApi->CreateTexture(&gammaTex, L"gamma.jpg");
		if ( errCode != eGapiResult::OK )
		{
			int asd = 5;
			asd++;
		}

		firstRun = false;
	}
	gApi->SetRenderTargetDefault();
	gApi->SetShaderProgram(shaderScreenCopy);
	gApi->SetTexture(L"inputTexture", gammaTex);
	gApi->Draw(3);
	*/

	gApi->Present();
	return eGraphicsResult::OK;
}


// Render a graphics scene
void cGraphicsEngine::RenderScene(cGraphicsScene& scene, ITexture2D* target, float elapsed) {
	// load settings from scene
	camera = &scene.camera;
	camera->SetAspectRatio(float((double)screenWidth / (double)screenHeight));
	sceneManager = &scene.sceneManager;
	this->elapsed = elapsed;

	// --- --- render shadow map --- --- //
	shadowRenderer->RenderShadowMaps(scene.sceneManager);

	// --- --- composition w/ deferred --- --- //
	ASSERT(deferredRenderer);
	deferredRenderer->RenderComposition();
	
	//--- --- Deferred rendering result --- --- //
	ITexture2D* deferredComposition = deferredRenderer->GetCompositionBuffer();

	// Screen Space Analytic Reflection
	postProcessor->SetInputSSAR(deferredComposition, deferredRenderer->GetDepthBuffer(), deferredRenderer->GetGBuffer(1)); // Color, depth, normal
	postProcessor->SetOutputSSAR(hdrTextures[0]);
	postProcessor->ProcessSSAR(scene.GetCamera());

	// TODO Graphics Option -> Screen Space Variance Reflection
	//postProcessor->SetInputSSVR(deferredComposition, deferredRenderer->GetDepthBuffer(), deferredRenderer->GetGBuffer(1)); // Color, depth, normal
	//postProcessor->SetOutputSSVR(hdrTextures[0]);
	//postProcessor->ProcessSSVR(scene.GetCamera());

	// Motion blur
	postProcessor->SetInputMB(hdrTextures[0], deferredRenderer->GetDepthBuffer());
	postProcessor->SetOutputMB(deferredComposition, hdrTextures[1], deferredRenderer->GetDepthStencilBuffer()); // Color, velocityBuffer, depth
	postProcessor->ProcessMB(elapsed, scene.GetCamera());

	// Depth of field
	postProcessor->SetInputDOF(deferredComposition, deferredRenderer->GetDepthBuffer());
	postProcessor->SetOutputDOF(hdrTextures[0]);
	postProcessor->ProcessDOF(elapsed, scene.GetCamera());


	// Lol using GBuffer0
	ITexture2D* gBuffer0 = deferredRenderer->GetGBuffer(0);

	// Need write luminance value to alpha channel for FXAA
	// HDR
	if (scene.state.hdr.enabled) {
		hdrProcessor->SetSource(hdrTextures[0]);
		hdrProcessor->SetDestination(gBuffer0);						// set destination
		hdrProcessor->adaptedLuminance = scene.luminanceAdaptation; // copy luminance value
		hdrProcessor->Update(elapsed);								// update hdr
		scene.luminanceAdaptation = hdrProcessor->adaptedLuminance; // copy luminance value
	}
	else {
		gApi->SetRenderTargets(1, &gBuffer0);
		gApi->SetShaderProgram(shaderScreenCopy);
		gApi->SetTexture(0, hdrTextures[0]);
		gApi->Draw(3);
	}

	
	// FXAA
	// gBuffer0 holds LDR values
	postProcessor->SetInputFXAA(gBuffer0);
	postProcessor->SetOutputFXAA(gApi->GetDefaultRenderTarget());
	postProcessor->ProcessFXAA();
}

////////////////////////////////////////////////////////////////////////////////
// Internal stuff
void cGraphicsEngine::LoadShaders() {
	auto Create = [this](const wchar_t* shader)->IShaderProgram* {
		return SafeLoadShader(gApi, shader);
	};
	try {
		shaderScreenCopy = Create(L"shaders/screen_copy.cg");
	}
	catch (...) {
		UnloadShaders();
		throw;
	}
}
void cGraphicsEngine::UnloadShaders() {
	SAFE_RELEASE(shaderScreenCopy);
}

void cGraphicsEngine::ReloadBuffers() {
	// backup
	auto oldBuff = curSceneBuffer;

	// try create new
	ITexture2D::tDesc desc;
	desc.arraySize = 1;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.width = screenWidth;
	desc.height = screenHeight;
	desc.mipLevels = 1;
	desc.usage = eUsage::DEFAULT;

	desc.format = eFormat::R8G8B8A8_UNORM;
	auto errCode = gApi->CreateTexture(&curSceneBuffer, desc);

	if (errCode != eGapiResult::OK) {
		// failed: rollback
		curSceneBuffer = oldBuff;
		// throw
		throw std::runtime_error("scene backbuffer");
	}

	// success: release old
	SAFE_RELEASE(oldBuff);



	// high dynamic ranged textures for post processes, to preserve hdr values across post processes
	for (auto& t : hdrTextures) {
		// Back up
		oldBuff = t;

		desc.format = eFormat::R16G16B16A16_FLOAT;
		errCode = gApi->CreateTexture(&t, desc);

		if (errCode != eGapiResult::OK) {
			t = oldBuff;
			throw std::runtime_error("global hdr textureS in GraphicsEngine::ReloadBuffer");
		}

		SAFE_RELEASE(oldBuff);
	}
}


////////////////////////////////////////////////////////////////////////////////
//	Get sub-components where allowed
cResourceManager* cGraphicsEngine::GetResourceManager() {
	return resourceManager;
}

IGraphicsApi* cGraphicsEngine::GetGraphicsApi() {
	return gApi;
}


////////////////////////////////////////////////////////////////////////////////
// Helpers
auto SafeLoadShader(IGraphicsApi* gApi, const wchar_t* shader)->IShaderProgram* {
	// create shader program
	IShaderProgram* shaderProg;
	auto r = gApi->CreateShaderProgram(&shaderProg, shader);
	// check results
	switch (r) {
		case eGapiResult::OK: {
								  return shaderProg;
		}
		default: {
					 const zsString errMsg = gApi->GetLastErrorMsg();

					 char msg[2048];
					 cStrUtil::ToAnsi(errMsg, msg, 2048);
					 throw std::runtime_error(msg);
		}
	}
};

