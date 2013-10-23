////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ISceneManager.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the scene manager.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../../Core/src/GraphicsEntity.h"
#include "../../Core/src/zsString.h"

class cCamera;

class ISceneManager {
public:
	virtual cGraphicsEntity& AddEntity(const zsString& geometry, const zsString& material) = 0;
	virtual void RemoveEntity(const cGraphicsEntity& entity) = 0;
	virtual cCamera* GetActiveCamera() const = 0;
	virtual void SetActiveCamera(cCamera *cam) = 0;
};