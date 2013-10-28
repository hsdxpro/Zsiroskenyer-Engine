// LogicEngine.h By Zsíroskenyér Team 2013.10.26 20:48
#pragma once

#include "Entity.h"
#include "EntityType.h"

#include <map>
#include <list>
#include "zsString.h"

#include "..\..\GraphicsEngineRaster\src\ResourceManager.h"
#include "IPhysicsType.h"

class cLogicEngine {
public:
	bool IsEntityTypeExits(const zsString& str);

	cEntityType* CreateEntityType(const zsString& name, cGeometryRef *graphicsGeom, cMaterialRef *material, IPhysicsType* physType);
	cEntity* AddEntity(cGraphicsEntity* gEntity, IPhysicsEntity* pEntity);

protected:
	std::map<zsString, cEntityType*> entityTypes;
	std::list<cEntity*> entities;
};