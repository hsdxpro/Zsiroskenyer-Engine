// RigidEntityBullet.h By Zsíroskenyér Team 2013.10.28 10:26
#pragma once

#include "..\..\Core\src\IPhysicsEntity.h"
#include "BulletDynamics\Dynamics\btRigidBody.h"

class cRigidEntityBullet : public IPhysicsEntity {
public:
	void SetPosition(const Vec3& pos);
	void SetRotation(const Quat& rot);

	Vec3 GetPosition();
	Quat GetRotation();

	cRigidEntityBullet(btRigidBody* body);
	void Release() override;
protected:
	btRigidBody* body;
};