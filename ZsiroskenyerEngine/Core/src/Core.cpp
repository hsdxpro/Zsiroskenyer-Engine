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

void cCore::DebugRender(unsigned long renderFlags) {
	//std::list<Vec3> edgeList = physicsEngine->GetCollisionShapeEdges();
	std::list<Vec3> asdTeszt;
	asdTeszt.push_back(Vec3(0,0,0));
}

void cCore::Update(float deltaT) {
	physicsEngine->SimulateWorld(deltaT);
	logicEngine->Update();
}

cEntityType* cCore::CreateEntityType(const zsString& name, const zsString& graphGeomPath, const zsString& physGeomPath, const zsString& mtlPath, float mass /*= 0.0f*/) {
	cResourceManager* GRMgr = graphicsEngine->GetResourceManager();
	cGeometryRef* geom = GRMgr->GetGeometry(graphGeomPath);
	cMaterialRef* mtl = GRMgr->GetMaterial(mtlPath);
	IPhysicsType* physicsType = physicsEngine->GetRigidType(physGeomPath, mass);

	return logicEngine->CreateEntityType(name, geom, mtl, physicsType);
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
