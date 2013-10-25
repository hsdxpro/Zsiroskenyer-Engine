// Core.h By Zsíroskenyér Team 2013.10.25 23:16
// That class is above all Engine modules
// Like Graphics, Physics, Sound etc..
// Core can create things that need more modul to work together
#pragma once

#include "IPhysicsEngine.h"
#include "IGraphicsEngine.h"

class cCore {
public:
	IGraphicsEngine* GetGraphicsEngine();
	IPhysicsEngine* GetPhysicsEngine();

	static cCore* GetInstance();
	~cCore();
protected:
	cCore(){}
	static cCore* instance;

	IGraphicsEngine *graphicsEngine;
	IPhysicsEngine* physicsEngine;
};