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
#include "../../Core/src/math/Matrix44.h"
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

class IGeometryBuilder;
class IGeometryRef;

class IGraphicsEntity;
class IGraphicsLight;

////////////////////////////////////////////////////////////////////////////////
//	Dll accessor
extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngineRaster(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config, const char** errorMessage = nullptr);


////////////////////////////////////////////////////////////////////////////////
//	GraphicsScene
class cGraphicsScene : public IGraphicsScene {
	friend class cGraphicsEngine;
public:
	// constructor & destructor
	cGraphicsScene(cGraphicsEngine& parent, tRenderState state = tRenderState());
	~cGraphicsScene();

	// entities & lights
	IGraphicsEntity* CreateEntity(const wchar_t* geomPath, const wchar_t* mtlPath) override;
	IGraphicsEntity* CreateEntity(IGeometryRef* customGeom, const wchar_t* mtlPath) override;
	void DeleteEntity(const IGraphicsEntity* entity) override;
	IGraphicsLight* CreateLight() override;
	void DeleteLight(const IGraphicsLight* light) override;

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

	// Used by friend classes (Hdr, motionBlur)
	float luminanceAdaptation;
	Matrix44 lastCameraMatrix;
};


////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine
class cGraphicsEngine : public IGraphicsEngine {
	friend class cGraphicsScene;
	class cDeferredRenderer;
	class cHDRProcessor;
	class cPostProcessor;
	class cShadowRenderer;
public:
	// lifecycle (creation, destruction)
	cGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config);
	cGraphicsEngine(const cGraphicsEngine&) = delete;
	~cGraphicsEngine();
	cGraphicsEngine& operator=(const cGraphicsEngine&) = delete;
	void Release() override;

	// rendering pipeline
	eGraphicsResult Update(float elapsed = 0.0f) override;

	eGraphicsResult Resize(unsigned width, unsigned height) override;
	eGraphicsResult ReloadShaders() override;
	
	// scene & geom management
	IGeometryBuilder* CreateCustomGeometry() override;
	IGraphicsScene*	  CreateScene(tRenderState state = tRenderState()) override;

	void DeleteScene(const IGraphicsScene* scene) override;

	// Configurate graphics engine
	eGraphicsResult SetConfig(tGraphicsConfig config) override;

	const char* GetLastErrorMessage() override;
	cResourceManager*	GetResourceManager();
	IGraphicsApi*		GetGraphicsApi() override;

private:
	void RenderScene(cGraphicsScene& scene, ITexture2D* target, float elapsed);
	void RenderForward();
	void RenderDeferred();

	void LoadShaders();
	void UnloadShaders();

	void ReloadBuffers();

	// render states
	static const tDepthStencilDesc depthStencilDefault;
	static const tBlendDesc blendDefault;

	// rendering stuff
	cSceneManager* sceneManager; // temporary storage for current scene state (copied from cScene)
	cCamera* camera;
	Matrix44* lastCameraMatrix;
	float elapsed;
	ITexture2D* currentSceneBuffer;

	// state
	unsigned screenWidth, screenHeight;
	tGraphicsConfig config;

	// sub-compnents: rendering & graphical
	IGraphicsApi* gApi;
	cDeferredRenderer* deferredRenderer;
	cHDRProcessor* hdrProcessor;
	cPostProcessor* postProcessor;
	cShadowRenderer* shadowRenderer;

	cResourceManager* resourceManager;
	std::set<cGraphicsScene*> graphicsScenes;
	std::deque<cGraphicsScene*> graphicsSceneOrder;
	
	// shaders
	IShaderProgram* shaderScreenCopy;

	// misc
	std::string lastErrorMessage;

	////////////////////////////////////////////////
	// --- --- --- --- SUBCLASSES --- --- --- --- //
	////////////////////////////////////////////////

	///////////////////////////////////////
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
		ITexture2D* GetDepthBuffer();

	private:
		void LoadShaders();
		void UnloadShaders();
		void Cleanup();
		eGapiResult ReallocBuffers();

		ITexture2D* gBuffer[3];
		ITexture2D* compositionBuffer;
		//ITexture2D* DOFInput;
		ITexture2D* depthBuffer;
		ITexture2D* depthBufferCopy;
		ITexture2D* ambientOcclusionBuffer;

		IGraphicsApi* gApi;
		IShaderProgram* shaderGBuffer;
		IShaderProgram *shaderDirectional, *shaderPoint, *shaderSpot, *shaderAmbient, *shaderSky;
		IShaderProgram* shaderSSAO, *shaderHBAO;
		//IShaderProgram *shaderDof, *shaderMotionBlur;
		cGraphicsEngine& parent;

		IVertexBuffer *vbSpot, *vbPoint;
		IIndexBuffer *ibSpot, *ibPoint;

		// Remove that also..
		ITexture2D* motionBlurredBuffer;

		unsigned bufferWidth, bufferHeight;
	};

	///////////////////////////////////////
	// General post-processor helper class
	class cPostProcessor {
		friend class cGraphicsEngine;
	public:
		cPostProcessor(cGraphicsEngine& parent);
		cPostProcessor(const cPostProcessor&) = delete;
		~cPostProcessor();
		cPostProcessor& operator=(const cPostProcessor&) = delete;

		// Reload belonging shaders
		void ReloadShaders();

		// Resize buffers etc for that sizes
		eGraphicsResult Resize(unsigned width, unsigned height);

		// Do the post process bull shit
		void RenderComposition(float frameDeltaTime);


		// Need call before "RenderComposition"
		void SetInputBuffers(ITexture2D* srcColor, ITexture2D* srcDepth);
		

		ITexture2D* GetCompositionBuffer();

	private:
		void LoadShaders();
		void UnloadShaders();
		void Cleanup();
		eGapiResult ReallocBuffers();

		// Resulting color texture after post process
		ITexture2D* compositionBuffer;

		// Tmp buffers for swapping between shaders input, output
		ITexture2D* bufferA;
		ITexture2D* bufferB;

		cGraphicsEngine& parent;
		IGraphicsApi* gApi;
	};

	///////////////////////////////////////
	// HDR post-processor helper class
	class cHDRProcessor {
		friend class cGraphicsEngine;
	public:
		cHDRProcessor(cGraphicsEngine& parent);
		~cHDRProcessor();

		void Update(float elapsedSec = -1.0f);
		void ReloadShaders();

		void SetDestination(ITexture2D* dest);
		eGraphicsResult SetSource(ITexture2D* t);

	private:
		void LoadShaders();
		void UnloadShaders();
		void Cleanup();

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
		unsigned sourceWidth, sourceHeight;
	};

	///////////////////////////////////////
	//	Shadow mapper helper class
	class cShadowRenderer {
	public:
		cShadowRenderer(cGraphicsEngine& parent);
		~cShadowRenderer();

		void RenderShadowMaps(cSceneManager& sceneManager);
		void ReloadShaders();

	private:
		void LoadShaders();
		void UnloadShaders();
		void Cleanup();

		IShaderProgram* shaderDirectional;
		IShaderProgram* shaderSpot;
		IShaderProgram* shaderPoint;

		cGraphicsEngine& parent;
		IGraphicsApi* gApi;
	};
};


// Helper function for safely loading shaders
auto SafeLoadShader(IGraphicsApi* gApi, const wchar_t* shader)->IShaderProgram*;
