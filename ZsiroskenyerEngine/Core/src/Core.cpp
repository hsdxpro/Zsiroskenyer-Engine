#include "Core.h"

#include "common.h"
#include "Factory.h"

// TMP @TODO remove it
#include "Camera.h"
#include "Entity.h"

cCore* cCore::instance = NULL;

cCore* cCore::GetInstance() {
	if(cCore::instance == NULL)
		cCore::instance = new cCore();
	return cCore::instance;
}

cCore::cCore() {
	static_assert(false, "CSINALJ VALAMIT A CORE-RAL MERT: CreateGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config)");
	graphicsEngine = Factory.CreateGraphicsEngine();
	physicsEngine = Factory.CreatePhysicsEngineBullet();
	logicEngine = new cLogicEngine();

	// For debug purposes
	debugRenderer = new cRenderer(graphicsEngine->GetGraphicsApi(), graphicsEngine->GetShaderManager());
}

cCore::~cCore() {
	for(auto it = graphicsEntities.begin(); it != graphicsEntities.end(); it++)
		SAFE_DELETE(*it);

	for(auto it = physicsEntities.begin(); it != physicsEntities.end(); it++)
		SAFE_DELETE(*it);

	// Destroy engines
	SAFE_RELEASE(graphicsEngine);
	SAFE_RELEASE(physicsEngine);
	SAFE_DELETE(logicEngine);
}

void cCore::DebugRender(unsigned long renderFlags) {
	// Render Physics Triangles
	static Vec3 edges[400000]; // TODO THIS IS NOT THE BEST FOR YOU :D
	if(renderFlags & (unsigned long)eDebugRenderMode::PHYSICS_TRIANGLES) {
		size_t nEdges;
		physicsEngine->GetCollisionShapeEdges(edges, 400000, nEdges);

		if (nEdges != 0) {
			// Render lines for physics..
			cCamera* cam = graphicsEngine->GetActiveCamera();
			Matrix44 viewProj = cam->GetViewMatrix() * cam->GetProjMatrix();
			debugRenderer->RenderLines(viewProj, edges, nEdges, Vec3(1.0f, 0.0f, 0.0f));
		}
	}
}

void cCore::Update(float deltaT) {
	// Simulate physics world
	physicsEngine->SimulateWorld(deltaT);

	// Update Links
	// For each Root link
	for (auto it = entityLinks.begin(); it != entityLinks.end(); it++) {
		auto childs = it->childs;
	
		// For each child
		for (auto it2 = childs.begin(); it2 != childs.end(); it2++) {
			it2->entity->SetWorldTransform(it->entity);
		}
	}
}

cEntity* cCore::AddEntity(const zsString& graphGeomPath,const zsString& physicsGeom, const zsString& mtlPath, float mass) {
	// Create entity module parts
	cGraphicsEntity* gEntity = graphicsEngine->CreateEntity(graphGeomPath, mtlPath);
	IPhysicsEntity* pEntity = physicsEngine->CreateRigidEntity(physicsGeom, mass);

	// Node for physics
	tLinkNode nodeRootPhysics(pEntity);

	// Node for graphics
	tLinkNode nodeGraphics(gEntity);

	// Link them
	nodeRootPhysics.childs.push_back(nodeGraphics);
	entityLinks.push_back(nodeRootPhysics);
	
	graphicsEntities.push_back(gEntity);
	physicsEntities.push_back(pEntity);
	cEntity* e = new cEntity(gEntity, pEntity);
	return e;
}

IGraphicsEngine* cCore::GetGraphicsEngine() {
	return graphicsEngine;
}

IPhysicsEngine* cCore::GetPhysicsEngine() {
	return physicsEngine;
}
