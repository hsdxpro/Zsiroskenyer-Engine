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
#include "../../Core/src/IShaderProgram.h"
#include "../../Core/src/ITexture2D.h"
#include "../../Core/src/IIndexBuffer.h"

#include "GraphicsEntity.h"
#include "GraphicsLight.h"
#include "../../Core/src/Camera.h"

#include "../../Core/src/lighting/lighting.h"

// For lerping
#include "../../Core/src/math/math_all.h"

#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//	Constructor & Destructor
cGraphicsEngine::cDeferredRenderer::cDeferredRenderer(cGraphicsEngine& parent)
: shaderGBuffer(nullptr), parent(parent), gApi(parent.gApi)
{
	// Buffers to null
	compositionBuffer = nullptr;
	depthBuffer = nullptr;
	depthBufferCopy = nullptr;
	ambientOcclusionBuffer = nullptr;
	randomTexture = nullptr;
	for (auto& v : gBuffer)
		v = nullptr;

	// shaders to null
	shaderAmbient = shaderDirectional = shaderPoint = shaderSpot = nullptr;
	shaderSky = nullptr;
	shaderSSAO = nullptr;
	shaderHBAO = nullptr;

	// light volumes to null
	ibSpot = ibPoint = nullptr;
	vbSpot = vbPoint = nullptr;


	// Set size
	bufferWidth = parent.screenWidth;
	bufferHeight = parent.screenHeight;

	// Loading shaders
	try {
		LoadShaders();
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create shaders:\n") + e.what());
	}

	// Create buffers
	if (ReallocBuffers() != eGapiResult::OK) {
		throw std::runtime_error("failed to create texture buffers");
	}

	// Create light volume buffers
	cVertexFormat f;
	cVertexFormat::Attribute a;
		a.bitsPerComponent = cVertexFormat::_32_BIT;
		a.nComponents = 3;
		a.semantic = cVertexFormat::POSITION;
		a.type = cVertexFormat::FLOAT;
	f.Create({ a });

	eGapiResult gr;
	size_t size = sizeof(vbDataLightPoint);
	gr = gApi->CreateVertexBuffer(&vbPoint, eUsage::IMMUTABLE, f, sizeof(vbDataLightPoint), vbDataLightPoint);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
	gr = gApi->CreateIndexBuffer(&ibPoint, eUsage::IMMUTABLE, sizeof(ibDataLightPoint), ibDataLightPoint);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
	gr = gApi->CreateVertexBuffer(&vbSpot, eUsage::IMMUTABLE, f, sizeof(vbDataLightSpot), vbDataLightSpot);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
	gr = gApi->CreateIndexBuffer(&ibSpot, eUsage::IMMUTABLE, sizeof(ibDataLightSpot), ibDataLightSpot);
	if (gr != eGapiResult::OK) { Cleanup(); throw std::runtime_error("failed to create light volume buffers"); }
}


void cGraphicsEngine::cDeferredRenderer::Cleanup() {
	// shaders
	UnloadShaders();
	// texture objects
	for (auto& v : gBuffer)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer);
	SAFE_RELEASE(depthBuffer);
	SAFE_RELEASE(depthBufferCopy);
	SAFE_RELEASE(ambientOcclusionBuffer);
	SAFE_RELEASE(randomTexture);

	// mesh objects
	SAFE_RELEASE(ibPoint);
	SAFE_RELEASE(vbPoint);
	SAFE_RELEASE(ibSpot);
	SAFE_RELEASE(vbSpot);
}

cGraphicsEngine::cDeferredRenderer::~cDeferredRenderer() {
	Cleanup();
}

////////////////////////////////////////////////////////////////////////////////
//	Internal functions

