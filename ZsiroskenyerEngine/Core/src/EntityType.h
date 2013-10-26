// EntityType.h By Zsíroskenyér Team 2013.10.26 20:45
// General Entity Description, like it's mass, it's geometry
#pragma once
#include "..\..\GraphicsEngineRaster\src\ResourceManager.h"

class cEntityType {
public:
	cEntityType(cGeometryRef *graphicsGeom, cGeometryRef *physicsGeom, cMaterialRef *material, float mass);
protected:
	cGeometryRef* graphicsGeom;
	cGeometryRef* physicsGeom;
	cMaterialRef* material;

	float mass;
};