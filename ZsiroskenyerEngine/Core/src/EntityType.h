// EntityType.h By Zs�roskeny�r Team 2013.10.26 20:45
// General Entity Description, like it's mass, it's geometry
#pragma once
#include "..\..\GraphicsEngineRaster\src\ResourceManager.h"

#include "IPhysicsType.h"

class cEntityType {
public:
	cGeometryRef* GetGraphicsGeometry() const;
	cMaterialRef* GetMaterial() const;
	const IPhysicsType* GetPhysicsType() const;

	cEntityType(cGeometryRef *graphicsGeom, cMaterialRef *material, IPhysicsType* physicsType);
protected:
	cGeometryRef* graphicsGeom;
	cMaterialRef* material;
	IPhysicsType* physicsType;
};