// IPhysicsEngine.h By Zsíroskenyér Team 2013.10.23 22:34
#pragma once

#include "zsString.h"
#include "GeometryBuilder.h"
#include "IPhysicsType.h"

class IPhysicsEngine {
public:
	virtual void SimulateWorld(float deltaT) = 0;
	virtual IPhysicsType* LoadRigidType(const zsString& geomPath, float mass, const cGeometryBuilder::tGeometryDesc* desc = NULL) = 0;

	virtual bool IsGeometryExists(const zsString& geomPath) = 0;

	virtual void Release() = 0;
};