// RigidEntityBullet.h By Zsíroskenyér Team 2013.10.28 10:26
#pragma once

#include "..\..\Core\src\IPhysicsEntity.h"
#include "..\..\Core\src\Transform3D.h"

class btRigidBody;

class cRigidEntityBullet : public IPhysicsEntity, private cTransform3D {
public:
	cRigidEntityBullet(btRigidBody* body);
	void Release() override;

	void SetPos(const Vec3& p) override;
	void SetPos(float x, float y, float z) override;

	void SetRot(const Quat& r) override;
	void SetRot(float x, float y, float z, float w) override;

	void SetScale(const Vec3& s) override;
	void SetScale(float x, float y, float z) override;

	const Vec3& GetPos() override;
	const Quat& GetRot() override;
	const Vec3& GetScale() override;

protected:
	btRigidBody* body;

	//cTransform3D worldTransform;
};