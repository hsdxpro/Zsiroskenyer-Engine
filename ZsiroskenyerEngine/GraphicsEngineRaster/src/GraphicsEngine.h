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
class IShaderManager;
class cSceneManager;
class cResourceManager;

class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();
	~cGraphicsEngine();
	void Release() override;
	void ReloadResources() override;

	void RenderSceneForward() override;

	void SetActiveCamera(cCamera* cam) override;

	cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) override;

	cSceneManager*		GetSceneManager();
	cResourceManager*	GetResourceManager();
	IGraphicsApi*		GetGraphicsApi() override;
	IShaderManager*		GetShaderManager() override;

	cCamera*			GetActiveCamera() override;
private:	
	IGraphicsApi* gApi;
	IShaderManager* shaderManager;
	cResourceManager* resourceManager;
	cSceneManager* sceneManager;

	class cDeferredRenderer {
	public:
	private:
	};
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster() {
	return new cGraphicsEngine();
}