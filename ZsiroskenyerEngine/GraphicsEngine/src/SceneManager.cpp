////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "SceneManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////


#include "SceneManager.h"
#include "ResourceManager.h"
#include "InstanceGroup.h"
#include "Entity.h"
#include "Exception.h"

#include <cassert>

using namespace std;


// constructor and destructor
cSceneManager::cSceneManager(cResourceManager& rm) : resourceManager(rm) {
}
cSceneManager::~cSceneManager() {
}


// entities
cEntity& cSceneManager::AddEntity(const zsString& geometry, const zsString& material) {
	cInstanceGroup* instGroup = nullptr;
	cEntity* entity;

	try {
		cGeometryRef g = resourceManager.LoadGeometry(geometry);
		cMaterialRef m = resourceManager.LoadMaterial(material);
		instGroup = new cInstanceGroup(g,m);
		auto insertResult = instanceGroups.insert(instGroup);
		if (insertResult.second==true) {
			delete instGroup;
		}
		instGroup = *insertResult.first;

		entity = new cEntity();
		entity->instanceGroup = instGroup;
		instGroup->entities.insert(entity);

		return *entity;
	}
	catch (GraphicsEngineException&) {
		throw;
	}
}

void cSceneManager::RemoveEntity(const cEntity& entity) {
	cInstanceGroup* instGroup = entity.instanceGroup;

	auto it = instanceGroups.find(instGroup);
	if (it==instanceGroups.end() || *it!=instGroup) { // only for debugging!
		assert(false);
		return;
	}

	instGroup->entities.erase(const_cast<cEntity*>(&entity));
	if (instGroup->entities.size()==0) {
		instanceGroups.erase(instGroup);
	}
}




