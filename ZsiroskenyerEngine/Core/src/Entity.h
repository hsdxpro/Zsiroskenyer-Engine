// Entity.h By Zs�roskeny�r Team 2013.10.25 23:08
// consist of cGraphicsEntity, cPhysicsEntity etc..
#pragma once

#include "GraphicsEntity.h"
#include "IPhysicsEntity.h"

class cEntity {
public:
	cEntity(cGraphicsEntity* g, IPhysicsEntity* p);
protected:
	cGraphicsEntity* graphicsEntity;
	IPhysicsEntity* physicsEntity;
};