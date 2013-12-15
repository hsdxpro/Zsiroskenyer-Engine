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
	camera(70.0f/180.f*3.1415926f, 1.0f, 0.5f, 2000.0f),
	luminanceAdaptation(0.0f)
{
}

cGraphicsScene::~cGraphicsScene() {
}


//	Add/Remove scene entities
cGraphicsEntity* cGraphicsScene::CreateEntity(const zsString& geomPath, const zsString& mtlPath) {
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
	throw std::runtime_error("azt hitted mi? :D");
}
void cGraphicsScene::DeleteLight(const cGraphicsLight* light) {

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
