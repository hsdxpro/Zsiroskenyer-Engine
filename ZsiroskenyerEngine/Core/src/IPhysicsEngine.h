// IPhysicsEngine.h By Zs�roskeny�r Team 2013.10.23 22:34
#pragma once

#include "IPhysicsType.h"
#include "IPhysicsEntity.h"

#include "zsString.h"
#include "math\Vec3.h"

class IPhysicsEngine {
public:
	// Simulation of the physics world ( deltaT seconds )
	virtual void SimulateWorld(float deltaT) = 0;

	// Add new rigid entity to the physics world
	virtual IPhysicsEntity *AddRigidEntity(const IPhysicsType* type, const Vec3& position) = 0;

	// Get PhysicsType associated to the geomPath file
	virtual IPhysicsType* GetRigidType(const zsString& geomPath, float mass) = 0;

	// Get all edges from the physics world
	virtual void GetCollisionShapeEdges(Vec3* edges, size_t size, size_t& nEdges) = 0;

	// Check whether collision geometry exists based on filePath
	virtual bool IsGeometryExists(const zsString& geomPath) = 0;

	// astala vista PhysicsEngine
	virtual void Release() = 0;
};