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
	for(auto it = graphicsEntities.begin(); it != graphicsEntities.end(); it++)
		SAFE_DELETE(*it);

	for(auto it = physicsEntities.begin(); it != physicsEntities.end(); it++)
		SAFE_DELETE(*it);

	for(auto it = entities.begin(); it != entities.end(); it++)
		SAFE_DELETE(*it);

	// Destroy engines
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
	Vec3* edges = new Vec3[1000];
	size_t nEdges;
	physicsEngine->GetCollisionShapeEdges(edges, 1000, nEdges);

	// The graphicsEngine can do it...

	graphicsEngine->RenderLines(edges, nEdges, Vec3(1.0f, 0.0f, 0.0f));

	delete[] edges;
}

void cCore::Update(float deltaT) {
	physicsEngine->SimulateWorld(deltaT);
}

cEntity* cCore::AddEntity(const zsString& graphGeomPath,const zsString& physicsGeom, const zsString& mtlPath, float mass) {
	cGraphicsEntity* gEntity = graphicsEngine->GetGraphicsEntity(graphGeomPath, mtlPath);
	IPhysicsEntity* pEntity = physicsEngine->GetRigidEntity(physicsGeom, mass);

	graphicsEntities.push_back(gEntity);
	physicsEntities.push_back(pEntity);
	cEntity* e = new cEntity(gEntity, pEntity);
	entities.push_back(e);
	return e;
}

IGraphicsEngine* cCore::GetGraphicsEngine() {
	return graphicsEngine;
}

IPhysicsEngine* cCore::GetPhysicsEngine() {
	return physicsEngine;
}
