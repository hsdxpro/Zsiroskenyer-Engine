////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ISceneManager.h
//	2013.oct.10, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the scene manager.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../../GraphicsEngine/src/Entity.h"
#include "../../CommonLib/src/zsString.h"

class IManagerScene {
public:
	virtual cEntity& AddEntity(const zsString& geometry, const zsString& material) = 0;
	virtual void RemoveEntity(const cEntity& entity) = 0;
};