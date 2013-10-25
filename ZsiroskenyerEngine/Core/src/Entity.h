// Entity.h By Zsíroskenyér Team 2013.10.25 23:08
// consist of cGraphicsEntity, cPhysicsEntity etc..
#pragma once

#include "GraphicsEntity.h"
#include "PhysicsEntity.h"

class cEntity {
public:
protected:
	cGraphicsEntity* graphicsEntity;
	cPhysicsEntity* physicsEntity;
};