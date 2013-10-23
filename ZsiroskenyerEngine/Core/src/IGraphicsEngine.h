////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IGraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "ISceneManager.h"
#include "../../GraphicsEngineRaster/src/ResourceManager.h"
#include "../../Core/src/IGraphicsApi.h"

class IGraphicsEngine {
public:
	virtual void Release() = 0;

	virtual void RenderWorldForward() = 0;

	virtual ISceneManager*		GetManagerScene() = 0;
	virtual cResourceManager*	GetManagerResource() = 0;
	virtual IGraphicsApi*		GetGraphicsApi() = 0;
};