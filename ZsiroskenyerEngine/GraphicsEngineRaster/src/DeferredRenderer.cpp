////////////////////////////////////////////////////////////////////////////////
//	ZsiroskenyerTeam
////////////////////////////////////////////////////////////////////////////////
//	Implementation of the cGraphicsEngine::cDeferredRenderer inner class.
//
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "SceneManager.h"

#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IShaderManager.h"
#include "../../Core/src/IShaderProgram.h"
#include "../../Core/src/ITexture2D.h"

#include <string>
#include <stdexcept>


#define BUFFER_WIDTH	800
#define BUFFER_HEIGHT	600


////////////////////////////////////////////////////////////////////////////////
//	Constructor & Destructor
cGraphicsEngine::cDeferredRenderer::cDeferredRenderer(cGraphicsEngine& parent)
: shaderGBuffer(NULL), shaderComposition(NULL), parent(parent), gApi(parent.gApi)
{	
	// buffers to null
	for (auto& v : gBuffer)
		v = NULL;

	// create shaders
	shaderGBuffer = parent.shaderManager->LoadShader(L"shaders/", L"deferred_gbuffer.cg");
	shaderComposition =	parent.shaderManager->LoadShader(L"shaders/", L"deferred_compose.cg");
	if (!shaderGBuffer || !shaderComposition) {
		std::string msg = std::string("failed to create shaders:") + (shaderGBuffer ? "" : " g-buffer") + (shaderComposition ? "" : " composition");
		if (shaderGBuffer) parent.shaderManager->UnloadShader(shaderGBuffer);
		if (shaderComposition) parent.shaderManager->UnloadShader(shaderComposition);
		shaderGBuffer = shaderComposition = NULL;
		throw std::runtime_error(msg);
	}
	// create buffers
	if (ReallocBuffers()!=eGapiResult::OK) {
		parent.shaderManager->UnloadShader(shaderGBuffer);
		parent.shaderManager->UnloadShader(shaderComposition);
		throw std::runtime_error("failed to create texture buffers");
	}
}

cGraphicsEngine::cDeferredRenderer::~cDeferredRenderer() {
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);
}

////////////////////////////////////////////////////////////////////////////////
//	Internal functions

//	Buffer handling
eGapiResult cGraphicsEngine::cDeferredRenderer::ReallocBuffers() {
	// release old buffers first
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);

	// create new buffers
	eGapiResult results[] = {
		gApi->CreateTexture(&gBuffer[0], bufferWidth, bufferHeight, 1, 1, eFormat::R8G8B8A8_UNORM, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE),
		gApi->CreateTexture(&gBuffer[1], bufferWidth, bufferHeight, 1, 1, eFormat::R16G16_SINT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE),
		gApi->CreateTexture(&gBuffer[2], bufferWidth, bufferHeight, 1, 1, eFormat::R8G8B8A8_UNORM, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE),
		gApi->CreateTexture(&compositionBuffer, bufferWidth, bufferHeight, 1, 1, eFormat::R16G16B16A16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE),
	};

	for (int i = 0; i < sizeof(results) / sizeof(results[0]); i++) {
		if (results[i] != eGapiResult::OK) {
			for (auto& p : gBuffer) SAFE_RELEASE(p);
			SAFE_RELEASE(compositionBuffer);
			return results[i];
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//	Public usage
eGraphicsResult cGraphicsEngine::cDeferredRenderer::Resize(unsigned width, unsigned height) {
	// reallocate buffers w/ new size
	bufferWidth = width;
	bufferHeight = height;
	eGapiResult r= ReallocBuffers();
	// analyze results and return
	if (r == eGapiResult::OK)
		return eGraphicsResult::OK;
	else if (r == eGapiResult::ERROR_OUT_OF_MEMORY)
		return eGraphicsResult::ERROR_OUT_OF_MEMORY;
	else
		return eGraphicsResult::ERROR_UNKNOWN;
}