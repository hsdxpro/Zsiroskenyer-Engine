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


////////////////////////////////////////////////////////////////////////////////
//	Constructor & Destructor
cGraphicsEngine::cDeferredRenderer::cDeferredRenderer(cGraphicsEngine& parent)
: shaderGBuffer(NULL), shaderComposition(NULL), parent(parent), gApi(parent.gApi)
{	
	// buffers to null
	compositionBuffer = NULL;
	depthBuffer = NULL;
	helperBuffer = NULL;
	for (auto& v : gBuffer)
		v = NULL;

	// set size
	bufferWidth = parent.screenWidth;
	bufferHeight = parent.screenHeight;

	// "Creating" shaders
	ReloadShaders();

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

	// Prepare constant buffers for shaders (gBuffer, composition)
	// Matrix44, worldViewProj, world.   Vec3 camPos, + 1 byte dummy ( GBUFFER)
	eGapiResult gr = gApi->CreateConstantBuffer(&gBufferConstantBuffer, 2 * sizeof(Matrix44) + sizeof(Vec4), eUsage::DEFAULT, NULL);

	// Matrix44 invViewProj, Vec3 campos, + 1 byte dummy( COMPOSITION)
	gr = gApi->CreateConstantBuffer(&compConstantBuffer, sizeof(Matrix44)+sizeof(Vec4), eUsage::DEFAULT, NULL);
}

cGraphicsEngine::cDeferredRenderer::~cDeferredRenderer() {
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);
	SAFE_RELEASE(depthBuffer);

	SAFE_DELETE(gBufferConstantBuffer);
	SAFE_DELETE(compConstantBuffer);
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
	SAFE_RELEASE(helperBuffer);

	// create new buffers
	eGapiResult results[6];
	ITexture2D::tDesc desc;
	desc.width = bufferWidth;
	desc.height = bufferHeight;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.usage = eUsage::DEFAULT;
	/*
	results[0] = gApi->CreateTexture(&gBuffer[0], bufferWidth, bufferHeight, 1, 1, eFormat::R8G8B8A8_UNORM, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	results[1] = gApi->CreateTexture(&gBuffer[1], bufferWidth, bufferHeight, 1, 1, eFormat::R16G16_SNORM, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	results[2] = gApi->CreateTexture(&gBuffer[2], bufferWidth, bufferHeight, 1, 1, eFormat::R8G8B8A8_UNORM, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	results[3] = gApi->CreateTexture(&compositionBuffer, bufferWidth, bufferHeight, 1, 1, eFormat::R16G16B16A16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	results[4] = gApi->CreateTexture(&helperBuffer, bufferWidth, bufferHeight, 1, 1, eFormat::R16G16B16A16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	results[5] = gApi->CreateTexture(&depthBuffer, bufferWidth, bufferHeight, 1, 1, eFormat::UNKNOWN, (int)eBind::SHADER_RESOURCE | (int)eBind::DEPTH_STENCIL, eFormat::D24_UNORM_S8_UINT);
	*/
	desc.format = eFormat::R8G8B8A8_UNORM;		results[0] = gApi->CreateTexture(&gBuffer[0], desc);
	desc.format = eFormat::R16G16_SNORM;		results[1] = gApi->CreateTexture(&gBuffer[1], desc);
	desc.format = eFormat::R8G8B8A8_UNORM;		results[2] = gApi->CreateTexture(&gBuffer[2], desc);
	desc.format = eFormat::R16G16B16A16_FLOAT;	results[3] = gApi->CreateTexture(&compositionBuffer, desc);
	desc.format = eFormat::R16G16B16A16_FLOAT;	results[4] = gApi->CreateTexture(&helperBuffer, desc);
	
	desc.format = eFormat::R32_TYPELESS;	desc.depthFormat = eFormat::D24_UNORM_S8_UINT;	desc.bind = (int)eBind::SHADER_RESOURCE | (int)eBind::DEPTH_STENCIL;
	results[5] = gApi->CreateTexture(&depthBuffer, desc);

	for (int i = 0; i < sizeof(results) / sizeof(results[0]); i++) {
		if (results[i] != eGapiResult::OK) {
			for (auto& p : gBuffer) SAFE_RELEASE(p);
			SAFE_RELEASE(compositionBuffer);
			SAFE_RELEASE(helperBuffer);
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
	gApi->ClearTexture(depthBuffer);
	gApi->ClearTexture(gBuffer[0], 0, Vec4(0, 0, 0, 0));
// GBUFFER PASS______________________________________________________________________________________________________________________________________

	// Set BackBuffer
	gApi->SetRenderTargets(3, gBuffer, depthBuffer);

	// Set ShaderProgram
	gApi->SetShaderProgram(shaderGBuffer);

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
			ITexture2D* diffuse  = mtl[i].textureDiffuse.get();
			ITexture2D* normal   = mtl[i].textureNormal.get();
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

			struct gBuffConstantBuff
			{
				Matrix44 wvp;
				Matrix44 world;
				Vec3 camPos;
			} buff;
			buff.wvp = wvp;
			buff.world = world;
			buff.camPos = cam->GetPos();

			gApi->SetConstantBufferData(gBufferConstantBuffer, &buff);
			gApi->SetVSConstantBuffer(gBufferConstantBuffer, 0);

			// Draw entity..
			gApi->DrawIndexed(ib->GetSize() / sizeof(unsigned));
		}
	}



