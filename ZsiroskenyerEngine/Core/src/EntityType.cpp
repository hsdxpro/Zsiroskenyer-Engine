#include "EntityType.h"

cEntityType::cEntityType(cGeometryRef *graphicsGeom, cMaterialRef *material, IPhysicsType* physicsType)
:graphicsGeom(graphicsGeom), material(material), physicsType(physicsType){ 
}

cGeometryRef* cEntityType::GetGraphicsGeometry() const {
	return graphicsGeom;
}

cMaterialRef* cEntityType::GetMaterial() const {
	return material;
}

const IPhysicsType* cEntityType::GetPhysicsType() const {
	return physicsType;
}