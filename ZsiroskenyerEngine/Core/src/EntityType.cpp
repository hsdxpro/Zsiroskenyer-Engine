#include "EntityType.h"

cEntityType::cEntityType(cGeometryRef *graphicsGeom, cGeometryRef *physicsGeom, cMaterialRef *material, float mass)
:graphicsGeom(graphicsGeom), physicsGeom(physicsGeom), material(material), mass(mass) { 
}