//	Buffer handling
eGapiResult cGraphicsEngine::cDeferredRenderer::ReallocBuffers() {
	// backup old buffers
	for (auto& p : gBuffer) SAFE_RELEASE(p);
	ITexture2D* gBuffer_[3] = {gBuffer[0], gBuffer[1], gBuffer[1]};
	auto compositionBuffer_ = compositionBuffer;
	auto depthBuffer_ = depthBuffer;
	auto depthBufferCopy_ = depthBufferCopy;
	auto ambientOcclusionBuffer_ = ambientOcclusionBuffer;
	auto randomTexture_ = randomTexture;

	// create new buffers
	const int nBuffers = 8;

	eGapiResult results[nBuffers]; memset(&results, 0, sizeof(eGapiResult) * nBuffers); // Default everything is OK = 0
	int idxResult = -1;
	ITexture2D::tDesc desc;
	desc.width = bufferWidth;
	desc.height = bufferHeight;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.usage = eUsage::DEFAULT;

	// g-buffers
	desc.format = eFormat::R8G8B8A8_UNORM;		results[++idxResult] = gApi->CreateTexture(&gBuffer[0], desc);
	desc.format = eFormat::R16G16_SNORM;		results[++idxResult] = gApi->CreateTexture(&gBuffer[1], desc);
	desc.format = eFormat::R8G8B8A8_UNORM;		results[++idxResult] = gApi->CreateTexture(&gBuffer[2], desc);
	// light accumulation buffer
	desc.format = eFormat::R16G16B16A16_FLOAT;	results[++idxResult] = gApi->CreateTexture(&compositionBuffer, desc);
	// ambient occlusion
	desc.width = desc.width>=256 ? desc.width / 2 : desc.width;
	desc.height = desc.height>=256 ? desc.height / 2 : desc.height;
	desc.format = eFormat::R8_UNORM;			results[++idxResult] = gApi->CreateTexture(&ambientOcclusionBuffer, desc);
	desc.width = bufferWidth; desc.height = bufferHeight;

	// Random texture for ambient occlusion
	//desc.bind = (int)eBind::SHADER_RESOURCE;
	//desc.width = 4;
	//desc.height = 4;
	//desc.format = eFormat::R16G16B16A16_SNORM;
	//results[++idxResult] = gApi->CreateTexture(&randomTexture, desc);


	// depth buffer and depth copy
	desc.format = eFormat::R24_UNORM_X8_TYPELESS;	desc.depthFormat = eFormat::D24_UNORM_S8_UINT;	desc.bind = (int)eBind::DEPTH_STENCIL;
	results[++idxResult] = gApi->CreateTexture(&depthBuffer, desc);
	desc.bind = (int)eBind::SHADER_RESOURCE;
	results[++idxResult] = gApi->CreateTexture(&depthBufferCopy, desc);

	// check results
	for (int i = 0; i < sizeof(results) / sizeof(results[0]); i++) {
		if (results[i] != eGapiResult::OK) {
			// release non-working stuff
			for (auto& p : gBuffer) SAFE_RELEASE(p);
			SAFE_RELEASE(compositionBuffer);
			SAFE_RELEASE(depthBuffer);
			SAFE_RELEASE(depthBufferCopy);
			SAFE_RELEASE(ambientOcclusionBuffer);
			
			// rollback to previous
			compositionBuffer = compositionBuffer_;
			depthBuffer = depthBuffer_;
			depthBufferCopy = depthBufferCopy_;
			ambientOcclusionBuffer = ambientOcclusionBuffer_;
			for (int j = 0; j < sizeof(gBuffer) / sizeof(gBuffer[0]); ++j) {
				gBuffer[j] = gBuffer_[j];
			}

			// return dat error
			return results[i];
		}
	}

	// release old buffers on success
	for (auto& v : gBuffer_)
		SAFE_RELEASE(v);
	SAFE_RELEASE(compositionBuffer_);
	SAFE_RELEASE(depthBuffer_);
	SAFE_RELEASE(depthBufferCopy_);
	SAFE_RELEASE(ambientOcclusionBuffer_);


	return eGapiResult::OK;
}

// load shaders
void cGraphicsEngine::cDeferredRenderer::LoadShaders() {
	auto Create = [this](const wchar_t* shader)->IShaderProgram* {
		return SafeLoadShader(gApi, shader);
	};
	try {
		shaderGBuffer = Create(L"shaders/deferred_gbuffer.cg");

		shaderAmbient = Create(L"shaders/deferred_light_ambient.cg");
		shaderDirectional = Create(L"shaders/deferred_light_dir.cg");
		shaderPoint = Create(L"shaders/deferred_light_point.cg");
		shaderSpot = Create(L"shaders/deferred_light_spot.cg");

		shaderSky = Create(L"shaders/sky.cg");

		shaderSSAO = Create(L"shaders/ssao.cg");
		shaderHBAO = Create(L"shaders/hbao.cg");
	}
	catch (...) {
		UnloadShaders();
		throw;
	}
}
// unload shaders
void cGraphicsEngine::cDeferredRenderer::UnloadShaders() {
	SAFE_RELEASE(shaderGBuffer);

	SAFE_RELEASE(shaderAmbient);
	SAFE_RELEASE(shaderDirectional);
	SAFE_RELEASE(shaderPoint);
	SAFE_RELEASE(shaderSpot);

	SAFE_RELEASE(shaderSky);

	SAFE_RELEASE(shaderSSAO);
	SAFE_RELEASE(shaderHBAO);
}

