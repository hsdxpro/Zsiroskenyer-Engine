#include "LogicEngine.h"


void cLogicEngine::Update() {
	// Copy transformations of Physics Objects to GraphicsObjects
	for (auto* entity : entities) {
		entity->GetGraphicsEntity()->SetPosition(entity->GetPhysicsEntity()->GetPosition());
		entity->GetGraphicsEntity()->SetRotation(entity->GetPhysicsEntity()->GetRotation());
	}
}

cEntityType* cLogicEngine::CreateEntityType(const zsString& name, cGeometryRef *graphicsGeom, cMaterialRef *material, IPhysicsType* physType) {
	cEntityType* newType = new cEntityType(graphicsGeom, material, physType);
	entityTypes[name] = newType;
	return newType;
}

cEntity* cLogicEngine::AddEntity(cGraphicsEntity* gEntity, IPhysicsEntity* pEntity) {
	cEntity* e = new cEntity(gEntity, pEntity);
	entities.push_back(e);
	return e;
}

bool cLogicEngine::IsEntityTypeExits(const zsString& str) {
	return entityTypes.find(str) != entityTypes.end();
}