// IPhysicsEngine.h By Zsíroskenyér Team 2013.10.23 22:34
#pragma once

#include "IPhysicsType.h"
#include "IPhysicsEntity.h"

#include "GeometryBuilder.h"
#include "zsString.h"
#include "math\Vec3.h"

#include <list>

class IPhysicsEngine {
public:
	virtual void SimulateWorld(float deltaT) = 0;

	virtual IPhysicsEntity *AddRigidEntity(const IPhysicsType* type, const Vec3& position) = 0;
	virtual IPhysicsType* LoadRigidType(const zsString& geomPath, float mass, const cGeometryBuilder::tGeometryDesc* desc = NULL) = 0;

	virtual std::list<Vec3>* GetCollisionShapeEdges() = 0;
	virtual bool IsGeometryExists(const zsString& geomPath) = 0;

	virtual void Release() = 0;
};