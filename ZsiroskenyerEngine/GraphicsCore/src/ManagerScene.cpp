////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "SceneManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////


#include "ManagerScene.h"
#include "ManagerResource.h"
#include "InstanceGroup.h"
#include "../../GraphicsEngine/src/Entity.h"
#include "../../GraphicsEngine/src/Exception.h"

#include <cassert>

using namespace std;


// constructor and destructor
cManagerScene::cManagerScene(cManagerResource& rm) : managerResource(rm) {
}
cManagerScene::~cManagerScene() {
}


// entities
cEntity& cManagerScene::AddEntity(const zsString& geometry, const zsString& material) {
	cInstanceGroup* instGroup = nullptr;
	cInstanceGroup searchDummy;
	cEntity* entity;

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

		entity = new cEntity();
		entity->instanceGroup = instGroup;
		instGroup->entities.insert(entity);

		return *entity;
	}
	catch (GraphicsEngineException&) {
		throw;
	}
}

void cManagerScene::RemoveEntity(const cEntity& entity) {
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

instanceGroupSet& cManagerScene::GetInstanceGroups() {
	return instanceGroups;
}