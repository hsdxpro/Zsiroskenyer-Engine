////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "SceneManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////


#include "SceneManager.h"
#include "ResourceManager.h"
#include "InstanceGroup.h"
#include "../../Core/src/GraphicsEntity.h"
#include "../../Core/src/GraphicsLight.h"
#include "../../Core/src/Exception.h"
#include "../../Core/src/Camera.h"

#include <cassert>

//	Constructor & Destructor
cSceneManager::cSceneManager() {
}

cSceneManager::~cSceneManager() {
	Clear();
}


//	Add/Remove scene entities
cGraphicsEntity* cSceneManager::AddEntity(cGeometryRef geom, cMaterialRef mtl) {
	cInstanceGroup* instGroup = NULL;
	cInstanceGroup searchDummy;
	cGraphicsEntity* entity;

	try {
		searchDummy.geom = geom; searchDummy.mtl = mtl;

		auto it = instanceGroups.find(&searchDummy);
		// There is no instanceGroup, create it
		if (it == instanceGroups.end()) {
			instGroup = new cInstanceGroup(geom, mtl);
			instanceGroups.insert(instGroup);
		}
		// instanceGroup exists
		else {
			instGroup = *it;
		}

		entity = new cGraphicsEntity();
		entity->instanceGroup = instGroup;
		instGroup->entities.insert(entity);

		return entity;
	}
	catch (GraphicsEngineException&) {
		throw;
	}
}

void cSceneManager::RemoveEntity(const cGraphicsEntity* entity) {
	cInstanceGroup* instGroup = entity->instanceGroup;

	auto it = instanceGroups.find(instGroup);
	if (it==instanceGroups.end() || *it!=instGroup) { // only for debugging!
		assert(false);
		return;
	}

	instGroup->entities.erase(const_cast<cGraphicsEntity*>(entity));
	if (instGroup->entities.size() == 0) {
		instanceGroups.erase(instGroup);
	}

	delete entity;
}


// Add/Remove scene lights
cGraphicsLight* cSceneManager::AddLight() {
	cGraphicsLight* light = new cGraphicsLight();
	lights.insert(light);
	return light;
}
void cSceneManager::RemoveLight(const cGraphicsLight* light) {
	auto it = lights.find(const_cast<cGraphicsLight*>(light));
	if (it != lights.end()) {
		lights.erase(it);
	}
}

// Clear scene
void cSceneManager::Clear() {
	for (auto instGroup : instanceGroups) {
		for (auto entity : instGroup->entities) {
			delete entity;
		}
	}
	instanceGroups.clear();
}

//	Get entity list
const std::unordered_set<cInstanceGroup*, cInstGroupPtrHasher,cInstGroupPtrCompare>& cSceneManager::GetInstanceGroups() const {
	return instanceGroups;
}

// Get light list
const std::unordered_set<cGraphicsLight*>& cSceneManager::GetLights() const {
	return lights;
}
