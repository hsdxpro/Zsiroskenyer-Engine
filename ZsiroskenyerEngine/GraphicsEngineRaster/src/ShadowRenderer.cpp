////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	This class processes a complete scene, and deals with shadow map creation
//	as a whole. 
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"
#include "SceneManager.h"
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
	auto& lights = sceneManager.GetLights();
	for (auto& v : lights) {
		auto& light = v->first;
		auto& shadowMap = v->second;

		switch (light.type) {
			case cGraphicsLight::DIRECTIONAL:{
				// set sh map type
				shadowMap.SetType(light.type);
				shadowMap.ClearUnneeded();
				// get map for type
				auto& shm = *(cShadowMapDir*)&shadowMap;
				// init map if not compatible with currently inited
				if (!shm.IsValid(gApi, 512, eFormat::R16_UNORM, eFormat::D16_UNORM, 1)) {
					shm.Init(gApi, 512, eFormat::R16_UNORM, eFormat::D16_UNORM, 1);
				}

				// foreach cascade
				for (size_t i = 0; i < shm.GetMaps().size(); i++) {
					// compute transforms
					auto& map = shm.GetMaps()[i];
					shm.Transform(map.projMat, map.viewMat, light.direction, parent.camera->GetViewMatrix(), parent.camera->GetProjMatrix());

					// setup render
					gApi->SetRenderTargets(1, NULL, map.texture);
					gApi->SetShaderProgram(shaderDirectional);

					// foreach inst grp
					for (auto& instgrp : sceneManager.GetInstanceGroups()) {
						// set geom params
						/*
						static_assert(false, 
							"\nNem lehet shadow mapet renderelni mert hasznalhatatlan a vertex format / vertex stride\n"
							"Amig nem fixeljuk, addig sehova... ki ne kommenteld ezt, xD Jólvan nem kommentelem :)!"							
						);
						*/
						gApi->SetVertexBuffer(instgrp->geom->GetVertexBuffer());
						gApi->SetIndexBuffer(instgrp->geom->GetIndexBuffer());

						// render objects
						for (auto& obj : instgrp->entities) {

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
// Deal with them shada'z
void cGraphicsEngine::cShadowRenderer::ReloadShaders() {

}

void cGraphicsEngine::cShadowRenderer::LoadShaders() {

}

void cGraphicsEngine::cShadowRenderer::UnloadShaders() {

}