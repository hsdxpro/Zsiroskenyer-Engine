// IPhysicsEngine.h By Zsíroskenyér Team 2013.10.23 22:34
#pragma once

#include "zsString.h"
#include "math\Vec3.h"

class IPhysicsEntity;

class IPhysicsEngine {
public:
	// Simulation of the physics world ( deltaT seconds )
	virtual void SimulateWorld(float deltaT) = 0;

	// Add new rigid entity to the physics world
	virtual IPhysicsEntity *CreateRigidEntity(const zsString& physicsGeom, float mass) = 0;

	// Get all edges from the physics world
	virtual void GetCollisionShapeEdges(Vec3* edges, size_t size, size_t& nEdges) = 0;

	// astala vista PhysicsEngine
	virtual void Release() = 0;
};