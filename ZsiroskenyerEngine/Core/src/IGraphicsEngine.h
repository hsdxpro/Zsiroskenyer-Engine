////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IGraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "ISceneManager.h"
#include "..\..\GraphicsEngineRaster\src\ResourceManager.h"
#include "IGraphicsApi.h"

#include "math\vec3.h"

class IGraphicsEngine {
public:
	virtual void Release() = 0;

	virtual void RenderSceneForward() = 0;
	virtual void RenderLines(const Vec3* lines, size_t nLines, const Vec3& color = Vec3(1.0f, 1.0f, 1.0f)) = 0;

	virtual ISceneManager*		GetSceneManager() = 0;
	virtual cResourceManager*	GetResourceManager() = 0;
	virtual IGraphicsApi*		GetGraphicsApi() = 0;
};