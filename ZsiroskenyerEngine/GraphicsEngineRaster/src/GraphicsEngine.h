////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/IGraphicsEngine.h"


class IGraphicsApi;
class cShaderManager;
class cSceneManager;
class cResourceManager;

class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();
	void Release() override;

	void RenderSceneForward() override;

	ISceneManager*		GetSceneManager();
	cResourceManager*	GetResourceManager();
	IGraphicsApi*		GetGraphicsApi();
private:	
	IGraphicsApi* gApi;
	cShaderManager* shaderManager;
	cResourceManager* resourceManager;
	cSceneManager* sceneManager;
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster() {
	return new cGraphicsEngine();
}