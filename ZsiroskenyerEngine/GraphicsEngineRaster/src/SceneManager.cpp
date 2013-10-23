////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "SceneManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////


#include "SceneManager.h"
#include "ResourceManager.h"
#include "InstanceGroup.h"
#include "../../GraphicsEngine/src/GraphicsEntity.h"
#include "../../GraphicsEngine/src/Exception.h"
#include "../../GraphicsEngine/src/Camera.h"

#include <cassert>

using namespace std;


// constructor and destructor
cSceneManager::cSceneManager(cResourceManager& rm) : managerResource(rm), activeCamera(NULL) {
}
cSceneManager::~cSceneManager() {
}


// entities
cGraphicsEntity& cSceneManager::AddEntity(const zsString& geometry, const zsString& material) {
	cInstanceGroup* instGroup = nullptr;
	cInstanceGroup searchDummy;
	cGraphicsEntity* entity;

	try {
		cGeometryRef g = managerResource.LoadGeometry(geometry);
		cMaterialRef m = managerResource.LoadMaterial(material);
		searchDummy.geom = g; searchDummy.mtl = m;

		auto it = instanceGroups.find(&searchDummy);
		if (it==instanceGroups.end()) {
			instGroup = new cInstanceGroup(g,m);
			instanceGroups.insert(instGroup);
		}
		else {
			instGroup = *it;
		}

		entity = new cGraphicsEntity();
		entity->instanceGroup = instGroup;
		instGroup->entities.insert(entity);

		return *entity;
	}
	catch (GraphicsEngineException&) {
		throw;
	}
}

void cSceneManager::RemoveEntity(const cGraphicsEntity& entity) {
	cInstanceGroup* instGroup = entity.instanceGroup;

	auto it = instanceGroups.find(instGroup);
	if (it==instanceGroups.end() || *it!=instGroup) { // only for debugging!
		assert(false);
		return;
	}

	instGroup->entities.erase(const_cast<cGraphicsEntity*>(&entity));
	if (instGroup->entities.size()==0) {
		instanceGroups.erase(instGroup);
	}
}

void cSceneManager::SetActiveCamera(cCamera *cam) {
	activeCamera = cam;
}

cCamera *cSceneManager::GetActiveCamera() const {
	return activeCamera;
}

const std::unordered_set<cInstanceGroup*,	cInstGroupPtrHasher,cInstGroupPtrCompare>& cSceneManager::GetInstanceGroups() const {
	return instanceGroups;
}
