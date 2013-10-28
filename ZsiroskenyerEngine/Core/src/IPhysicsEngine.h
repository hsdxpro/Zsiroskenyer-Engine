// IPhysicsEngine.h By Zsíroskenyér Team 2013.10.23 22:34
#pragma once

#include "zsString.h"
#include "GeometryBuilder.h"
#include "IPhysicsType.h"

class IPhysicsEngine {
public:
	virtual void SimulateWorld(float deltaT) = 0;
	virtual IPhysicsType* LoadRigidType(const zsString& geomPath, const cGeometryBuilder::tGeometryDesc& desc, float mass) = 0;
	virtual IPhysicsType* GetRigidType(const zsString& geomPath) = 0;

	virtual void Release() = 0;
};