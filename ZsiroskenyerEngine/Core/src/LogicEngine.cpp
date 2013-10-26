#include "LogicEngine.h"

bool cLogicEngine::IsEntityTypeExits(const zsString& str) {
	return entityTypes.find(str) != entityTypes.end();
}

cEntityType* cLogicEngine::CreateEntityType(const zsString& name, cGeometryRef *graphicsGeom, cGeometryRef *physicsGeom, cMaterialRef *material, float mass) {
	cEntityType* newType = new cEntityType(graphicsGeom, physicsGeom, material, mass);
	entityTypes[name] = newType;
	return newType;
}