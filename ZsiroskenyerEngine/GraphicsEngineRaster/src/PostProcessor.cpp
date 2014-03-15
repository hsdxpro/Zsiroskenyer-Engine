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
	shaderMB = nullptr;
	shaderMB2DVelocity = nullptr;
	shaderDOF = nullptr;
	shaderFXAA = nullptr;

	// Input textures
	inputTexColor = nullptr;
	inputTexDepth = nullptr;

	outputTexColor = nullptr;
	outputTexVelocity2D = nullptr;

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

	//-------------------------------------------------------------------------------------//
	// -------------------- FIRST PASS : VELOCITY 2D BUFFER FILLING -----------------------//
	//-------------------------------------------------------------------------------------//
	gApi->SetRenderTargets(1, &outputTexVelocity2D, NULL);
	gApi->SetShaderProgram(shaderMB2DVelocity);

	struct s
	{
		Matrix44 invViewProj;
		Matrix44 prevViewProj;
		Vec2	 InvframeDeltaTimeDiv2ProductInputRes; Vec2 pad0;// 1.0f / frameDeltaTime / 2 * inputTexResolution
		Vec2	 minMaxPixelVel; Vec2 pad1;///clamp(x, -4, 4) minMax pixel velocity
	} mbConstants;

	Matrix44 viewMat = cam.GetViewMatrix();
	Matrix44 projMat = cam.GetProjMatrix();
	mbConstants.invViewProj = Matrix44Inverse(viewMat * projMat);
	mbConstants.prevViewProj = lastViewMat * projMat;
	mbConstants.InvframeDeltaTimeDiv2ProductInputRes = Vec2(1.0f / (frameDeltaTime * 2.0f * inputTexDepth->GetWidth()),
															1.0f / (frameDeltaTime * 2.0f * inputTexDepth->GetHeight()));

	mbConstants.minMaxPixelVel = Vec2( -4.0f / inputTexDepth->GetWidth(),
										4.0f / inputTexDepth->GetWidth());

	gApi->SetPSConstantBuffer(&mbConstants, sizeof(mbConstants), 0);
	gApi->SetTexture(L"depthTexture", inputTexDepth);

	// Current cam became last
	lastViewMat = viewMat;

	gApi->Draw(3);

	//-------------------------------------------------------------------------------------//
	// -------------------SECOND PASS : USE VELOCITY BUFFER FOR BLURING--------------------//
	//-------------------------------------------------------------------------------------//
	gApi->SetRenderTargets(1, &outputTexColor, NULL);
	gApi->SetShaderProgram(shaderMB);
	gApi->SetTexture(L"velocity2DTex", outputTexVelocity2D);
	gApi->SetTexture(L"inputTexture", inputTexColor);
	gApi->Draw(3);
}

// Dof
void cGraphicsEngine::cPostProcessor::ProcessDOF(const cCamera& cam) {
	gApi->SetRenderTargets(1, &outputTexColor, NULL);
	gApi->SetShaderProgram(shaderDOF);

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
	gApi->Draw(3);
}

// Set inputs
void cGraphicsEngine::cPostProcessor::SetInputMB(ITexture2D* color, ITexture2D* depth) {
	assert(color); assert(depth);
	inputTexColor = color; inputTexDepth = depth;
}

void cGraphicsEngine::cPostProcessor::SetInputDOF(ITexture2D* color, ITexture2D* depth) {
	assert(color); assert(depth);
	inputTexColor = color; inputTexDepth = depth;
}

// If FXAA shader defines that it takes luminance from alpha channel, then pass appropriate texture please..
void cGraphicsEngine::cPostProcessor::SetInputFXAA(ITexture2D* color) {
	assert(color);
	inputTexColor = color;
}

void cGraphicsEngine::cPostProcessor::SetOutputMB(ITexture2D* color, ITexture2D* velocity2D) {
	assert(color); assert(velocity2D);
	outputTexColor = color;
	outputTexVelocity2D = velocity2D;
}

// Set outputs
void cGraphicsEngine::cPostProcessor::SetOutputDOF(ITexture2D* color) {
	assert(color);
	outputTexColor = color;
}

void cGraphicsEngine::cPostProcessor::SetOutputFXAA(ITexture2D* color) {
	assert(color);
	outputTexColor = color;
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
		shaderMB			= Create(L"shaders/motion_blur.cg");
		shaderMB2DVelocity	= Create(L"shaders/motion_blur_2dvelocity.cg");
		shaderDOF			= Create(L"shaders/depth_of_field.cg");
		shaderFXAA			= Create(L"shaders/FXAA.cg");
	}
	catch (...) {
		UnloadShaders();
		throw;
	}
}

// unload shaders
void cGraphicsEngine::cPostProcessor::UnloadShaders() {
	SAFE_RELEASE(shaderMB);
	SAFE_RELEASE(shaderDOF);
	SAFE_RELEASE(shaderFXAA);
}

// reload shaders
void cGraphicsEngine::cPostProcessor::ReloadShaders() {
	auto Reload = [this](IShaderProgram** prog, const wchar_t* name)->void {
		IShaderProgram* tmp = SafeLoadShader(gApi, name); // it throws on error!
		(*prog)->Release();
		*prog = tmp;
	};
	Reload(&shaderMB,			L"shaders/motion_blur.cg");
	Reload(&shaderMB2DVelocity, L"shaders/motion_blur_2dvelocity.cg");
	Reload(&shaderDOF,			L"shaders/depth_of_field.cg");
	Reload(&shaderFXAA,			L"shaders/FXAA.cg");
}
