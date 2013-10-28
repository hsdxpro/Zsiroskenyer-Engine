////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ShaderManager.h
//	2013.oct.14, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "ShaderManager.h"
#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IShaderProgram.h"


// load a shader by its name, or return it if it's already loaded
IShaderProgram* cShaderManager::LoadShader(const zsString& shaderDir, const zsString& shaderName) {
	IShaderProgram* shProg = NULL;

	auto it = loadedShaders.right.find(shaderName);
	if (it != loadedShaders.right.end()) {
		shProg = it->second;
	} else {
		shProg = graphicsApi->CreateShaderProgram(shaderDir + shaderName);
		loadedShaders.insert(ShaderMapT::value_type(shProg, shaderName));
	}
	return shProg;
}


// unload shader
void cShaderManager::UnloadShader(IShaderProgram* shader) {
	auto it = loadedShaders.left.find(shader);
	if (it==loadedShaders.left.end())
		return;

	delete shader;
	loadedShaders.left.erase(it);
}

void cShaderManager::UnloadShader(const zsString& shaderName) {
	auto it = loadedShaders.right.find(shaderName);
	if (it==loadedShaders.right.end())
		return;

	delete it->second;
	loadedShaders.right.erase(it);
}


// reload a shader: it WILL load previously unloaded shaders as well
IShaderProgram* cShaderManager::ReloadShader(const zsString& shaderDir, const zsString& shaderName) {
	UnloadShader(shaderName);
	return LoadShader(shaderDir, shaderName);
}
// this version won't load previously unloaded shaders, since it doesn't know the name
IShaderProgram* cShaderManager::ReloadShader(const zsString& shaderDir, IShaderProgram* shader) {
	// get shader's name first
	auto it = loadedShaders.left.find(shader);
	if (it==loadedShaders.left.end())
		return NULL;

	zsString name = it->second;

	UnloadShader(shader);
	return LoadShader(shaderDir, name);
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


// constructs the shader manager with a graphics interface [required, no default]
cShaderManager::cShaderManager(IGraphicsApi* graphicsApi) : graphicsApi(graphicsApi) {
}
// destructing the manager DESTROYS ALL LOADED SHADERS
cShaderManager::~cShaderManager() {
	Reset();
}