// COMPOSITION PASS______________________________________________________________________________________________________________________________________
	gApi->SetRenderTargets(1, &compositionBuffer, NULL);

	// Set ShaderProgram
	gApi->SetShaderProgram(shaderComposition);

	// Campos for toying with camera attached lights
	struct compBuffConstantBuff {
		Matrix44 invViewProj;
		Vec3 camPos;
	} buffer;
	buffer.invViewProj = Matrix44::Inverse(viewProjMat);
	buffer.camPos = cam->GetPos();

	gApi->SetConstantBufferData(compConstantBuffer, &buffer);
	gApi->SetPSConstantBuffer(compConstantBuffer, 0);
	
	// Load up gBuffers to composition shader
	for (unsigned i = 0; i < 3; i++)
		gApi->SetTexture(gBuffer[i], i);

	// Load up depth buffer
	gApi->SetTexture(depthBuffer, 3);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);



// MOTION BLUR______________________________________________________________________________________________________________________________________
	gApi->SetRenderTargets(1, &helperBuffer, NULL);

	IShaderProgram* motionBlurShProg = parent.GetShaderManager()->GetShaderByName(L"motion_blur.cg");
	gApi->SetShaderProgram(motionBlurShProg);

	static IConstantBuffer* shitBuffer;
	static eGapiResult gr = gApi->CreateConstantBuffer(&shitBuffer, 2 * sizeof(Matrix44), eUsage::DEFAULT, NULL);

	static Matrix44 prevViewProj = viewProjMat;
	struct shitBuffStruct
	{
		Matrix44 invViewProj;
		Matrix44 prevViewProj;
	} asd;
	asd.invViewProj = buffer.invViewProj;
	asd.prevViewProj = prevViewProj;

	gApi->SetConstantBufferData(shitBuffer, &asd);
	gApi->SetPSConstantBuffer(shitBuffer, 0);

	gApi->SetTexture(compositionBuffer, 0);
	gApi->SetTexture(depthBuffer, 1);

	prevViewProj = viewProjMat;

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);



// DEPTH OF FIELD______________________________________________________________________________________________________________________________________
	gApi->SetRenderTargets(1, &compositionBuffer, NULL);

	IShaderProgram* dofShProg = parent.GetShaderManager()->GetShaderByName(L"depth_of_field.cg");
	gApi->SetShaderProgram(dofShProg);

	gApi->SetTexture(helperBuffer, 0);
	gApi->SetTexture(depthBuffer, 1);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
}

void cGraphicsEngine::cDeferredRenderer::ReloadShaders() {
	shaderGBuffer = parent.shaderManager->ReloadShader(L"shaders/deferred_gbuffer.cg");
	shaderComposition = parent.shaderManager->ReloadShader(L"shaders/deferred_compose.cg");
	parent.screenCopyShader = parent.shaderManager->ReloadShader(L"shaders/screen_copy.cg");
	parent.shaderManager->ReloadShader(L"shaders/motion_blur.cg");
	parent.shaderManager->ReloadShader(L"shaders/depth_of_field.cg");
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