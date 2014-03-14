////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer tEAM
////////////////////////////////////////////////////////////////////////////////
//	General post-processor for the renderer
//	Supported effects:
//		- depth of field
//		- motion blur
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEngine.h"

// TODO REMOVE IT OR I KILL MYSELF
#include <windows.h>

cGraphicsEngine::cPostProcessor::cPostProcessor(cGraphicsEngine& parent)
:parent(parent), gApi(parent.gApi)
{
	// Shaders
	shaderMotionBlur = nullptr;
	shaderDof = nullptr;
	shaderFXAA = nullptr;

	// Input textures
	inputTexColor = nullptr;
	inputTexDepth = nullptr;

	outputTexColor = nullptr;

	// create shaders
	try {
		LoadShaders();
	}
	catch (std::exception& e) {
		Cleanup();
		throw std::runtime_error(std::string("failed to create shaders:\n") + e.what());
	}
}

cGraphicsEngine::cPostProcessor::~cPostProcessor() {
	// release all textures
	Cleanup();
}

// Motion Blur
void cGraphicsEngine::cPostProcessor::ProcessMB(float frameDeltaTime, const cCamera& cam) {
	assert(outputTexColor);

	gApi->SetRenderTargets(1, &outputTexColor, NULL);
	gApi->SetShaderProgram(shaderMotionBlur);

	struct s
	{
		Matrix44 invViewProj;
		Matrix44 prevViewProj;
		float	 frameDeltaTime; float _pad[3];
	} mbConstants;

	Matrix44 viewMat = cam.GetViewMatrix();
	Matrix44 projMat = cam.GetProjMatrix();
	mbConstants.invViewProj = Matrix44Inverse(viewMat * projMat);
	mbConstants.prevViewProj = prevViewMat * projMat;
	mbConstants.frameDeltaTime = frameDeltaTime;

	gApi->SetPSConstantBuffer(&mbConstants, sizeof(mbConstants), 0);

	gApi->SetTexture(L"textureInput", inputTexColor);
	gApi->SetTexture(L"depthTexture", inputTexDepth);

	// asd new
	prevViewMat = viewMat;

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
}

// Dof
void cGraphicsEngine::cPostProcessor::ProcessDOF(const cCamera& cam) {
	gApi->SetRenderTargets(1, &outputTexColor, NULL);
	gApi->SetShaderProgram(shaderDof);

	struct tDofConstants
	{
		Matrix44 invViewProj;
		Vec3 camPos; float _pad;
	} dofConstants;

	dofConstants.invViewProj = Matrix44Inverse(cam.GetViewMatrix() *  cam.GetProjMatrix());
	dofConstants.camPos = cam.GetPos();

	gApi->SetPSConstantBuffer(&dofConstants, sizeof(dofConstants), 0);

	gApi->SetTexture(L"inputTexture", inputTexColor);
	gApi->SetTexture(L"depthTexture", inputTexDepth);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
}


// FXAA
void cGraphicsEngine::cPostProcessor::ProcessFXAA() {
	gApi->SetRenderTargets(1, &outputTexColor, NULL);
	gApi->SetShaderProgram(shaderFXAA);

	Vec4 invResolution(	1.0f / outputTexColor->GetWidth(),
						1.0f / outputTexColor->GetHeight(),
						0, // pad
						0);// pad
	
	gApi->SetPSConstantBuffer(&invResolution, sizeof(Vec4), 0);

	gApi->SetTexture(L"inputTexture", inputTexColor);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
}

// Set inputs
void cGraphicsEngine::cPostProcessor::SetInputMB(ITexture2D* color, ITexture2D* depth) {
	inputTexColor = color; inputTexDepth = depth;
}

void cGraphicsEngine::cPostProcessor::SetInputDOF(ITexture2D* color, ITexture2D* depth) {
	inputTexColor = color; inputTexDepth = depth;
}

// If FXAA shader defines that it takes luminance from alpha channel, then pass appropriate texture please..
void cGraphicsEngine::cPostProcessor::SetInputFXAA(ITexture2D* color) {
	inputTexColor = color;
}

// Set outputs
void cGraphicsEngine::cPostProcessor::SetOutput(ITexture2D* t) {
	outputTexColor = t;
}

// INNNNERR
void cGraphicsEngine::cPostProcessor::Cleanup() {
	// release shaders
	UnloadShaders();
}

// load shaders
void cGraphicsEngine::cPostProcessor::LoadShaders() {
	auto Create = [this](const wchar_t* shader)->IShaderProgram* {
		return SafeLoadShader(gApi, shader);
	};
	try {
		shaderMotionBlur = Create(L"shaders/motion_blur.cg");
		shaderDof = Create(L"shaders/depth_of_field.cg");
		shaderFXAA = Create(L"shaders/FXAA.cg");
	}
	catch (...) {
		UnloadShaders();
		throw;
	}
}

// unload shaders
void cGraphicsEngine::cPostProcessor::UnloadShaders() {
	SAFE_RELEASE(shaderMotionBlur);
	SAFE_RELEASE(shaderDof);
	SAFE_RELEASE(shaderFXAA);
}

// reload shaders
void cGraphicsEngine::cPostProcessor::ReloadShaders() {
	auto Reload = [this](IShaderProgram** prog, const wchar_t* name)->void {
		IShaderProgram* tmp = SafeLoadShader(gApi, name); // it throws on error!
		(*prog)->Release();
		*prog = tmp;
	};
	Reload(&shaderMotionBlur, L"shaders/motion_blur.cg");
	Reload(&shaderDof, L"shaders/depth_of_field.cg");
	Reload(&shaderDof, L"shaders/FXAA.cg");
}
