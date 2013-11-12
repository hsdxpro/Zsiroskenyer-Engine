// Core.h By Zsíroskenyér Team 2013.10.25 23:16
// That class is above all Engine modules
// Like Graphics, Physics, Sound etc..
// Core can create things that need more modul to work together
#pragma once

#include "IPhysicsEngine.h"
#include "IGraphicsEngine.h"
#include "LogicEngine.h"

// For debug rendering
#include "Renderer.h"

#include "Transform3D.h"

#include <list>

class cEntity;

class cCore {
public:
	enum class eDebugRenderMode {
		PHYSICS_AABB		= 1 << 0,
		PHYSICS_TRIANGLES	= 1 << 1,
		PHYSICS_COLPOINTS	= 1 << 2,
	};

	~cCore();

	// Use eDebugRenderMode bit combinations
	void DebugRender(unsigned long renderFlags);

	void Update(float deltaT);

	cEntity* AddEntity(const zsString& graphGeomPath, const zsString& physicsGeom, const zsString& mtlPath, float mass);

	IGraphicsEngine* GetGraphicsEngine();
	IPhysicsEngine* GetPhysicsEngine();

	static cCore* GetInstance();

protected:
	cCore();
	static cCore* instance;

	// Modules
	IGraphicsEngine* graphicsEngine;
	IPhysicsEngine* physicsEngine;
	cLogicEngine*	logicEngine;

	// Debug renderer
	cRenderer* debugRenderer;

	// Entities from modules
	std::list<cGraphicsEntity*> graphicsEntities;
	std::list<IPhysicsEntity*> physicsEntities;

	// Link between modules
	struct tTransNode { 
		tTransNode(const cTransform3D& trans): trans(trans) {}
		const cTransform3D& trans;
		std::list<tTransNode> childs;
	};
	std::list<tTransNode> entityLinks;
};