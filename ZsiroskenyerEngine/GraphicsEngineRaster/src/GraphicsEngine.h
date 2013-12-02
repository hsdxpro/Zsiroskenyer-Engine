////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/IGraphicsEngine.h"
#include "../../Core/src/GAPI.h"

class ITexture2D;
class IGraphicsApi;
class IShaderManager;
class cSceneManager;
class cResourceManager;
class ITexture2D;
class IShaderProgram;
class IWindow;

extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config);


class cGraphicsEngine : public IGraphicsEngine {
	class cDeferredRenderer;
	class cHDRProcessor;
public:
	// lifecycle (creation, destruction)
	cGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config);
	cGraphicsEngine(const cGraphicsEngine&) = delete;
	~cGraphicsEngine();
	cGraphicsEngine& operator=(const cGraphicsEngine&) = delete;
	void Release() override;

	// utlity & settings
	eGraphicsResult ReloadResources() override;
	eGraphicsResult SetConfig(tGraphicsConfig config) override;
	eGraphicsResult Resize(unsigned width, unsigned height) override;
	void SetActiveCamera(cCamera* cam) override;
	cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) override;

	// rendering
	eGraphicsResult Update() override;
		
	// sub-component accessors
	cSceneManager*		GetSceneManager();
	cResourceManager*	GetResourceManager();
	IGraphicsApi*		GetGraphicsApi() override;
	IShaderManager*		GetShaderManager() override;
	cCamera*			GetActiveCamera() override;
private:
	// internal functions
	void RenderSceneForward();
	void RenderSceneDeferred();

	// state
	unsigned screenWidth, screenHeight;

	// sub-compnents
	IGraphicsApi* gApi;
	IShaderManager* shaderManager;
	cResourceManager* resourceManager;
	cSceneManager* sceneManager;
	cDeferredRenderer* deferredRenderer;
	cHDRProcessor* hdrProcessor;
	
	// some member var for fast access in deferred functions
	IShaderProgram* screenCopyShader;

	// deferred renderer helper subclass
	class cDeferredRenderer {
	public:
		// constructor
		cDeferredRenderer(cGraphicsEngine& parent);
		cDeferredRenderer(const cDeferredRenderer&) = delete;
		~cDeferredRenderer();
		cDeferredRenderer& operator=(const cDeferredRenderer&) = delete;

		void RenderComposition();

		void ReloadShaders();

		eGraphicsResult Resize(unsigned width, unsigned height);

		ITexture2D* GetCompositionBuffer();
	private:
		ITexture2D* gBuffer[3];
		ITexture2D* compositionBuffer;
		ITexture2D* helperBuffer;
		ITexture2D* depthBuffer;

		IGraphicsApi* gApi;
		IShaderProgram* shaderGBuffer;
		IShaderProgram* shaderComposition;
		cGraphicsEngine& parent;

		eGapiResult ReallocBuffers();

		// Constant buffers for shaders TODO, move them to IShaderProgram
		IConstantBuffer* gBufferConstantBuffer;
		IConstantBuffer* compConstantBuffer;

		// Remove that also..
		ITexture2D* motionBlurredBuffer;

		unsigned bufferWidth, bufferHeight;
	};

	// HDR post-processor helper class
	class cHDRProcessor {
	public:
		cHDRProcessor(cGraphicsEngine& parent);
		~cHDRProcessor();
	private:
		cGraphicsEngine& parent;
	};
};