// reload shaders
void cGraphicsEngine::cDeferredRenderer::ReloadShaders() {
	auto Reload = [this](IShaderProgram** prog, const wchar_t* name)->void {
		IShaderProgram* tmp = SafeLoadShader(gApi, name); // it throws on error!
		(*prog)->Release();
		*prog = tmp;
	};
	Reload(&shaderGBuffer, L"shaders/deferred_gbuffer.cg");

	Reload(&shaderAmbient, L"shaders/deferred_light_ambient.cg");
	Reload(&shaderDirectional, L"shaders/deferred_light_dir.cg");
	Reload(&shaderPoint, L"shaders/deferred_light_point.cg");
	Reload(&shaderSpot, L"shaders/deferred_light_spot.cg");

	Reload(&shaderSky, L"shaders/sky.cg");

	Reload(&shaderSSAO, L"shaders/ssao.cg");
	Reload(&shaderSSAO, L"shaders/hbao.cg");
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
	gApi->SetDepthStencilState(depthStencilState, 0x01); // depth
	gApi->SetBlendState(blendDefault); // blend


	Matrix44 projMat = cam->GetProjMatrix();
	Matrix44 viewMat = cam->GetViewMatrix();
	Matrix44 viewProjMat = viewMat * projMat;
	Matrix44 invViewProjMat = Matrix44Inverse(viewProjMat);
	Matrix44& prevView = *parent.lastCameraMatrix;

	// Foreach: Instance group
	for (auto& group : parent.sceneManager->GetInstanceGroups()) {
		cGeometry& geom = *group->geom.get();
		cMaterial& mtl = *group->mtl.get();
		
		// Set Geometry
		gApi->SetIndexBuffer(geom.GetIndexBuffer());
		gApi->SetVertexBuffer(geom.GetVertexBuffer());
		
		// Foreach: Entity per-poly material group
		for (auto& matGroup : geom.GetMatGroups()) {
			// Set material
			struct {
				// basic parameters
				Vec3 diffuseColor;
				float glossiness;
				float specularLevel;

				// texture properties
				int32_t hasDiffuseMap;
				int32_t hasNormalMap;
				int32_t hasGlossinessMap;
				int32_t hasSpecLevelMap;
				int32_t useCutout;
			} mtlConstants;

			auto nSubMtl = mtl.GetNSubMaterials();
			if (nSubMtl > 0) {
				auto submtl = mtl[matGroup.id % mtl.GetNSubMaterials()];

				// deal with textures
				ITexture2D* diffuse = submtl.textureDiffuse.get();
				ITexture2D* normal = submtl.textureNormal.get();
				ITexture2D* specular = submtl.textureSpecular.get();

				mtlConstants.hasDiffuseMap = (diffuse != nullptr);
				if (diffuse) {
					gApi->SetTexture(L"diffuseMap", diffuse);
				}
				mtlConstants.hasNormalMap = (normal != nullptr);
				if (normal) {
					gApi->SetTexture(L"normalMap", normal);
				}
				mtlConstants.hasSpecLevelMap = (specular != nullptr);
				if (specular) {
					gApi->SetTexture(L"specLevelMap", specular);
				}
				// unused
				mtlConstants.hasGlossinessMap = false;
				mtlConstants.useCutout = true;				

				// deal with basic params
				mtlConstants.diffuseColor = submtl.diffuse;
				mtlConstants.glossiness = submtl.glossiness;
				mtlConstants.specularLevel = 0.3f;
			}
			else {
				// basic parameters
				mtlConstants.diffuseColor = Vec3(1.0f, 0.0f, 1.0f);
				mtlConstants.glossiness = 0.0f;
				mtlConstants.specularLevel = 0.0f;

				// texture properties
				mtlConstants.hasDiffuseMap = false;
				mtlConstants.hasNormalMap = false;
				mtlConstants.hasGlossinessMap = false;
				mtlConstants.hasSpecLevelMap = false;
				mtlConstants.useCutout = false;
			}

			gApi->SetVSConstantBuffer(&mtlConstants, sizeof(mtlConstants), 20);
			gApi->SetPSConstantBuffer(&mtlConstants, sizeof(mtlConstants), 20);

			// Foreach: Entity -> Draw this group
			for (auto& entity : group->entities) {
				// Entity world matrix
				Matrix44 worldMat = entity->GetWorldMatrix();

				// cbuffer
				struct {
					Matrix44 worldViewProj;
					Matrix44 worldView;
					Matrix44 world;
					Vec3 camPos;
					float near, far;
				} shaderTransform;
				shaderTransform.worldViewProj = worldMat * viewProjMat;
				shaderTransform.worldView = worldMat * viewMat;
				shaderTransform.world = worldMat;
				shaderTransform.camPos = cam->GetPos();
				shaderTransform.near = cam->GetNearPlane();
				shaderTransform.far = cam->GetFarPlane();

				gApi->SetVSConstantBuffer(&shaderTransform, sizeof(shaderTransform), 0);

				// draw
				gApi->DrawIndexed(matGroup.indexCount, matGroup.indexOffset);
			}
		}
	}

	// copy depth to shader resource
	gApi->CopyResource(depthBuffer, depthBufferCopy);

	//--------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- COMPOSITION PASS --- --- --- --- --- --- --- //
	//--------------------------------------------------------------------------//
	
	// Depth-stencil state
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


	//--------------------------------------------------------------------------//
	// --- --- --- --- --AMBIENT OCCLUSION (composition pass)-- --- --- --- --- //
	//--------------------------------------------------------------------------//

	// HBAO FUCK YEAH, I really like the small and smart constant buffer :D
	struct
	{
		float AOResolution[2];		float _pad0[2];
		float InvAOResolution[2];	float _pad1[2];

		float FocalLen[2];			float _pad2[2];

		float UVToViewA[2];			float _pad3[2];
		float UVToViewB[2];			float _pad4[2];

		float R;					float _pad5[3];
		float R2;					float _pad6[3];
		float NegInvR2;				float _pad7[3];
		float MaxRadiusPixels;		float _pad8[3];

		float AngleBias;			float _pad9[3];
		float TanAngleBias;			float _pad10[3];
		float PowExponent;			float _pad11[3];
		float Strength;				float _pad12[3];
	} data;

	data.AOResolution[0] = (float)ambientOcclusionBuffer->GetWidth();
	data.AOResolution[1] = (float)ambientOcclusionBuffer->GetHeight();
	data.InvAOResolution[0] = 1.0f / data.AOResolution[0];
	data.InvAOResolution[1] = 1.0f / data.AOResolution[1];

	// TODO
	const float fovYRad = cam->GetFOVRad() / cam->GetAspectRatio(); // HACKED ONE.......
	data.FocalLen[0] = 1.0f / tanf(fovYRad * 0.5f) * (data.AOResolution[1] / data.AOResolution[0]);
	data.FocalLen[1] = 1.0f / tanf(fovYRad * 0.5f);

	data.UVToViewA[0] = 2.0f * (1.0f / data.FocalLen[0]);
	data.UVToViewA[1] = -2.0f * (1.0f / data.FocalLen[1]);
	data.UVToViewB[0] = -1.0f * (1.0f / data.FocalLen[0]);
	data.UVToViewB[1] = 1.0f * (1.0f / data.FocalLen[1]);

	data.R = 0.05f; // WHOA RANDOM
	data.R2 = data.R * data.R;
	data.NegInvR2 = -1.0f / data.R2;
	data.MaxRadiusPixels = 0.1f * std::min(gApi->GetDefaultRenderTarget()->GetWidth(), gApi->GetDefaultRenderTarget()->GetHeight());
	data.AngleBias = 10; // TODO LOW GEOM FAILS
	data.TanAngleBias = tanf(data.AngleBias);
	data.PowExponent = 1; // TODO HACKED
	data.Strength = 1; // TODO HACKED

	gApi->SetShaderProgram(shaderHBAO);
	gApi->SetRenderTargets(1, &ambientOcclusionBuffer, NULL);

	// Need random texture, now using gpu garbage:D

	gApi->SetTexture(L"tLinearDepth", depthBufferCopy);
	gApi->SetPSConstantBuffer(&data, sizeof(data), 0);
	gApi->Draw(3);
	

	/*
	struct _aoShaderConstants {
		Matrix44 projMat;
		Matrix44 invViewProj;
		Vec3 camPos; float pad1;
	} aoShaderConstants;
	aoShaderConstants.projMat = projMat;
	aoShaderConstants.invViewProj = invViewProjMat;
	aoShaderConstants.camPos = cam->GetPos();

	gApi->SetShaderProgram(shaderSSAO);
	gApi->SetRenderTargets(1, &ambientOcclusionBuffer, NULL);
	gApi->SetTexture(L"normalTexture", gBuffer[1]);
	gApi->SetTexture(L"depthTexture", depthBufferCopy);
	gApi->SetPSConstantBuffer(&aoShaderConstants, sizeof(aoShaderConstants), 0);
	gApi->Draw(3);
	*/

	//--------------------------------------------------------------------------//
	// --- --- --- --- --- RENDER STATES (composition pass) --- --- --- --- --- //
	//--------------------------------------------------------------------------//
	
	// Additive blending state
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

	//-------------------------------------------------------------------------//
	// --- --- --- --- --LIGHTING PASS, in composition pass -- --- --- --- --- //
	//-------------------------------------------------------------------------//

	// Set render target
	gApi->SetRenderTargets(1, &compositionBuffer, depthBuffer);

	// Sort scene lights by type, apply validation of light params
	std::vector<std::pair<cGraphicsLight, cShadowMap>*> directionalLights;
	std::vector<std::pair<cGraphicsLight, cShadowMap>*> spotLights;
	std::vector<std::pair<cGraphicsLight, cShadowMap>*> pointLights;
	Vec3 ambientLight(0.0f, 0.0f, 0.0f);

	auto& lightList = parent.sceneManager->GetLights();
	for (auto light : lightList) {
		auto& light_ = light->first;
		if (!light_.enabled)
			continue;
		switch (light_.type) {
			case cGraphicsLight::AMBIENT:
				ambientLight += light_.color;
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
		Matrix44 viewProj;
		Matrix44 invViewProj;
		Vec3 camPos;		float _pad0;
		Vec3 lightColor;	float _pad1;
		Vec3 lightPos;		float _pad3;
		Vec3 lightDir;		float _pad4;
		float lightRange;	float _pad5[3];
		Vec3 lightAtten;	float _pad6;
		float lightAngleInner, lightAngleOuter;	float _pad7[2];
	} shaderConstants;
	float f = 2.0f;
	memset(&shaderConstants, *((int*)&f), sizeof(shaderConstants));

	Matrix44 viewProj = viewProjMat;
	shaderConstants.viewProj = viewProj;
	shaderConstants.invViewProj = viewProj;
	shaderConstants.invViewProj.Inverse();
	shaderConstants.camPos = cam->GetPos();
	Matrix44 test = shaderConstants.invViewProj * shaderConstants.viewProj;


	// --- --- RENDER EACH LIGHTGROUP --- --- // dir, spot, ambient, point

	//------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- COMPUTE SKY PARAMS --- --- --- --- --- --- //
	//------------------------------------------------------------------------//
	struct {
		Matrix44 invViewProj;
		Vec3 camPos; float _pad1;
		Vec3 sunDir; float _pad2;
		Vec3 sunColor; float _pad3;
		Vec3 horizonColor; float _pad4;
		Vec3 zenithColor; float _pad5;
		float rayleighFactor; float _pad6[3];
	} skyConstants;

	skyConstants.invViewProj = shaderConstants.invViewProj;
	skyConstants.camPos = cam->GetPos();
	skyConstants.sunDir = directionalLights.size() > 0 ? directionalLights[0]->first.direction : Vec3(0, 0, -1);

	IntensitySpectrum spectrum;
	spectrum.BlackBody(6400);
	spectrum.Scale(1.0f / spectrum[spectrum.Peak()]);
	float sunZenithAngle = acos(Dot(-skyConstants.sunDir, Vec3(0, 0, 1)));
	float airMass = RelativeAirMass((float)ZS_PI / 2.0f - sunZenithAngle) / RelativeAirMass(0);
	Rayleigh(spectrum, airMass*2.0f);
	Vec3 sunColor = spectrum.ToRGB();
	sunColor /= std::max(sunColor.x, std::max(sunColor.y, sunColor.z));
	Vec3 zenithColor = Vec3(0.5, 0.68, 0.9);
	Vec3 hazeColor = Vec3(0.8f, 0.7f, 0.68f);

	// normalize sky color params
	auto NormalizeColor = [](Vec3 c) {
		return c / (0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z);
	};
	sunColor = NormalizeColor(sunColor);
	zenithColor = NormalizeColor(zenithColor);
	hazeColor = NormalizeColor(hazeColor);

	skyConstants.sunColor = sunColor;
	skyConstants.horizonColor = (2.0f * sunColor + hazeColor) / 3.0f; // sunset
	skyConstants.zenithColor = zenithColor;
	skyConstants.rayleighFactor = 1.0f;	

	if (directionalLights.size() > 0) {
		directionalLights[0]->first.color = sunColor * 1.4f;
	}


	//-------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- DIRECTIONAL LIGHTS  --- --- --- --- --- --- //
	//-------------------------------------------------------------------------//
	gApi->SetShaderProgram(shaderDirectional);

	gApi->SetTexture(L"gBuffer0", gBuffer[0]);
	gApi->SetTexture(L"gBuffer1", gBuffer[1]);
	gApi->SetTexture(L"gBuffer2", gBuffer[2]);
	gApi->SetTexture(L"depthBuffer", depthBufferCopy);
	

	for (auto light_ : directionalLights) {
		auto& light = light_->first;
		auto& shadowMap = (cShadowMapDir&)light_->second;

		// load shader constants
		shaderConstants.lightColor = light.color;
		shaderConstants.lightDir = light.direction;
		gApi->SetPSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);

		// shadowmap constants
		struct {
			Matrix44 lightViewProj[12];
			int32_t nCascades;
			uint32_t castShadows;
		} shadowConst;

		// compute shadow map constants, if any
		if (shadowMap) {
			// get maps
			auto& transforms = shadowMap.GetTransforms();
			shadowConst.nCascades = shadowMap.GetNumCascades();
			shadowConst.castShadows = true;

			// for each cascade set shader values
			for (int i = 0; i < shadowConst.nCascades; i++) {
				shadowConst.lightViewProj[i] = transforms[i].viewMat * transforms[i].projMat;
			}
			// set shadow maps in shader
			gApi->SetTexture(L"shadowMap_Array", shadowMap.GetTexture());
		}
		else {
			shadowConst.castShadows = false;
		}

		gApi->SetPSConstantBuffer(&shadowConst, sizeof(shadowConst), 100);

		// draw an FSQ
		gApi->Draw(3);
	}


	//-------------------------------------------------------------------------//
	// --- --- --- --- --- --- ---  AMBIENT LIGHTS --- --- --- --- --- --- --- //
	//-------------------------------------------------------------------------//
	gApi->SetShaderProgram(shaderAmbient);

	gApi->SetTexture(L"gBuffer0", gBuffer[0]);
	gApi->SetTexture(L"gBuffer1", gBuffer[1]);
	gApi->SetTexture(L"gBuffer2", gBuffer[2]);
	gApi->SetTexture(L"depthBuffer", depthBufferCopy);
	gApi->SetTexture(L"ambientOcclusionTexture", ambientOcclusionBuffer);

	// load shader constants
	shaderConstants.lightColor = ambientLight;
	gApi->SetPSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);
	gApi->Draw(3); //FSQ


	//-------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- -- POINT LIGHTS --- --- --- --- --- --- --- //
	//-------------------------------------------------------------------------//
	gApi->SetShaderProgram(shaderPoint);

	gApi->SetTexture(L"gBuffer0", gBuffer[0]);
	gApi->SetTexture(L"gBuffer1", gBuffer[1]);
	gApi->SetTexture(L"gBuffer2", gBuffer[2]);
	gApi->SetTexture(L"depthBuffer", depthBufferCopy);

	gApi->SetVertexBuffer(vbPoint);
	gApi->SetIndexBuffer(ibPoint);

	for (auto light_ : pointLights) {
		auto& light = light_->first;

		// set shader constants
		shaderConstants.lightAtten = Vec3(light.atten0, light.atten1, light.atten2);
		shaderConstants.lightColor = light.color;
		shaderConstants.lightPos = light.position;
		shaderConstants.lightRange = light.range;
		gApi->SetVSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);
		gApi->SetPSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);

		// draw that bullshit
		size_t nIndices = ibPoint->GetSize() / sizeof(unsigned);
		gApi->DrawIndexed(nIndices);
	}

	//-------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- --- SPOT LIGHTS --- --- --- --- --- --- --- //
	//-------------------------------------------------------------------------//
	gApi->SetShaderProgram(shaderSpot);

	gApi->SetTexture(L"gBuffer0", gBuffer[0]);
	gApi->SetTexture(L"gBuffer1", gBuffer[1]);
	gApi->SetTexture(L"gBuffer2", gBuffer[2]);
	gApi->SetTexture(L"depthBuffer", depthBufferCopy);

	gApi->SetVertexBuffer(vbSpot);
	gApi->SetIndexBuffer(ibSpot);

	for (auto light_ : spotLights) {
		auto& light = light_->first;

		// set shader constants
		shaderConstants.lightAtten = Vec3(light.atten0, light.atten1, light.atten2);
		shaderConstants.lightColor = light.color;
		shaderConstants.lightPos = light.position;
		shaderConstants.lightRange = light.range;
		shaderConstants.lightAngleInner = light.smallAngle;
		shaderConstants.lightAngleOuter = light.bigAngle;
		shaderConstants.lightDir = light.direction;

		gApi->SetVSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);
		gApi->SetPSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);

		// draw that bullshit
		size_t nIndices = ibSpot->GetSize() / sizeof(unsigned);
		gApi->DrawIndexed(nIndices);
	}

	//-------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- --- --- SKY --- --- --- --- --- --- --- --- //
	//-------------------------------------------------------------------------//

	depthStencilState.stencilOpBackFace.stencilCompare = eComparisonFunc::EQUAL;
	depthStencilState.stencilOpFrontFace = depthStencilState.stencilOpBackFace;
	gApi->SetDepthStencilState(depthStencilState, 0x00);

	gApi->SetShaderProgram(shaderSky);
	gApi->SetVSConstantBuffer(&skyConstants, sizeof(skyConstants), 0);
	gApi->SetPSConstantBuffer(&skyConstants, sizeof(skyConstants), 0);
	
	gApi->Draw(3);

	// Set back render state to default
	gApi->SetDepthStencilState(depthStencilDefault, 0x00);
	gApi->SetBlendState(blendDefault);
}

