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
#include "../../Core/src/IConstantBuffer.h"
#include "../../Core/src/IIndexBuffer.h"

#include "../../Core/src/GraphicsEntity.h"
#include "../../Core/src/Camera.h"

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
	compositionBuffer = NULL;
	depthBuffer = NULL;
	for (auto& v : gBuffer)
		v = NULL;

	// Create shaders
	shaderGBuffer = parent.shaderManager->LoadShader(L"shaders/deferred_gbuffer.cg");
	shaderComposition =	parent.shaderManager->LoadShader(L"shaders/deferred_compose.cg");
	parent.shaderManager->LoadShader(L"shaders/screen_copy.cg");

	if (!shaderGBuffer || !shaderComposition) {
		std::string msg = std::string("Failed to create shaders:") + (shaderGBuffer ? "" : " g-buffer") + (shaderComposition ? "" : " composition");
		if (shaderGBuffer) parent.shaderManager->UnloadShader(shaderGBuffer);
		if (shaderComposition) parent.shaderManager->UnloadShader(shaderComposition);
		shaderGBuffer = shaderComposition = NULL;
		throw std::runtime_error(msg);
	}
	// Create buffers
	if (ReallocBuffers() != eGapiResult::OK) {
		parent.shaderManager->UnloadShader(shaderGBuffer);
		parent.shaderManager->UnloadShader(shaderComposition);
		throw std::runtime_error("failed to create texture buffers");
	}
}

cGraphicsEngine::cDeferredRenderer::~cDeferredRenderer() {
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);
	SAFE_RELEASE(depthBuffer);
}

////////////////////////////////////////////////////////////////////////////////
//	Internal functions

//	Buffer handling
eGapiResult cGraphicsEngine::cDeferredRenderer::ReallocBuffers() {
	// release old buffers first
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);
	SAFE_RELEASE(depthBuffer);

	// create new buffers
	eGapiResult results[] = {
		gApi->CreateTexture(&gBuffer[0],		BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R8G8B8A8_UNORM,		(int)eBind::RENDER_TARGET   | (int)eBind::SHADER_RESOURCE),
		gApi->CreateTexture(&gBuffer[1],		BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R16G16_SINT,		(int)eBind::RENDER_TARGET   | (int)eBind::SHADER_RESOURCE),
		gApi->CreateTexture(&gBuffer[2],		BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R8G8B8A8_UNORM,		(int)eBind::RENDER_TARGET   | (int)eBind::SHADER_RESOURCE),
		gApi->CreateTexture(&compositionBuffer, BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::R16G16B16A16_FLOAT, (int)eBind::RENDER_TARGET   | (int)eBind::SHADER_RESOURCE),
		gApi->CreateTexture(&depthBuffer,		BUFFER_WIDTH, BUFFER_HEIGHT, 1, 1, eFormat::UNKNOWN,			(int)eBind::SHADER_RESOURCE | (int)eBind::DEPTH_STENCIL, eFormat::D24_UNORM_S8_UINT),
	};

	for (int i = 0; i < sizeof(results) / sizeof(results[0]); i++) {
		if (results[i] != eGapiResult::OK) {
			for (auto& p : gBuffer) SAFE_RELEASE(p);
			SAFE_RELEASE(compositionBuffer);
			SAFE_RELEASE(depthBuffer);
			return results[i];
		}
	}
	return eGapiResult::OK;
}

// Render the scene to composition buffer
void cGraphicsEngine::cDeferredRenderer::RenderComposition() {
	ASSERT(parent.sceneManager->GetActiveCamera() != NULL);

	// Clear buffers
	parent.gApi->ClearTexture(depthBuffer);

	// Set BackBuffer
	parent.gApi->SetRenderTargets(3, &gBuffer[0], depthBuffer);

	// Set ShaderProgram
	parent.gApi->SetShaderProgram(shaderGBuffer);

	// Get camera params
	cCamera* cam = parent.sceneManager->GetActiveCamera();
	Matrix44 viewProjMat = cam->GetViewMatrix() * cam->GetProjMatrix();

	// Render each instanceGroup
	for (auto& group : parent.sceneManager->GetInstanceGroups()) {

		// Set Geometry
		const IIndexBuffer* ib = (*group->geom).GetIndexBuffer();
		gApi->SetIndexBuffer(ib);
		gApi->SetVertexBuffer((*(group->geom)).GetVertexBuffer(), shaderGBuffer->GetVertexFormatSize());

		// Set SubMaterials
		auto& mtl = *group->mtl;
		for (size_t i = 0; i < mtl.GetNSubMaterials(); i++) {
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
			Matrix44 wvp = world * viewProjMat;

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

	// Compose pass
	parent.gApi->SetRenderTargets(1, &compositionBuffer, NULL);

	// Set ShaderProgram
	parent.gApi->SetShaderProgram(shaderComposition);

	// Campos for toying with camera attached lights
	struct buffStruct {
		Matrix44 invProj;
		Matrix44 invView;

		Vec3	 camPos;
		bool	 padding0; // padding yeah for Vec3
	}buffer;

	cam->GetViewMatrix().Inverse(buffer.invView);
	cam->GetProjMatrix().Inverse(buffer.invProj);
	buffer.camPos = cam->GetPos();

	IConstantBuffer* camPosBuffer;
	parent.gApi->CreateConstantBuffer(&camPosBuffer, sizeof(buffStruct), eUsage::IMMUTABLE, (void*)&buffer);
	parent.gApi->SetPSConstantBuffer(camPosBuffer, 0);
	
	// Load up gBuffers to composition shader
	for (unsigned i = 0; i < 3; i++)
		parent.gApi->SetTexture(gBuffer[i], i);
	parent.gApi->SetTexture(depthBuffer, 3);

	// Draw triangle, hardware will quadify them automatically :)
	parent.gApi->Draw(3);

	// Free up mem
	SAFE_RELEASE(camPosBuffer);
}

// Access to composition buffer for further processing like post-process & whatever
ITexture2D* cGraphicsEngine::cDeferredRenderer::GetCompositionBuffer() {
	return compositionBuffer;
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