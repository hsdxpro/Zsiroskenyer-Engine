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

// For lerping
#include "../../Core/src/math/math_all.h"

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

	// Prepare constant buffers for shaders gBuffer
	// Matrix44, worldViewProj, world.   Vec3 camPos, + 1 byte dummy ( GBUFFER)
	eGapiResult gr = gApi->CreateConstantBuffer(&gBufferConstantBuffer, 2 * sizeof(Matrix44)+sizeof(Vec4), eUsage::DEFAULT, NULL);

	// Matrix44 invViewProj, Vec3 campos, + 1 byte dummy( COMPOSITION)
	gr = gApi->CreateConstantBuffer(&compConstantBuffer, 2 * sizeof(Matrix44)+sizeof(Vec4), eUsage::DEFAULT, NULL);
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
	eGapiResult results[7];
	ITexture2D::tDesc desc;
	desc.width = bufferWidth;
	desc.height = bufferHeight;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.usage = eUsage::DEFAULT;

	desc.format = eFormat::R8G8B8A8_UNORM;		results[0] = gApi->CreateTexture(&gBuffer[0], desc);
	desc.format = eFormat::R16G16_SNORM;		results[1] = gApi->CreateTexture(&gBuffer[1], desc);
	desc.format = eFormat::R8G8B8A8_UNORM;		results[2] = gApi->CreateTexture(&gBuffer[2], desc);
	desc.format = eFormat::R16G16B16A16_FLOAT;	results[3] = gApi->CreateTexture(&compositionBuffer, desc);
	desc.format = eFormat::R16G16B16A16_FLOAT;	results[4] = gApi->CreateTexture(&helperBuffer, desc);

	desc.format = eFormat::R24_UNORM_X8_TYPELESS;	desc.depthFormat = eFormat::D24_UNORM_S8_UINT;	desc.bind = (int)eBind::DEPTH_STENCIL;
	results[5] = gApi->CreateTexture(&depthBuffer, desc);
	desc.bind = (int)eBind::SHADER_RESOURCE;
	results[6] = gApi->CreateTexture(&depthBufferShader, desc);

	for (int i = 0; i < sizeof(results) / sizeof(results[0]); i++) {
		if (results[i] != eGapiResult::OK) {
			for (auto& p : gBuffer) SAFE_RELEASE(p);
			SAFE_RELEASE(compositionBuffer);
			SAFE_RELEASE(helperBuffer);
			SAFE_RELEASE(depthBuffer);
			SAFE_RELEASE(depthBufferShader);
			return results[i];
		}
	}
	return eGapiResult::OK;
}

