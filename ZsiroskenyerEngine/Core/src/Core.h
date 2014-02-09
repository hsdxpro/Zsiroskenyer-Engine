// Core.h By Zsíroskenyér Team 2013.10.25 23:16
// That class is above all Engine modules
// Like Graphics, Physics, Sound etc..
// Core can create things that need more modul to work together
#pragma once

#include "IPhysicsEngine.h"
#include "IGraphicsEngine.h"
#include "LogicEngine.h"

#include "ITransformable3D.h"
#include "Transformable.h"

// For debug rendering
#include "Renderer.h"

#include <list>

class cEntity;

class cCore {
public:
	enum class eDebugRenderMode {
		PHYSICS_AABB		= 1 << 0,
		PHYSICS_TRIANGLES	= 1 << 1,
		PHYSICS_COLPOINTS	= 1 << 2,
	};

	// Link between modules
	struct tLinkNode {
		tLinkNode(cTransformable entity)
		:entity(entity) {}

		// Transformable entity
		cTransformable entity;

		std::vector<tLinkNode> childs;
	};

	cCore(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config);
	~cCore();

	// Use eDebugRenderMode bit combinations
	void DebugRender(IGraphicsScene* s, unsigned long renderFlags);

	void Update(float deltaT);

	cEntity* AddEntity(IGraphicsScene* s,const zsString& graphGeomPath, const zsString& physicsGeom, const zsString& mtlPath, float mass, bool soft = false);

	IGraphicsEngine* GetGraphicsEngine();
	IPhysicsEngine* GetPhysicsEngine();
protected:
	void UpdateChildLinksRecursively(cCore::tLinkNode& n);
protected:
	// Modules
	IGraphicsEngine* graphicsEngine;
	IPhysicsEngine* physicsEngine;
	cLogicEngine*	logicEngine;

	// Debug renderer
	cRenderer* debugRenderer;

	// Entities from modules
	std::list<IGraphicsEntity*> graphicsEntities;
	std::list<IPhysicsEntity*> physicsEntities;

	std::vector<cCore::tLinkNode> entityLinks;
};