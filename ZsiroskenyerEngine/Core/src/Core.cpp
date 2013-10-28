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

void cCore::Update(float deltaT) {
	physicsEngine->SimulateWorld(deltaT);
	logicEngine->Update();
}

cEntityType* cCore::CreateEntityType(const zsString& name, const zsString& graphGeomPath, const zsString& physGeomPath, const zsString& mtlPath, float mass /*= 0.0f*/) {
	cResourceManager* GRMgr = graphicsEngine->GetResourceManager();

	// Graphics and physics description
	cGeometryRef* gGeom = NULL;
	IPhysicsType* physicsType = NULL;

	const bool gGeomExists = GRMgr->IsGeometryExists(graphGeomPath);
	const bool pGeomExists = physicsEngine->IsGeometryExists(physGeomPath);

	// Geometry descriptors
	cGeometryBuilder::tGeometryDesc* gGeomDesc = NULL;
	cGeometryBuilder::tGeometryDesc* pGeomDesc = NULL;

	// Graphics Geometry or Physics geometry doesn't exists
	if(!(gGeomExists && pGeomExists)) {

		// Building geometries
		cGeometryBuilder builder;

		// Nem létezik fizikai geometria, és különbözik 
		if(! pGeomExists || graphGeomPath != physGeomPath)
			pGeomDesc = builder.LoadGeometryDAE(physGeomPath);

		// Create graphics geom desc
		if(! gGeomExists) {
			gGeomDesc = builder.LoadGeometryDAE(graphGeomPath);
			if(graphGeomPath == physGeomPath)
				pGeomDesc = gGeomDesc;
			else
				pGeomDesc = builder.LoadGeometryDAE(physGeomPath);
		// graphics geom exists
		} else {
			if(! pGeomExists) {
				pGeomDesc = builder.LoadGeometryDAE(physGeomPath);
			}
		}
	}
	
	gGeom = GRMgr->LoadGeometry(graphGeomPath, gGeomDesc);
	physicsType = physicsEngine->LoadRigidType(physGeomPath, mass, pGeomDesc);

	delete gGeomDesc;

	if(pGeomDesc != gGeomDesc)
		SAFE_DELETE(pGeomDesc);
	gGeomDesc = NULL;

	return logicEngine->CreateEntityType(name, gGeom, GRMgr->LoadMaterial(mtlPath), physicsType);
}

cEntity* cCore::AddEntity(cEntityType* type, const Vec3& position) {
	cGraphicsEntity* gEntity = graphicsEngine->GetSceneManager()->AddEntity(type->GetGraphicsGeometry(), type->GetMaterial());
	IPhysicsEntity* pEntity = physicsEngine->AddRigidEntity(type->GetPhysicsType(), position);

	cEntity* e = logicEngine->AddEntity(gEntity, pEntity);
		e->SetPosition(position);
	return e;
}

IGraphicsEngine* cCore::GetGraphicsEngine() {
	return graphicsEngine;
}

IPhysicsEngine* cCore::GetPhysicsEngine() {
	return physicsEngine;
}
