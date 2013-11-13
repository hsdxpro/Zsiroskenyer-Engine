// RigidEntityBullet.h By Zsíroskenyér Team 2013.10.28 10:26
#pragma once

#include "..\..\Core\src\IPhysicsEntity.h"
#include "BulletDynamics\Dynamics\btRigidBody.h"

class cRigidEntityBullet : public IPhysicsEntity {
public:
	cRigidEntityBullet(btRigidBody* body);
	void Release() override;

	void SetPos(float x, float y, float z) override;
	void SetRot(float x, float y, float z, float w) override;
	void SetScale(float x, float y, float z) override;

	const Vec3& GetPos() const override;
	const Quat& GetRot() const override;
	const Vec3& GetScale() const override;

protected:
	btRigidBody* body;
};