// Render the scene to composition buffer
void cGraphicsEngine::cDeferredRenderer::RenderComposition() {
	// Declared here, will be used throughout whole function
	tDepthStencilDesc depthStencilState;
	tBlendDesc blendState;

	// Clear buffers
	gApi->ClearTexture(depthBuffer);
	gApi->ClearTexture(gBuffer[0], 0, Vec4(0.5f, 0.7f, 0.8f, 0.0f));
	gApi->ClearTexture(gBuffer[1], 0, Vec4(0.5f, 0.5f, 0.0f, 0.0f));
	gApi->ClearTexture(gBuffer[2], 0, Vec4(0.0f, 0.0f, 0.0f, 0.0f));

	
	//----------------------------------------------------------------------//
	// --- --- --- --- --- --- --- GBUFFER PASS --- --- --- --- --- --- --- //
	//----------------------------------------------------------------------//

	// Set g-buffer multiple-render targets
	gApi->SetRenderTargets(3, gBuffer, depthBuffer);
	// Set g-buffer shader
	gApi->SetShaderProgram(shaderGBuffer);
	// Get camera params
	cCamera* cam = parent.camera;

	// Set stencil and blend state
	depthStencilState = depthStencilDefault;
	depthStencilState.stencilEnable = true;
	depthStencilState.stencilOpBackFace.stencilCompare = eComparisonFunc::ALWAYS;
	depthStencilState.stencilOpBackFace.stencilPass = eStencilOp::REPLACE;
	depthStencilState.stencilOpFrontFace = depthStencilState.stencilOpBackFace;
	depthStencilState.stencilReadMask = depthStencilState.stencilWriteMask = 0x01;
	eGapiResult r = gApi->SetDepthStencilState(depthStencilState, 0x01);


	// Just lerping view, then combine how you want
	Matrix44 currView = cam->GetViewMatrix();
	static Matrix44 prevView = currView;
	Matrix44 currLerpedView = lerp( prevView, currView, 0.35f);
	Matrix44 projMat = cam->GetProjMatrix();
	Matrix44 viewProjMat = currLerpedView * projMat;

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
				gApi->SetTexture(diffuse, 0);
			if (normal != NULL)
				gApi->SetTexture(normal, 1);
			if (specular != NULL)
				gApi->SetTexture(specular, 2);
			if (displace != NULL)
				gApi->SetTexture(displace, 3);
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

	// copy depth to shader resource
	gApi->CopyResource(depthBuffer, depthBufferShader);

	//--------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- COMPOSITION PASS --- --- --- --- --- --- --- //
	//--------------------------------------------------------------------------//
	
	// Set render states
		// Depth-stencil
	depthStencilState = depthStencilDefault;
	depthStencilState.depthCompare = eComparisonFunc::ALWAYS;
	depthStencilState.depthWriteEnable = false;
	depthStencilState.stencilEnable = true;
	depthStencilState.stencilOpBackFace.stencilCompare = eComparisonFunc::EQUAL;
	depthStencilState.stencilOpBackFace.stencilFail = eStencilOp::KEEP;
	depthStencilState.stencilOpBackFace.stencilPass = eStencilOp::KEEP;
	depthStencilState.stencilOpBackFace.stencilPassDepthFail = eStencilOp::KEEP;
	depthStencilState.stencilReadMask = depthStencilState.stencilWriteMask = 0x01;
	depthStencilState.stencilOpFrontFace = depthStencilState.stencilOpBackFace;
	gApi->SetDepthStencilState(depthStencilState, 0x01);
		// Additive blending


	// Set render target
	gApi->SetRenderTargets(1, &compositionBuffer, depthBuffer);
	// Set ShaderProgram
	gApi->SetShaderProgram(shaderComposition);

	// Campos for toying with camera attached lights
	struct compBuffConstantBuff {
		Matrix44 invViewProj;
		Matrix44 proj;
		Vec3 camPos;
	} buffer;
	buffer.invViewProj = Matrix44::Inverse(viewProjMat);
	buffer.camPos = cam->GetPos();
	buffer.proj = projMat;

	gApi->SetConstantBufferData(compConstantBuffer, &buffer);
	gApi->SetPSConstantBuffer(compConstantBuffer, 0);

	// Load up gBuffers to composition shader
	for (unsigned i = 0; i < 3; i++)
		gApi->SetTexture(gBuffer[i], i);

	// Load up depth buffer
	gApi->SetTexture(depthBufferShader, 3);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);

	// Set back render state to default
	gApi->SetDepthStencilState(depthStencilDefault, 0x00);
	gApi->SetBlendState(blendDefault);


	// MOTION BLUR______________________________________________________________________________________________________________________________________
	gApi->SetRenderTargets(1, &helperBuffer, NULL);

	IShaderProgram* motionBlurShProg = parent.GetShaderManager()->GetShaderByName(L"motion_blur.cg");
	gApi->SetShaderProgram(motionBlurShProg);

	static IConstantBuffer* shitBuffer;
	static eGapiResult gr = gApi->CreateConstantBuffer(&shitBuffer, 2 * sizeof(Matrix44), eUsage::DEFAULT, NULL);

	struct shitBuffStruct
	{
		Matrix44 invViewProj;
		Matrix44 prevViewProj;
	} asd;

	asd.invViewProj = Matrix44::Inverse(viewProjMat);
	asd.prevViewProj = prevView * projMat;

	gApi->SetConstantBufferData(shitBuffer, &asd);
	gApi->SetPSConstantBuffer(shitBuffer, 0);

	gApi->SetTexture(compositionBuffer, 0);
	gApi->SetTexture(depthBufferShader, 1);

	// asd new
	prevView = currLerpedView;

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);


	// DEPTH OF FIELD______________________________________________________________________________________________________________________________________
	gApi->SetRenderTargets(1, &compositionBuffer, NULL);

	IShaderProgram* dofShProg = parent.GetShaderManager()->GetShaderByName(L"depth_of_field.cg");
	gApi->SetShaderProgram(dofShProg);

	gApi->SetTexture(helperBuffer, 0);
	gApi->SetTexture(depthBufferShader, 1);

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
	eGapiResult r = ReallocBuffers();
	// analyze results and return
	if (r == eGapiResult::OK)
		return eGraphicsResult::OK;
	else if (r == eGapiResult::ERROR_OUT_OF_MEMORY)
		return eGraphicsResult::ERROR_OUT_OF_MEMORY;
	else
		return eGraphicsResult::ERROR_UNKNOWN;
}