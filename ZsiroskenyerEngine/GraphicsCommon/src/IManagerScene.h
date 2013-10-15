////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ISceneManager.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the scene manager.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../../GraphicsCommon/src/Entity.h"
#include "../../CommonLib/src/zsString.h"

class cCamera;

class IManagerScene {
public:
	virtual cEntity& AddEntity(const zsString& geometry, const zsString& material) = 0;
	virtual void RemoveEntity(const cEntity& entity) = 0;
	virtual cCamera* GetActiveCamera() const = 0;
	virtual void SetActiveCamera(cCamera *cam) = 0;
};