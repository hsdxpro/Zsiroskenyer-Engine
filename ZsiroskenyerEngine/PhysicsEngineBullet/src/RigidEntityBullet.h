// RigidEntityBullet.h By Zsíroskenyér Team 2013.10.28 10:26
#pragma once

#include "..\..\Core\src\IPhysicsEntity.h"
#include "BulletDynamics\Dynamics\btRigidBody.h"

class cRigidEntityBullet : public IPhysicsEntity {
public:
	cRigidEntityBullet(btRigidBody* body);
	void Release() override;

	void SetPos(const Vec3& p);
	void SetPos(float x, float y, float z);

	void SetRot(const Quat& r);
	void SetRot(float x, float y, float z, float w);

	void SetScale(const Vec3& s);
	void SetScale(float x, float y, float z);

protected:
	btRigidBody* body;
};