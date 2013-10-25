// Entity.h By Zsíroskenyér Team 2013.10.25 23:08
// consist of cGraphicsEntity, cPhysicsEntity etc..
#pragma once

#include "GraphicsEntity.h"
#include "PhysicsEntity.h"

class cEntity {
protected:
	cGraphicsEntity* graphicsEntity;
	cGraphicsEntity* physicsEntity;
};