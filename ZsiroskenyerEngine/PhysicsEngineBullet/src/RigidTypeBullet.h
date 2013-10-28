// RigidTypeBullet.h By Zsíroskenyér Team 2013.10.28
// Bullet Physics implementation of Rigid physics type
#pragma once

#include "..\..\Core\src\IPhysicsType.h"

#include "BulletCollision\CollisionShapes\btCollisionShape.h"

class cRigidTypeBullet : public IPhysicsType {
public:
	btCollisionShape* GetCollisionShape();
	float GetMass() const;
	const btVector3& GetLocalInertia() const;

	cRigidTypeBullet(btCollisionShape* colShape, float mass);
	void Release() override;
protected:
	btCollisionShape *colShape;
	btVector3 localInertia;
	float mass;
};