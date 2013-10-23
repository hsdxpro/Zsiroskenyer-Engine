////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IGraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "IManagerScene.h"
#include "../../GraphicsEngineRaster/src/ManagerResource.h"
#include "../../GraphicsApi/src/IGraphicsApi.h"

class IGraphicsEngine {
public:
	virtual void Release() = 0;

	virtual void RenderWorldForward() = 0;

	virtual IManagerScene*		GetManagerScene() = 0;
	virtual cManagerResource*	GetManagerResource() = 0;
	virtual IGraphicsApi*		GetGraphicsApi() = 0;
};