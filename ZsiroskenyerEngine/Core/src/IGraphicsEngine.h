////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IGraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "ISceneManager.h"
#include "../../GraphicsEngineRaster/src/ResourceManager.h"
#include "IGraphicsApi.h"

class IGraphicsEngine {
public:
	virtual void Release() = 0;

	virtual void RenderSceneForward() = 0;

	virtual ISceneManager*		GetSceneManager() = 0;
	virtual cResourceManager*	GetResourceManager() = 0;
	virtual IGraphicsApi*		GetGraphicsApi() = 0;
};