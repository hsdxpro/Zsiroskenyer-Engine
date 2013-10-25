#include "Core.h"

#include "common.h"
#include "Factory.h"

cCore* cCore::instance = NULL;

cCore* cCore::GetInstance() {
	if(cCore::instance == NULL)
		cCore::instance = new cCore();
	return cCore::instance;
}

cCore::cCore() {
	graphicsEngine = Factory.CreateGraphicsEngine();
	physicsEngine = Factory.CreatePhysicsEngineBullet();
}

cCore::~cCore() {
	SAFE_RELEASE(graphicsEngine);
	SAFE_RELEASE(physicsEngine);
}

void cCore::AddEntity(const zsString& geomPath, const zsString& materialPath, float mass) {
	// Parse geom... add it to modules
	graphicsEngine->GetSceneManager()->AddEntity(geomPath, materialPath);
	//physicsEngine->AddEntity(mass);
}

IGraphicsEngine* cCore::GetGraphicsEngine() {
	return graphicsEngine;
}

IPhysicsEngine* cCore::GetPhysicsEngine() {
	return physicsEngine;
}
