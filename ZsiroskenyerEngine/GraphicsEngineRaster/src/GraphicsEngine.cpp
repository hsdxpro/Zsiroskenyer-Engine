////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsEngine.h"

#include "SceneManager.h"
#include "ResourceManager.h"

#include "Geometry.h"
#include "..\..\Core\src\GraphicsEntity.h"
#include "..\..\Core\src\Camera.h"

#include "..\..\Core\src\GAPI.h"

#include "..\..\Core\src\math/Matrix44.h"
#include "..\..\Core\src\common.h"

#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>


////////////////////////////////////////////////////////////////////////////////
// DLL pure C interface
extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config, const char** errorMessage) {
	cGraphicsEngine* engine = NULL;
	static std::string errMsg;
	try {
		engine = new cGraphicsEngine(targetWindow, screenWidth, screenHeight, config);
	}
	catch (std::exception& e) {
		//std::cerr << "[fatal] graphics engine creation failed with message: " << e.what() << std::endl;
		errMsg = "graphics engine creation failed with message: ";
		errMsg += e.what();
		if (errorMessage != nullptr) {
			*errorMessage = errMsg.c_str();
		}
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
hdrProcessor(NULL),
shaderScreenCopy(NULL)
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

	// Create resource manager
	resourceManager = new cResourceManager(gApi);

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
	UnloadShaders();
	SAFE_DELETE(resourceManager)
	SAFE_RELEASE(gApi);
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
		*prog = tmp;
	};
	try {
		Reload(&shaderScreenCopy, L"shaders/screen_copy.cg");
		deferredRenderer->ReloadShaders();
		hdrProcessor->ReloadShaders();
		return eGraphicsResult::OK;
	}
	catch (std::exception& e) {
		lastErrorMessage = "";
		lastErrorMessage += "failed to reload shader:\n";
		lastErrorMessage += e.what();
		return eGraphicsResult::ERROR_UNKNOWN;
	}
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
	camera->SetAspectRatio(float((double)screenWidth / (double)screenHeight));
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
		gApi->SetShaderProgram(shaderScreenCopy);
		//gApi->SetTexture(L"texture0", composedBuffer);
		gApi->SetTexture(composedBuffer, 0);
		gApi->Draw(3);
	}
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
					 char* s = new char[errMsg.size() + 1];
					 s[errMsg.size()] = '\0';
					 wcstombs(s, errMsg.c_str(), errMsg.size());
					 std::runtime_error errThrow(s);
					 delete[] s;
					 throw errThrow;
		}
	}
};

