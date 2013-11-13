// IPhysicsEntity.h By Zsíroskenyér Team 2013.10.25 23:41
#pragma once

#include "math\vec3.h"
#include "math\quat.h"
#include "Transform3D.h"
#include "IBaseEntity.h"

class IPhysicsEntity : public IBaseEntity {
public:
	virtual void Release() = 0;
};