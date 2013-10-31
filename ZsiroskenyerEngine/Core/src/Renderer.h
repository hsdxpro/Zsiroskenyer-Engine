// Renderer.h By Zsíroskenyér Team 2013.10.31 9:13
// For general purpose rendering like 3D lines, or etc..
#pragma once

#include "math\vec3.h"
#include "math\Matrix44.h"

class IGraphicsApi;
class IShaderManager;

class cRenderer {
public:
	void RenderLines(const Matrix44& viewProj, const Vec3* lines, size_t nLines, const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));

	cRenderer(IGraphicsApi* gApi, IShaderManager* shaderManager);
protected:
	IGraphicsApi* gApi;
	IShaderManager* shaderManager;
};