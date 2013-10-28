#include "Entity.h"

cEntity::cEntity(cGraphicsEntity* g, IPhysicsEntity* p)
:graphicsEntity(g), physicsEntity(p)
{
}

void cEntity::SetPosition(const Vec3& pos) {
	// Set Position for graphics and Physics to
	graphicsEntity->SetPosition(pos);
	physicsEntity->SetPosition(pos);
}

void cEntity::SetRotation(const Quat& rot) {
	// Set rotation for graphics and Physics to
	graphicsEntity->SetRotation(rot);
	physicsEntity->SetRotation(rot);
}

cGraphicsEntity* cEntity::GetGraphicsEntity() {
	return graphicsEntity;
}

IPhysicsEntity* cEntity::GetPhysicsEntity() {
	return physicsEntity;
}