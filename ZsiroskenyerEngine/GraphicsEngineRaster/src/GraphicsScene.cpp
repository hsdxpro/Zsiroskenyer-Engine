////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Graphics Scene implementation
//	Not that much, but why put this to the engine.cpp?
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEngine.h"
#include "UserGeometry.h"
#include "GraphicsEntity.h"
#include "GraphicsLight.h"


//	Constructor & destructor
cGraphicsScene::cGraphicsScene(cGraphicsEngine& parent, tRenderState state) 
:	state(state), parent(parent),
	camera(cCamera::tProjPersp(70.0f/180.f*3.1415926f, 1.0f), 0.5f, 2000.0f),
	luminanceAdaptation(0.0f)
{ 
}

cGraphicsScene::~cGraphicsScene() {
}


//	Add/Remove scene entities
IGraphicsEntity* cGraphicsScene::CreateEntity(const wchar_t* geomPath, const wchar_t* mtlPath) {
	cGeometryRef geom = parent.resourceManager->GetGeometry(geomPath);
	cMaterialRef mtl = parent.resourceManager->GetMaterial(mtlPath);

	if (!geom || !mtl)
		return nullptr;

	return sceneManager.AddEntity(std::move(geom), std::move(mtl));
}
IGraphicsEntity* cGraphicsScene::CreateEntity(IGeometryRef* customGeom, const wchar_t* mtlPath) {
	cGeometryRef geom = *(cGeometryRef*)customGeom;
	cMaterialRef mtl = parent.resourceManager->GetMaterial(mtlPath);

	if (!geom || !mtl)
		return nullptr;

	return sceneManager.AddEntity(std::move(geom), std::move(mtl));
}
void cGraphicsScene::DeleteEntity(const IGraphicsEntity* entity) {
	sceneManager.RemoveEntity(static_cast<const cGraphicsEntity*>(entity));
}

//	Add/Remove scene lights
IGraphicsLight* cGraphicsScene::CreateLight() {
	return &(sceneManager.AddLight()->first);
}
void cGraphicsScene::DeleteLight(const IGraphicsLight* light) {
	auto light_ = static_cast<const cGraphicsLight*>(light);
	using T = std::pair<cGraphicsLight, cShadowMap>;
	sceneManager.RemoveLight(reinterpret_cast<T*>(size_t(light_)-offsetof(T,first)));
}

//	Interface
cCamera& cGraphicsScene::GetCamera() {
	return camera;
}

tRenderState& cGraphicsScene::GetState() {
	return state;
}

void cGraphicsScene::Clear() {
	sceneManager.Clear();
}



//	Scene order
void cGraphicsScene::MoveUp() {
	
	for (auto it = parent.graphicsSceneOrder.begin();
		it != parent.graphicsSceneOrder.end();
		it++)
	{
		if (*it == this) {
			if (it+1 != parent.graphicsSceneOrder.end()) {
				auto itNext = it + 1;
				auto tmp = *itNext;
				*itNext = *it;
				*it = tmp;
			}
			break;
		}
	}
}
void cGraphicsScene::MoveDown() {
	for (auto it = parent.graphicsSceneOrder.begin();
		it != parent.graphicsSceneOrder.end();
		it++)
	{
		if (*it == this) {
			if (it != parent.graphicsSceneOrder.begin()) {
				auto itPrev = it - 1;
				auto tmp = *itPrev;
				*itPrev = *it;
				*it = tmp;
			}
		}
	}
}
void cGraphicsScene::MoveTop() {
	for (auto it = parent.graphicsSceneOrder.begin();
		it != parent.graphicsSceneOrder.end();
		it++)
	{
		if (*it == this) {
			parent.graphicsSceneOrder.erase(it);
			parent.graphicsSceneOrder.push_back(this);
		}
	}
}
void cGraphicsScene::MoveBottom() {
	for (auto it = parent.graphicsSceneOrder.begin();
		it != parent.graphicsSceneOrder.end();
		it++)
	{
		if (*it == this) {
			parent.graphicsSceneOrder.erase(it);
			parent.graphicsSceneOrder.push_front(this);
		}
	}
}