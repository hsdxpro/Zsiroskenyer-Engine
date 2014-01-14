////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ShaderManager.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//
// THIS FILE IS NO LONGER IN USE, BUT IS KEPT FOR A SHORT TIME
//
////////////////////////////////////////////////////////////////////////////////

/*

#include "ShaderManager.h"
#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IShaderProgram.h"
#include "../../Core/src/StrUtil.h"

cShaderManager::cShaderManager(IGraphicsApi* gApi) 
	:gApi(gApi) {
}

cShaderManager::~cShaderManager() {
	Reset();
}

IShaderProgram* cShaderManager::LoadShader(const zsString& shaderPath) {
	IShaderProgram* shProg = NULL;

	zsString shaderName = shaderPath;
	cStrUtil::CutDirectory(shaderName);

	auto it = loadedShaders.right.find(shaderName);
	if (it != loadedShaders.right.end()) {
		shProg = it->second;
	} else {
		gApi->CreateShaderProgram(&shProg, shaderPath);
		loadedShaders.insert(ShaderMapT::value_type(shProg, shaderName));
	}
	return shProg;
}


// unload shader
void cShaderManager::UnloadShader(IShaderProgram* shader) {
	auto it = loadedShaders.left.find(shader);
	if (it==loadedShaders.left.end())
		return;

	shader->Release();
	loadedShaders.left.erase(it);
}

void cShaderManager::UnloadShader(const zsString& shaderName) {
	auto it = loadedShaders.right.find(shaderName);
	if (it==loadedShaders.right.end())
		return;

	it->second->Release();
	loadedShaders.right.erase(it);
}


// reload a shader: it WILL load previously unloaded shaders as well
IShaderProgram* cShaderManager::ReloadShader(const zsString& shaderPath) {
	zsString shaderName = shaderPath;
	cStrUtil::CutDirectory(shaderName);

	UnloadShader(shaderName);
	return LoadShader(shaderPath);
}
// this version won't load previously unloaded shaders, since it doesn't know the name
IShaderProgram* cShaderManager::ReloadShader(const zsString& shaderPath, IShaderProgram* shader) {
	// get shader's name first
	auto it = loadedShaders.left.find(shader);
	if (it==loadedShaders.left.end())
		return NULL;

	zsString name = it->second;

	UnloadShader(shader);
	return LoadShader(shaderPath);
}


// unloads all shader resources
void cShaderManager::Reset() {
	for (auto& shader : loadedShaders) {
		delete shader.left;
	}

	loadedShaders.clear();
}


// return a reference to a shader by its name
IShaderProgram* cShaderManager::GetShaderByName(const zsString& name) {
	auto it = loadedShaders.right.find(name);
	if (it==loadedShaders.right.end())
		return NULL;
	else
		return it->second;
}

*/