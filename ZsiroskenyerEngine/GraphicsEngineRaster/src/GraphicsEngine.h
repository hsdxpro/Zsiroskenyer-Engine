////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/IGraphicsEngine.h"
#include "../../Core/src/GAPI.h"
#include "SceneManager.h"
#include <set>

class ITexture2D;
class IGraphicsApi;
class IShaderManager;
class cResourceManager;
class ITexture2D;
class IShaderProgram;
class IWindow;

////////////////////////////////////////////////////////////////////////////////
//	Dll accessor
extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config);


////////////////////////////////////////////////////////////////////////////////
//	GraphicsScene
class cGraphicsScene : public IGraphicsScene {
public:
	// constructor & destructor
	cGraphicsScene();
	~cGraphicsScene();

	// entities & lights
	cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) override;
	void DeleteEntity(const cGraphicsEntity* entity) override;
	cGraphicsLight* CreateLight() override;
	void DeleteLight(const cGraphicsLight* light) override;

	// scene state
	void SetActiveCamera(cCamera* cam) override;
	cCamera* GetActiveCamera() override;
	void SetRenderState(tRenderState state) override;
	void Clear() override;
private:
	tRenderState state;
	cSceneManager sceneManager;
};


////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine
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

	// DEPRECATED
	void SetActiveCamera(cCamera* cam) override;
	cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) override;
	cCamera* GetActiveCamera() override;
#pragma deprecated(SetActiveCamera, CreateEntity, GetActiveCamera)

	// NEW - scene management
	IGraphicsScene*	CreateScene() override;
	void			DeleteScene(const IGraphicsScene* scene) override;

	// rendering
	eGraphicsResult Update(float elapsed = 0.0f) override;
		
	// sub-component accessors
	cSceneManager*		GetSceneManager();
	cResourceManager*	GetResourceManager();
	IGraphicsApi*		GetGraphicsApi() override;
	IShaderManager*		GetShaderManager() override;
private:
	// internal functions
	void RenderSceneForward();
	void RenderSceneDeferred();

	// state
	unsigned screenWidth, screenHeight;
	tGraphicsConfig config;

	// sub-compnents: rendering & graphical
	IGraphicsApi* gApi;
	IShaderManager* shaderManager;
	cResourceManager* resourceManager;
	std::set<cGraphicsScene*> graphicsScenes;
	cDeferredRenderer* deferredRenderer;
	cHDRProcessor* hdrProcessor;
	// DEPRECATED
	cSceneManager* sceneManager;
#pragma deprecated(sceneManager)
	
	// some member var for fast access in deferred functions
	IShaderProgram* screenCopyShader;


	// --- --- SUBCLASSES --- --- //

	// Deferred renderer helper subclass
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
		eGraphicsResult SetSource(ITexture2D* srcTexture, unsigned sourceWidth, unsigned sourceHeight);
		void SetDestination(ITexture2D* dest);
		void Update(float elapsedSec = -1.0f);
	private:
		cGraphicsEngine& parent;
		IGraphicsApi* gApi;
		ITexture2D* source;
		ITexture2D* dest;
		ITexture2D* luminanceBuffer[10]; // for 10 buffers down from 512x512 ... 1x1
		ITexture2D* luminanceStaging;
		ITexture2D* blurBuffer;
		ITexture2D* downSampled;
		float avgLuminance;
		float adaptedLuminance;
		IShaderProgram *shaderLumSample, *shaderLumAvg, *shaderCompose;
		IConstantBuffer* cbCompose;
		unsigned sourceWidth, sourceHeight;

		void Cleanup();
	};
};
