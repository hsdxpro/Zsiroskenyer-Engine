////////////////////////////////////////////////////////////////////////////////
//	ZsiroskenyerTeam
////////////////////////////////////////////////////////////////////////////////
//	Implementation of the cGraphicsEngine::cDeferredRenderer inner class.
//
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "SceneManager.h"
#include "DeferredLightVolume.h"

#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IShaderManager.h"
#include "../../Core/src/IShaderProgram.h"
#include "../../Core/src/ITexture2D.h"
#include "../../Core/src/IConstantBuffer.h"
#include "../../Core/src/IIndexBuffer.h"

#include "../../Core/src/GraphicsEntity.h"
#include "../../Core/src/GraphicsLight.h"
#include "../../Core/src/Camera.h"

// For lerping
#include "../../Core/src/math/math_all.h"

#include <string>
#include <stdexcept>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
//	Constructor & Destructor
cGraphicsEngine::cDeferredRenderer::cDeferredRenderer(cGraphicsEngine& parent)
: shaderGBuffer(NULL), shaderComposition(NULL), parent(parent), gApi(parent.gApi)
{
	// buffers to null
	compositionBuffer = NULL;
	depthBuffer = NULL;
	DOFInput = NULL;
	shaderAmbient = shaderDirectional = shaderPoint = shaderSpot = NULL;
	ibSpot = ibPoint = NULL;
	vbSpot = vbPoint = NULL;
	for (auto& v : gBuffer)
		v = NULL;

	// set size
	bufferWidth = parent.screenWidth;
	bufferHeight = parent.screenHeight;

	// "Creating" shaders
	ReloadShaders();

	if (!shaderGBuffer || !shaderComposition || !shaderDirectional || !shaderAmbient || !shaderPoint) {
		std::string msg = std::string("Failed to create shaders:") + (shaderGBuffer ? "" : " g-buffer") + (shaderComposition ? "" : " composition");
		throw std::runtime_error(msg);
	}
	// Create buffers
	if (ReallocBuffers() != eGapiResult::OK) {
		throw std::runtime_error("failed to create texture buffers");
	}

	// Create shader constant buffers
	eGapiResult gr;
	gr = gApi->CreateConstantBuffer(&gBufferConstantBuffer, 2 * sizeof(Matrix44)+sizeof(Vec4), eUsage::DEFAULT, NULL);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create constant buffers"); }
	gr = gApi->CreateConstantBuffer(&lightPassConstants, 240, eUsage::DEFAULT, NULL);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create constant buffers"); }
	gr = gApi->CreateConstantBuffer(&compConstantBuffer, 2 * sizeof(Matrix44)+sizeof(Vec4), eUsage::DEFAULT, NULL);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create constant buffers"); }

	// Create light volume buffers
	size_t size = sizeof(vbDataLightPoint);
	gr = gApi->CreateVertexBuffer(&vbPoint, sizeof(vbDataLightPoint), eUsage::IMMUTABLE, vbDataLightPoint);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
	gr = gApi->CreateIndexBuffer(&ibPoint, sizeof(ibDataLightPoint), eUsage::IMMUTABLE, ibDataLightPoint);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
	gr = gApi->CreateVertexBuffer(&vbSpot, sizeof(vbDataLightSpot), eUsage::IMMUTABLE, vbDataLightSpot);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
	gr = gApi->CreateIndexBuffer(&ibSpot, sizeof(ibDataLightSpot), eUsage::IMMUTABLE, ibDataLightSpot);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
}


void cGraphicsEngine::cDeferredRenderer::Cleanup() {
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);
	SAFE_RELEASE(depthBuffer);
	SAFE_RELEASE(depthBufferCopy);

	SAFE_RELEASE(ibPoint);
	SAFE_RELEASE(vbPoint);
	SAFE_RELEASE(ibSpot);
	SAFE_RELEASE(vbSpot);

	SAFE_RELEASE(gBufferConstantBuffer);
	SAFE_RELEASE(compConstantBuffer);
	SAFE_RELEASE(lightPassConstants);
}

