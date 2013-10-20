////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ManagerShader.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "ManagerShader.h"
#include "../../GraphicsApi/src/IGraphicsApi.h"
#include "../../GraphicsApi/src/IShaderProgram.h"


// load a shader by its name, or return it if it's already loaded
IShaderProgram* cManagerShader::LoadShader(const zsString& shaderDir, const zsString& shaderName) {
	auto it = loadedShaders.right.find(shaderName);
	if (it!=loadedShaders.right.end()) {
		return it->second;
	}

	IShaderProgram* shader = graphicsApi->CreateShaderProgram(shaderDir + shaderName);
	if (shader==NULL)
		return NULL;

	loadedShaders.insert(ShaderMapT::value_type(shader, shaderName));

	return shader;
}


// unload shader
void cManagerShader::UnloadShader(IShaderProgram* shader) {
	auto it = loadedShaders.left.find(shader);
	if (it==loadedShaders.left.end())
		return;

	delete shader;
	loadedShaders.left.erase(it);
}

void cManagerShader::UnloadShader(const zsString& shaderName) {
	auto it = loadedShaders.right.find(shaderName);
	if (it==loadedShaders.right.end())
		return;

	delete it->second;
	loadedShaders.right.erase(it);
}


// reload a shader: it WILL load previously unloaded shaders as well
IShaderProgram* cManagerShader::ReloadShader(const zsString& shaderDir, const zsString& shaderName) {
	UnloadShader(shaderName);
	return LoadShader(shaderDir, shaderName);
}
// this version won't load previously unloaded shaders, since it doesn't know the name
IShaderProgram* cManagerShader::ReloadShader(const zsString& shaderDir, IShaderProgram* shader) {
	// get shader's name first
	auto it = loadedShaders.left.find(shader);
	if (it==loadedShaders.left.end())
		return NULL;

	zsString name = it->second;

	UnloadShader(shader);
	return LoadShader(shaderDir, name);
}


// unloads all shader resources
void cManagerShader::Reset() {
	for (auto& shader : loadedShaders) {
		delete shader.left;
	}

	loadedShaders.clear();
}


// return a reference to a shader by its name
IShaderProgram* cManagerShader::GetShaderByName(const zsString& name) {
	auto it = loadedShaders.right.find(name);
	if (it==loadedShaders.right.end())
		return NULL;
	else
		return it->second;
}


// constructs the shader manager with a graphics interface [required, no default]
cManagerShader::cManagerShader(IGraphicsApi* graphicsApi) : graphicsApi(graphicsApi) {
}
// destructing the manager DESTROYS ALL LOADED SHADERS
cManagerShader::~cManagerShader() {
	Reset();
}