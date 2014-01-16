////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Graphics Scene implementation
//	Not that much, but why put this to the engine.cpp?
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEngine.h"


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
cGraphicsEntity* cGraphicsScene::CreateEntity(const wchar_t* geomPath, const wchar_t* mtlPath) {
	cGeometryRef geom = parent.resourceManager->GetGeometry(geomPath);
	cMaterialRef mtl = parent.resourceManager->GetMaterial(mtlPath);

	if (!geom || !mtl)
		return NULL;

	return sceneManager.AddEntity(std::move(geom), std::move(mtl));
}
void cGraphicsScene::DeleteEntity(const cGraphicsEntity* entity) {
	sceneManager.RemoveEntity(entity);
}

//	Add/Remove scene lights
cGraphicsLight* cGraphicsScene::CreateLight() {
	return sceneManager.AddLight();
}
void cGraphicsScene::DeleteLight(const cGraphicsLight* light) {
	sceneManager.RemoveLight(light);
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