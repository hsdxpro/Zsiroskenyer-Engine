// IPhysicsEngine.h By Zsíroskenyér Team 2013.10.23 22:34
#pragma once

#include "zsString.h"
#include "GeometryBuilder.h"

class IPhysicsEngine {
public:
	virtual void simulateWorld(float deltaT) = 0;
	virtual void Release() = 0;

	virtual bool IsGeometryExists(const zsString& filePath) = 0;
	virtual void LoadGeometry(const zsString& filePath, const cGeometryBuilder::tGeometryDesc& geomDesc) = 0;
};