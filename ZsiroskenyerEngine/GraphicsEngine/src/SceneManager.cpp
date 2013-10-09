////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "SceneManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////


#include "SceneManager.h"
#include "ResourceManager.h"
#include "InstanceGroup.h"

using namespace std;


// constructor and destructor
cSceneManager::cSceneManager(cResourceManager& rm) : resourceManager(rm) {
}
cSceneManager::~cSceneManager() {
}


// entities
Entity& cSceneManager::AddEntity(const std::string& geometry, const std::string& material) {
	cInstanceGroup* instGroup = nullptr;
	try {
		cGeometryRef g = resourceManager.LoadGeometry(geometry);
		cMaterialRef m = resourceManager.LoadMaterial(material);
		instGroup = new cInstanceGroup(g,m);
		instanceGroups.insert(instGroup);
	}
	catch (exception& e) {
		throw;
	}
}

void cSceneManager::RemoveEntity(const Entity& entity) {

}