cGraphicsEngine::cDeferredRenderer::~cDeferredRenderer() {
	Cleanup();
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
	SAFE_RELEASE(DOFInput);

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
	desc.format = eFormat::R16G16B16A16_FLOAT;	results[4] = gApi->CreateTexture(&DOFInput, desc);

	desc.format = eFormat::R24_UNORM_X8_TYPELESS;	desc.depthFormat = eFormat::D24_UNORM_S8_UINT;	desc.bind = (int)eBind::DEPTH_STENCIL;
	results[5] = gApi->CreateTexture(&depthBuffer, desc);
	desc.bind = (int)eBind::SHADER_RESOURCE;
	results[6] = gApi->CreateTexture(&depthBufferCopy, desc);

	for (int i = 0; i < sizeof(results) / sizeof(results[0]); i++) {
		if (results[i] != eGapiResult::OK) {
			for (auto& p : gBuffer) SAFE_RELEASE(p);
			SAFE_RELEASE(compositionBuffer);
			SAFE_RELEASE(DOFInput);
			SAFE_RELEASE(depthBuffer);
			SAFE_RELEASE(depthBufferCopy);
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
	gApi->ClearTexture(compositionBuffer, 0, Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	
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
			// Set textures
			ITexture2D* diffuse = mtl[i].textureDiffuse.get();
			ITexture2D* normal = mtl[i].textureNormal.get();
			ITexture2D* specular = mtl[i].textureSpecular.get();
			ITexture2D* displace = mtl[i].textureDisplace.get();
			
			if(diffuse)	gApi->SetTexture(L"diffuseTex",	diffuse);
			if(normal)	gApi->SetTexture(L"normalTex",	normal);
			//if(specular)	gApi->SetTexture(L"specularTex",specular);
			//if(displace)	gApi->SetTexture(L"displaceTex",displace);
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
	gApi->CopyResource(depthBuffer, depthBufferCopy);

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
		// Additive blending
	blendState[0].blendOp = eBlendOp::ADD;
	blendState[0].blendOpAlpha = eBlendOp::MAX;
	blendState[0].destBlend = eBlendFactor::ONE;
	blendState[0].destBlendAlpha = eBlendFactor::ZERO;
	blendState[0].srcBlend = eBlendFactor::ONE;
	blendState[0].srcBlendAlpha = eBlendFactor::ONE;
	blendState[0].enable = true;
	blendState[0].writeMask = (uint8_t)eBlendWriteMask::ALL;

	gApi->SetDepthStencilState(depthStencilState, 0x01);
	gApi->SetBlendState(blendState);

	// NEW LIGHTING PASS

	// Set render target
	gApi->SetRenderTargets(1, &compositionBuffer, depthBuffer);

	// Sort scene lights by type
	std::vector<cGraphicsLight*> directionalLights;
	std::vector<cGraphicsLight*> spotLights;
	std::vector<cGraphicsLight*> pointLights;
	Vec3 ambientLight(0.0f, 0.0f, 0.0f);

	auto& lightList = parent.sceneManager->GetLights();
	for (auto light : lightList) {
		switch (light->type) {
			case cGraphicsLight::AMBIENT:
				ambientLight += light->color;
				break;
			case cGraphicsLight::DIRECTIONAL:
				directionalLights.push_back(light);
				break;
			case cGraphicsLight::POINT:
				pointLights.push_back(light);
				break;
			case cGraphicsLight::SPOT:
				spotLights.push_back(light);
				break;
			default:
				break;
		}
	}
		
	// Struct for shader constants
	/*
	float4x4 invViewProj : register(c0);
	float4x4 viewProj : register(c4);
	float4 camPos : register(c8);
	float3 lightColor : register(c9);
	float3 lightPos : register(c10);
	float3 lightDir : register(c11);
	float lightRange : register(c12);
	float3 lightAtten : register(c13);
	float2 lightAngle : register(c14);
	*/
	struct {
		Matrix44 invViewProj;
		Matrix44 viewProj;
		Vec3 camPos;		float _pad0;
		Vec3 lightColor;	float _pad1;
		Vec3 lightPos;		float _pad3;
		Vec3 lightDir;		float _pad4;
		float lightRange;	float _pad5[3];
		Vec3 lightAtten;	float _pad6;
		float lightAngleInner, lightAngleOuter;	float _pad7[2];
	} shaderConstants;

	Matrix44 viewProj = cam->GetViewMatrix() * cam->GetProjMatrix();
	shaderConstants.invViewProj = Matrix44::Inverse(viewProj);
	shaderConstants.viewProj = viewProj;

	// Render each light group
	// Directional lights
	gApi->SetShaderProgram(shaderDirectional);
	gApi->SetTexture(gBuffer[0], 0);
	gApi->SetTexture(gBuffer[1], 1);
	gApi->SetTexture(gBuffer[2], 2);
	gApi->SetTexture(depthBufferCopy, 3);

	for (auto light : directionalLights) {

		// load shader constants
		shaderConstants.lightColor = light->color;
		shaderConstants.lightDir = light->direction;
		gApi->SetConstantBufferData(lightPassConstants, &shaderConstants);
		gApi->SetPSConstantBuffer(lightPassConstants, 0);

		// draw an FSQ
		gApi->Draw(3);
	}


	// Ambient lights
	gApi->SetShaderProgram(shaderAmbient);

	// load shader constants
	shaderConstants.lightColor = ambientLight;
	gApi->SetConstantBufferData(lightPassConstants, &shaderConstants);
	gApi->SetPSConstantBuffer(lightPassConstants, 0);
	gApi->Draw(3); //FSQ


	// Point lights
	gApi->SetShaderProgram(shaderPoint);
	gApi->SetVertexBuffer(vbPoint, 4*2*sizeof(float));
	gApi->SetIndexBuffer(ibPoint);
	for (auto light : pointLights) {

		// set shader constants
		shaderConstants.lightAtten = Vec3(light->atten0, light->atten1, light->atten2);
		shaderConstants.lightColor = light->color;
		shaderConstants.lightPos = light->position;
		shaderConstants.lightRange = light->range;
		gApi->SetConstantBufferData(lightPassConstants, &shaderConstants);
		gApi->SetPSConstantBuffer(lightPassConstants, 0);

		// draw that bullshit
		gApi->DrawIndexed(sizeof(ibDataLightPoint) / 3 / sizeof(unsigned));
	}

	// OLD LIGHTING PASS
	/* 
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
	gApi->SetTexture(depthBufferCopy, 3);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
	*/

	// Set back render state to default
	gApi->SetDepthStencilState(depthStencilDefault, 0x00);
	gApi->SetBlendState(blendDefault);

	// MOTION BLUR______________________________________________________________________________________________________________________________________
	gApi->SetRenderTargets(1, &DOFInput, NULL);

	IShaderProgram* motionBlurShader = parent.GetShaderManager()->GetShaderByName(L"motion_blur.cg");
	gApi->SetShaderProgram(motionBlurShader);

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

	gApi->SetTexture(L"textureInput",compositionBuffer);
	gApi->SetTexture(L"depthTexture", depthBufferCopy);

	// asd new
	prevView = currLerpedView;

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);


	// DEPTH OF FIELD______________________________________________________________________________________________________________________________________
	gApi->SetRenderTargets(1, &compositionBuffer, NULL);

	IShaderProgram* DOFShader = parent.GetShaderManager()->GetShaderByName(L"depth_of_field.cg");
	gApi->SetShaderProgram(DOFShader);

	gApi->SetTexture(L"inputTexture",DOFInput);
	gApi->SetTexture(L"depthTexture", depthBufferCopy);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
}

void cGraphicsEngine::cDeferredRenderer::ReloadShaders() {
	shaderGBuffer = parent.shaderManager->ReloadShader(L"shaders/deferred_gbuffer.cg");
	shaderComposition = parent.shaderManager->ReloadShader(L"shaders/deferred_compose.cg");

	shaderAmbient = parent.shaderManager->ReloadShader(L"shaders/deferred_light_ambient.cg");;
	shaderDirectional = parent.shaderManager->ReloadShader(L"shaders/deferred_light_dir.cg");
	shaderPoint = parent.shaderManager->ReloadShader(L"shaders/deferred_light_point.cg");
	shaderSpot = NULL;

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