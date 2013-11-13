// IPhysicsEntity.h By Zsíroskenyér Team 2013.10.25 23:41
#pragma once

#include "math\vec3.h"
#include "math\quat.h"
#include "ITransformable3D.h"

class IPhysicsEntity : public ITransformable3D {
public:
	virtual void Release() = 0;
};