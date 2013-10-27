////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ISceneManager.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the scene manager.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "GraphicsEntity.h"
#include "zsString.h"

class cCamera;
class cGeometryRef;
class cMaterialRef;

class ISceneManager {
public:
	virtual cGraphicsEntity* AddEntity(cGeometryRef *geom, cMaterialRef* mtl) = 0;
	virtual void RemoveEntity(const cGraphicsEntity& entity) = 0;

	virtual void SetActiveCamera(cCamera *cam) = 0;
	virtual cCamera *GetActiveCamera() const = 0;
};