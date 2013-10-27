#include "EntityType.h"

cEntityType::cEntityType(cGeometryRef *graphicsGeom, cMaterialRef *material, float mass)
:graphicsGeom(graphicsGeom), material(material), mass(mass) { 
}

cGeometryRef* cEntityType::GetGraphicsGeometry() const {
	return graphicsGeom;
}

cMaterialRef* cEntityType::GetMaterial() const {
	return material;
}