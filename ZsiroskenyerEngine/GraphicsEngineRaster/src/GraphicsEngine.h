////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/IGraphicsEngine.h"

class ITexture2D;
class IGraphicsApi;
class IShaderManager;
class cSceneManager;
class cResourceManager;
class ITexture2D;
class IShaderProgram;

class cGraphicsEngine : public IGraphicsEngine {
	class cDeferredRenderer;
public:
	// lifecycle (creation, destruction)
	cGraphicsEngine();
	~cGraphicsEngine();
	void Release() override;

	// utlity
	void ReloadResources() override;
	void SetActiveCamera(cCamera* cam) override;
	cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) override;

	// rendering
	void RenderSceneForward() override;
	
	// sub-component accessors
	cSceneManager*		GetSceneManager();
	cResourceManager*	GetResourceManager();
	IGraphicsApi*		GetGraphicsApi() override;
	IShaderManager*		GetShaderManager() override;
	cCamera*			GetActiveCamera() override;
private:
	// sub-compnents
	IGraphicsApi* gApi;
	IShaderManager* shaderManager;
	cResourceManager* resourceManager;
	cSceneManager* sceneManager;
	cDeferredRenderer* deferredRenderer;

	// deferred renderer helper subclass
	class cDeferredRenderer {
	public:
		cDeferredRenderer(cGraphicsEngine& parent);
		~cDeferredRenderer();
	private:
		ITexture2D* gBuffer[3];
		ITexture2D* compositionBuffer;
		IGraphicsApi* gApi;
		IShaderProgram* shaderGBuffer;
		IShaderProgram* shaderComposition;
		cGraphicsEngine& parent;
	};
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster() {
	return new cGraphicsEngine();
}