////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IGraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "zsString.h"

class IGraphicsApi;
class IShaderManager;
class cGraphicsEntity;
class cCamera;

class IGraphicsEngine {
public:
	virtual void Release() = 0;
	virtual void ReloadResources() = 0;

	virtual void RenderSceneForward() = 0;

	virtual void SetActiveCamera(cCamera* cam) = 0;

	virtual cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) = 0;

	virtual IGraphicsApi*		GetGraphicsApi() = 0;
	virtual IShaderManager*		GetShaderManager() = 0;

	virtual cCamera* GetActiveCamera() = 0;
};
