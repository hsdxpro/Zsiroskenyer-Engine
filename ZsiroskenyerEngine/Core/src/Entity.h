// Entity.h By Zsíroskenyér Team 2013.10.25 23:08
// consist of cGraphicsEntity, cPhysicsEntity etc..
#pragma once

#include "GraphicsEntity.h"
#include "IPhysicsEntity.h"

class cEntity {
public:
	void SetPosition(const Vec3& position);
	void SetRotation(const Quat& rotation);
	void SetScale(const Vec3& scale);
	void setVisibility(bool visible);

	cEntity(cGraphicsEntity* g, IPhysicsEntity* p);
protected:
	cGraphicsEntity* graphicsEntity;
	IPhysicsEntity* physicsEntity;
};