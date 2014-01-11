// SoftEntityBullet.h 2013.11.24 00:22
// Implementation of Soft body based on bullet
#pragma once

#include "..\..\Core\src\IPhysicsEntity.h"
#include "..\..\Core\src\Transform3D.h"

class btSoftBody;

class cSoftEntityBullet : public IPhysicsEntity {
public:
	cSoftEntityBullet(btSoftBody* body);
	void Release();

	void SetPos(const Vec3& p);
	void SetPos(float x, float y, float z);

	void SetRot(const Quat& r);
	void SetRot(float x, float y, float z, float w);


	void SetScale(const Vec3& s);
	void SetScale(float x, float y, float z);

	const Vec3& GetPos() const;
	const Quat& GetRot() const;
	const Vec3& GetScale() const;
protected:
	btSoftBody* body;

	cTransform3D worldTransform;
};