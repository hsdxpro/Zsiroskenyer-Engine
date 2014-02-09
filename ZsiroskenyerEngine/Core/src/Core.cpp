#include "Core.h"

#include "common.h"
#include "Factory.h"
#include "CreateGraphicsEngine.h"

// TMP @TODO remove it
#include "Camera.h"
#include "Entity.h"

#include <Windows.h> // remove this or I kill myself
					// btw, it's for graphics engine error message MessageBox()

cCore::cCore(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config) {
	// try to create graphics engine
	bool tryCreateGEngine = true;
	while (tryCreateGEngine) {
		const char* graphicsEngineError;
		graphicsEngine = CreateGraphicsEngine(targetWindow, screenWidth, screenHeight, config, &graphicsEngineError);
		if (!graphicsEngine) {
			int choice;
			choice = MessageBoxA(NULL, 
				graphicsEngineError, 
				"Failed to create graphics engine", 
				MB_RETRYCANCEL | MB_ICONERROR);
			switch (choice) {
				case IDRETRY:
					tryCreateGEngine = true;
					break;
				case IDOK:
				case IDCANCEL:
				default:
					MessageBoxA(NULL,
						"THE PROGRAM WILL NOW FORCE-QUIT",
						"Couldn't create Graphics Engine",
						MB_ICONWARNING | MB_OK);
					exit(1);
					break;
			}
		}
		else {
			tryCreateGEngine = false;
		}
	}

	// create other stuff
	physicsEngine = Factory.CreatePhysicsEngineBullet();
	logicEngine = new cLogicEngine();

	// For debug purposes
#pragma message("WARNING: IShaderManager wiped completely!")
	debugRenderer = new cRenderer(graphicsEngine->GetGraphicsApi(), nullptr);
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

void cCore::DebugRender(IGraphicsScene* s, unsigned long renderFlags) {
	// Render Physics Triangles
	static Vec3 edges[400000]; // TODO THIS IS NOT THE BEST FOR YOU :D
	if(renderFlags & (unsigned long)eDebugRenderMode::PHYSICS_TRIANGLES) {
		size_t nEdges;
		physicsEngine->GetCollisionShapeEdges(edges, 400000, nEdges);

		if (nEdges != 0) {
			// Render lines for physics..
			const cCamera& cam = s->GetCamera();
			Matrix44 viewProj = cam.GetViewMatrix() * cam.GetProjMatrix();
			debugRenderer->RenderLines(viewProj, edges, nEdges, Vec3(1.0f, 0.0f, 0.0f));
		}
	}
}

void cCore::Update(float deltaT) {
	// Simulate physics world
	physicsEngine->SimulateWorld(deltaT);

	// Update Links
	for (size_t i = 0; i < entityLinks.size(); i++)
	{
		UpdateChildLinksRecursively(entityLinks[i]);
	}

	graphicsEngine->Update(deltaT);
}

void cCore::UpdateChildLinksRecursively(cCore::tLinkNode& n) {
	for (size_t i = 0; i < n.childs.size(); i++) {
		n.childs[i].entity->Copy(n.entity);
		UpdateChildLinksRecursively(n.childs[i]);
	}
}

cEntity* cCore::AddEntity(IGraphicsScene* s, const zsString& graphGeomPath,const zsString& physicsGeom, const zsString& mtlPath, float mass, bool soft /*= false*/ ) {
	// Create entity module parts
	IGraphicsEntity* gEntity = s->CreateEntity(graphGeomPath.c_str(), mtlPath.c_str());
	IPhysicsEntity* pEntity;

	if (soft)
		pEntity = physicsEngine->CreateSoftEntity(physicsGeom, mass);
	else
		pEntity = physicsEngine->CreateRigidEntity(physicsGeom, mass);

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