// Access to composition buffer for further processing like post-process & whatever
ITexture2D* cGraphicsEngine::cDeferredRenderer::GetCompositionBuffer() {
	return compositionBuffer;
}

ITexture2D* cGraphicsEngine::cDeferredRenderer::GetDepthBuffer() {
	return depthBufferCopy;
}

ITexture2D* cGraphicsEngine::cDeferredRenderer::GetGBuffer(uint8_t idx) {
	// TODO Bullshit expectations, write array_size or utility somewhere
	return gBuffer[idx];
}

////////////////////////////////////////////////////////////////////////////////
//	Public usage
eGraphicsResult cGraphicsEngine::cDeferredRenderer::Resize(unsigned width, unsigned height) {
	auto bufferWidth_ = bufferWidth;
	auto bufferHeight_ = bufferHeight;

	// reallocate buffers w/ new size
	bufferWidth = width;
	bufferHeight = height;
	eGapiResult r = ReallocBuffers();
	// rollback on failure
	if (r != eGapiResult::OK) {
		bufferWidth = bufferWidth_;
		bufferHeight = bufferHeight_;
	}
	// analyze results and return
	if (r == eGapiResult::OK)
		return eGraphicsResult::OK;
	else if (r == eGapiResult::ERROR_OUT_OF_MEMORY)
		return eGraphicsResult::ERROR_OUT_OF_MEMORY;
	else
		return eGraphicsResult::ERROR_UNKNOWN;
}