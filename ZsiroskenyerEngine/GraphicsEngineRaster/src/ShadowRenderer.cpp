////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	This class processes a complete scene, and deals with shadow map creation
//	as a whole. 
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "SceneManager.h"
#include "GraphicsEntity.h"
#include "../../Core/src/GAPI.h"
#include <stdexcept>


////////////////////////////////////////////////////////////////////////////////
// Constructor
cGraphicsEngine::cShadowRenderer::cShadowRenderer(cGraphicsEngine& parent)
:
parent(parent),
gApi(parent.gApi),
shaderDirectional(nullptr),
shaderPoint(nullptr),
shaderSpot(nullptr)
{
	try {
		LoadShaders();
	}
	catch (std::exception& e) {
		throw std::runtime_error(std::string("failed to create shaders: ") + e.what());
	}
}

cGraphicsEngine::cShadowRenderer::~cShadowRenderer() {
	Cleanup();
}

void cGraphicsEngine::cShadowRenderer::Cleanup() {
	SAFE_RELEASE(shaderDirectional);
	SAFE_RELEASE(shaderPoint);
	SAFE_RELEASE(shaderSpot);
}


////////////////////////////////////////////////////////////////////////////////
// Render them shadowmapz
void cGraphicsEngine::cShadowRenderer::RenderShadowMaps(cSceneManager& sceneManager) {
	// set states
	tBlendDesc blend = blendDefault;
	blend[0].writeMask = 0;
	gApi->SetBlendState(blend);

	tDepthStencilDesc dsState = depthStencilDefault;
	dsState.depthCompare = eComparisonFunc::LESS;
	gApi->SetDepthStencilState(dsState, 0x00);

	// render lights
	auto& lights = sceneManager.GetLights();
	for (auto& v : lights) {
		auto& light = v->first;
		auto& shadowMap = v->second;
		if (!light.enabled)
			continue;

		switch (light.type) {
			case cGraphicsLight::DIRECTIONAL:{
				// set sh map type
				shadowMap.SetType(light.type);
				shadowMap.ClearUnneeded();
				// get map for type
				auto& shm = *(cShadowMapDir*)&shadowMap;
				// init map if not compatible with currently inited
				try {
					if (!shm.IsValid(gApi, 2048, eFormat::R16_UNORM, eFormat::D16_UNORM, 5)) {
						shm.Init(gApi, 2048, eFormat::R16_UNORM, eFormat::D16_UNORM, 5);
					}
				}
				catch (std::exception& e) {
					std::cerr << e.what() << std::endl;
					break; // breaks switch case label
				}

				// generate cascade splits
				size_t nCascades = shm.GetMaps().size();
				std::vector<float> cascadeSplits(nCascades + 1, 0.0f);

				float near = parent.camera->GetNearPlane();
				float far = parent.camera->GetFarPlane();

				for (size_t i = 0; i <= nCascades; i++) {
					cascadeSplits[i] = near*pow((far / near), float(i) / float(nCascades));
				}
				for (float& v : cascadeSplits) {
					v -= near;
					v /= (far - near);
				}

				// foreach cascade
				for (size_t i = 0; i < nCascades; i++) {
					// compute transforms
					auto& map = shm.GetMaps()[i];
					bool isGoodTransform = shm.Transform(
							map.projMat, 
							map.viewMat, 
							light.direction, 
							parent.camera->GetViewMatrix(), parent.camera->GetProjMatrix(), 
							cascadeSplits[i], cascadeSplits[i+1]);
					if (!isGoodTransform)
						continue;

					// setup render
					gApi->SetShaderProgram(shaderDirectional);
					gApi->SetRenderTargets(0, nullptr, map.texture);
					gApi->ClearTexture(map.texture);

					// foreach inst grp
					for (auto& instgrp : sceneManager.GetInstanceGroups()) {
						// set geom params
						gApi->SetVertexBuffer(instgrp->geom->GetVertexBuffer());
						gApi->SetIndexBuffer(instgrp->geom->GetIndexBuffer());

						// render objects
						for (auto& entity : instgrp->entities) {
							Matrix44 matWorld = entity->GetWorldMatrix();
							Matrix44 worldViewProj = matWorld * map.viewMat * map.projMat;
							gApi->SetVSConstantBuffer(&worldViewProj, sizeof(worldViewProj), 0);
							gApi->DrawIndexed(instgrp->geom->GetIndexBuffer()->GetSize() / 4);
						}
					}
				}
				break;
			}
			case cGraphicsLight::SPOT:{

				break;
			}
			case cGraphicsLight::POINT:{

				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Deal with the shaders
void cGraphicsEngine::cShadowRenderer::ReloadShaders() {
	auto Reload = [this](IShaderProgram** prog, const wchar_t* name)->void {
		IShaderProgram* tmp = SafeLoadShader(gApi, name); // it throws on error!
		(*prog)->Release();
		*prog = tmp;
	};
	Reload(&shaderDirectional, L"shaders/shadowmap_render_dir.cg");
}

void cGraphicsEngine::cShadowRenderer::LoadShaders() {
	auto Create = [this](const wchar_t* shader)->IShaderProgram* {
		return SafeLoadShader(gApi, shader);
	};
	try {
		shaderDirectional = Create(L"shaders/shadowmap_render_dir.cg");
	}
	catch (...) {
		UnloadShaders();
		throw;
	}
}

void cGraphicsEngine::cShadowRenderer::UnloadShaders() {
	SAFE_RELEASE(shaderDirectional);
}