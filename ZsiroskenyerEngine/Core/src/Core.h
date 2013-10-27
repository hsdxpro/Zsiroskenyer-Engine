// Core.h By Zsíroskenyér Team 2013.10.25 23:16
// That class is above all Engine modules
// Like Graphics, Physics, Sound etc..
// Core can create things that need more modul to work together
#pragma once

#include "IPhysicsEngine.h"
#include "IGraphicsEngine.h"
#include "LogicEngine.h"

class cCore {
public:
	void Update(float deltaT);

	cEntityType* CreateEntityType(const zsString& name, const zsString& physGraphGeomPath, const zsString& mtlPath, float mass = 0.0f);
	cEntity* AddEntity(cEntityType* type, const Vec3& position);

	IGraphicsEngine* GetGraphicsEngine();
	IPhysicsEngine* GetPhysicsEngine();

	static cCore* GetInstance();
	~cCore();
protected:
	cCore();
	static cCore* instance;

	IGraphicsEngine *graphicsEngine;
	IPhysicsEngine* physicsEngine;
	cLogicEngine*	logicEngine;
};