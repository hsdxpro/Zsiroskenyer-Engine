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
#include "../../Core/src/Camera.h"
#include "SceneManager.h"
#include <set>
#include <deque>

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
	friend class cGraphicsEngine;
public:
	// constructor & destructor
	cGraphicsScene(cGraphicsEngine& parent, tRenderState state = tRenderState());
	~cGraphicsScene();

	// entities & lights
	cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) override;
	void DeleteEntity(const cGraphicsEntity* entity) override;
	cGraphicsLight* CreateLight() override;
	void DeleteLight(const cGraphicsLight* light) override;

	// scene state
	cCamera& GetCamera() override;
	tRenderState& GetState() override;
	void Clear() override;

	// scene order
	void MoveUp() override;
	void MoveDown() override;
	void MoveTop() override;
	void MoveBottom() override;
private:
	// information for rendering pipeline
	tRenderState state;
	cSceneManager sceneManager;
	cGraphicsEngine& parent;
	cCamera camera;

	// temporary per-scene renderer data
	float luminanceAdaptation;
};


////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine
class cGraphicsEngine : public IGraphicsEngine {
	friend class cGraphicsScene;
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
	
	// scene management
	IGraphicsScene*	CreateScene(tRenderState state = tRenderState()) override;
	void			DeleteScene(const IGraphicsScene* scene) override;

	// rendering pipeline
	eGraphicsResult Update(float elapsed = 0.0f) override;
		// scene state loaded there
	cSceneManager* sceneManager;
	cCamera* camera;
	float elapsed;
		
	// sub-component accessors
	cSceneManager*		GetSceneManager();
	cResourceManager*	GetResourceManager();
	IGraphicsApi*		GetGraphicsApi() override;
	IShaderManager*		GetShaderManager() override;
private:
	// rendering functions
	void RenderScene(cGraphicsScene& scene, float elapsed);
	void RenderForward();
	void RenderDeferred();
	// render states
	static const tDepthStencilDesc depthStencilDefault;
	static const tBlendDesc blendDefault;

	// state
	unsigned screenWidth, screenHeight;
	tGraphicsConfig config;

	// sub-compnents: rendering & graphical
	IGraphicsApi* gApi;
	IShaderManager* shaderManager;
	cResourceManager* resourceManager;
	std::set<cGraphicsScene*> graphicsScenes;
	std::deque<cGraphicsScene*> graphicsSceneOrder;
	cDeferredRenderer* deferredRenderer;
	cHDRProcessor* hdrProcessor;
	
	// some member var for fast access in deferred functions
	IShaderProgram* screenCopyShader;


	// --- --- SUBCLASSES --- --- //

	// Deferred renderer helper subclass
	class cDeferredRenderer {
		friend class cGraphicsEngine;
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
		ITexture2D* DOFInput;
		ITexture2D* depthBuffer;
		ITexture2D* depthBufferCopy;

		IGraphicsApi* gApi;
		IShaderProgram* shaderGBuffer;
		IShaderProgram* shaderComposition;
		IShaderProgram *shaderDirectional, *shaderPoint, *shaderSpot, *shaderAmbient;
		cGraphicsEngine& parent;

		IVertexBuffer *vbSpot, *vbPoint;
		IIndexBuffer *ibSpot, *ibPoint;

		eGapiResult ReallocBuffers();
		void Cleanup();

		// Constant buffers for shaders TODO, move them to IShaderProgram
		IConstantBuffer* gBufferConstantBuffer;
		IConstantBuffer* compConstantBuffer;
		IConstantBuffer* lightPassConstants;

		// Remove that also..
		ITexture2D* motionBlurredBuffer;

		unsigned bufferWidth, bufferHeight;
	};

	// HDR post-processor helper class
	class cHDRProcessor {
		friend class cGraphicsEngine;
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
		IShaderProgram *shaderLumSample, *shaderLumAvg, *shaderOverbright, *shaderBlurVert, *shaderBlurHoriz, *shaderCompose;
		IConstantBuffer* cbCompose;
		unsigned sourceWidth, sourceHeight;

		void Cleanup();
	};
};
