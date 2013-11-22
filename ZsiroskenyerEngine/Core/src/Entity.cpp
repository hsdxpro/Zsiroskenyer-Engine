#include "Entity.h"

cEntity::cEntity(cGraphicsEntity* g, IPhysicsEntity* p)
:graphicsEntity(g), physicsEntity(p) {
}

void cEntity::SetPos(const Vec3& pos) {
	// Set Position for graphics and Physics to
	graphicsEntity->SetPos(pos);
	physicsEntity->SetPos(pos);
}

void cEntity::SetRot(const Quat& rot) {
	// Set rotation for graphics and Physics to
	graphicsEntity->SetRot(rot);
	physicsEntity->SetRot(rot);
}

void cEntity::SetScale(const Vec3& scale) {
	graphicsEntity->SetScale(scale);
	physicsEntity->SetScale(scale);
}

const Vec3& cEntity::GetPos() {
	return physicsEntity->GetPos();
}

cGraphicsEntity* cEntity::GetGraphicsEntity() {
	return graphicsEntity;
}

IPhysicsEntity* cEntity::GetPhysicsEntity() {
	return physicsEntity;
}