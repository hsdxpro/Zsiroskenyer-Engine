// IPhysicsEntity.h By Zsíroskenyér Team 2013.10.25 23:41
#pragma once

#include "math\vec3.h"
#include "math\quat.h"

class IPhysicsEntity {
public:
	virtual void SetPos(const Vec3& pos) = 0;
	virtual void SetRotation(const Quat& rot) = 0;

	virtual Vec3 GetPos() = 0;
	virtual Quat GetRotation() = 0;

	virtual void Release() = 0;
};