// IShaderManager.h By Zsíroskenyér Team 2013.10.31 9:33
#pragma once

#include "zsString.h"

class IShaderProgram;

class IShaderManager {
public:
	virtual IShaderProgram* LoadShader(const zsString& shaderDir, const zsString& shaderName) = 0;

	virtual void UnloadShader(const zsString& shaderName) = 0;
	virtual void UnloadShader(IShaderProgram* shader) = 0;

	virtual IShaderProgram* ReloadShader(const zsString& shaderDir, const zsString& shaderName)  = 0;
	virtual IShaderProgram* ReloadShader(const zsString& shaderDir, IShaderProgram* shader) = 0;

	virtual IShaderProgram* GetShaderByName(const zsString& shaderName) = 0;

	virtual void Reset() = 0; // unloads all shaders
};
