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

using namespace std;


cGraphicsEngine::cDeferredRenderer::cDeferredRenderer(cGraphicsEngine& parent)
: shaderGBuffer(NULL), shaderComposition(NULL), parent(parent), gApi(parent.gApi)
{	
	for (auto& v : gBuffer)
		v = NULL;
	// create shaders
	shaderGBuffer = parent.shaderManager->LoadShader(L"shaders/", L"deferred_gbuffer.cg");
	shaderComposition =	parent.shaderManager->LoadShader(L"shaders/", L"deferred_compose.cg");
	if (!shaderGBuffer || !shaderComposition) {
		string msg = string("failed to create shaders:") + (shaderGBuffer ? "" : " g-buffer") + (shaderComposition ? "" : " composition");
		if (shaderGBuffer) parent.shaderManager->UnloadShader(shaderGBuffer);
		if (shaderComposition) parent.shaderManager->UnloadShader(shaderComposition);
		shaderGBuffer = shaderComposition = NULL;
		throw runtime_error(msg);
	}
	// create texture buffers
	auto r1 = gApi->CreateTexture(&gBuffer[0], BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R8G8B8A8_UNORM, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	auto r2 = gApi->CreateTexture(&gBuffer[1], BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R16G16_SINT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	auto r3 = gApi->CreateTexture(&gBuffer[2], BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R8G8B8A8_UNORM, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	auto rc = gApi->CreateTexture(&compositionBuffer, BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R16G16B16A16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	if (r1 != eGapiResult::OK || r2 != eGapiResult::OK || r3 != eGapiResult::OK || rc!=eGapiResult::OK) {
		for (auto& v : gBuffer)
			SAFE_RELEASE(v);
		SAFE_RELEASE(compositionBuffer);
		parent.shaderManager->UnloadShader(shaderGBuffer);
		parent.shaderManager->UnloadShader(shaderComposition);
		throw runtime_error("failed to create texture buffers");
	}
}

cGraphicsEngine::cDeferredRenderer::~cDeferredRenderer() {
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);
}