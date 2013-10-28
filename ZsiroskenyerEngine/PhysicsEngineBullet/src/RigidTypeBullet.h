// RigidTypeBullet.h By Zsíroskenyér Team 2013.10.28
// Bullet Physics implementation of Rigid physics type
#pragma once

#include "..\..\Core\src\IPhysicsType.h"

#include "BulletCollision\CollisionShapes\btCollisionShape.h"

class cRigidTypeBullet : public IPhysicsType {
public:
	cRigidTypeBullet(btCollisionShape* colShape, float mass);
protected:
	btCollisionShape *colShape;
	float mass;
};