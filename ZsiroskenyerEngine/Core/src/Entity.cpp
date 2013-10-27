#include "Entity.h"

cEntity::cEntity(cGraphicsEntity* g, IPhysicsEntity* p)
:graphicsEntity(g), physicsEntity(p)
{
}

void cEntity::SetPosition(const Vec3& position) {
	graphicsEntity->position = position;
}

void cEntity::SetRotation(const Quat& rotation) {
	graphicsEntity->rotation = rotation;
}

void cEntity::SetScale(const Vec3& scale) {
	graphicsEntity->scale = scale;
}

void cEntity::setVisibility(bool visible) {
	graphicsEntity->isVisible = visible;
}