#include "Core.h"

#include "common.h"
#include "Factory.h"

#include "GeometryBuilder.h"

cCore* cCore::instance = NULL;

cCore* cCore::GetInstance() {
	if(cCore::instance == NULL)
		cCore::instance = new cCore();
	return cCore::instance;
}

cCore::cCore() {
	graphicsEngine = Factory.CreateGraphicsEngine();
	physicsEngine = Factory.CreatePhysicsEngineBullet();
	logicEngine = new cLogicEngine();
}

cCore::~cCore() {
	SAFE_RELEASE(graphicsEngine);
	SAFE_RELEASE(physicsEngine);
	SAFE_DELETE(logicEngine);
}

cEntityType* cCore::CreateEntityType(const zsString& typeName, const zsString& physGraphGeomPath, const zsString& materialPath, float mass /*= 0.0f*/) {
	/*
	// EntityType doesn't exists
	if(! logicEngine->IsEntityTypeExits(typeName)) {
		cResourceManager* graphicsRMgr = graphicsEngine->GetResourceManager();
//		cResourceManager* graphicsRMgr = graphicsEngine->GetResourceManager();

		// Graphics geometry doesn't exists
		if(! graphicsRMgr->IsGeometryExists(physGraphGeomPath)) {
			cGeometryBuilder builder;
			cGeometryBuilder::tGeometryDesc desc = builder.LoadGeometryDAE(physGraphGeomPath);
			graphicsRMgr->LoadGeometry(physGraphGeomPath, desc);
			if(! physicsEngine->IsGeometryExists(physGraphGeomPath)) {
				physicsEngine->LoadGeometry(physGraphGeomPath, desc);
			}
		}

		// Minden csak teszt jellegû...
		// LogicEngine -> Create EntityType....
	}
	*/
	return NULL;
}

cEntity* cCore::AddEntity(cEntityType* type, const Vec3& position) {
	return NULL;
}

IGraphicsEngine* cCore::GetGraphicsEngine() {
	return graphicsEngine;
}

IPhysicsEngine* cCore::GetPhysicsEngine() {
	return physicsEngine;
}
