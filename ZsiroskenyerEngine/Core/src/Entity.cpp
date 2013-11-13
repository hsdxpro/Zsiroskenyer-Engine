#include "Entity.h"

cEntity::cEntity(cGraphicsEntity* g, IPhysicsEntity* p)
:graphicsEntity(g), physicsEntity(p)
{
}

void cEntity::SetPos(const Vec3& pos) {
	// Set Position for graphics and Physics to
	((IBaseEntity*)graphicsEntity)->SetPos(pos);
	((IBaseEntity*)physicsEntity)->SetPos(pos);
}

void cEntity::SetRot(const Quat& rot) {
	// Set rotation for graphics and Physics to
	((IBaseEntity*)graphicsEntity)->SetRot(rot);
	((IBaseEntity*)physicsEntity)->SetRot(rot);
}

cGraphicsEntity* cEntity::GetGraphicsEntity() {
	return graphicsEntity;
}

IPhysicsEntity* cEntity::GetPhysicsEntity() {
	return physicsEntity;
}