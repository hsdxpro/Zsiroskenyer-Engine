// RigidEntityBullet.h By Zsíroskenyér Team 2013.10.28 10:26
#pragma once

#include "..\..\Core\src\IPhysicsEntity.h"
#include "BulletDynamics\Dynamics\btRigidBody.h"

class cRigidEntityBullet : public IPhysicsEntity {
public:
	void SetPos(const Vec3& pos) override;
	void SetRotation(const Quat& rot) override;

	Vec3 GetPos() override;
	Quat GetRotation() override;

	cRigidEntityBullet(btRigidBody* body);
	void Release() override;
protected:
	btRigidBody* body;
};