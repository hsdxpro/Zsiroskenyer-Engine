// Entity.h By Zsíroskenyér Team 2013.10.25 23:08
// consist of cGraphicsEntity, cPhysicsEntity etc..
#pragma once

#include "GraphicsEntity.h"
#include "IPhysicsEntity.h"

class cEntity {
public:
protected:
	cGraphicsEntity* graphicsEntity;
	IPhysicsEntity* physicsEntity;
};