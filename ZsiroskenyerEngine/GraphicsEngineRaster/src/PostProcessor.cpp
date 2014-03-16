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
	shaderFocalPlaneAdaption = nullptr;
	shaderFXAA = nullptr;

	// Input textures
	inputTexColor = nullptr;
	inputTexDepth = nullptr;

	// Output textures
	outputTexColor = nullptr;
	outputTexVelocity2D = nullptr;

	// self textures
	focalPlaneTexA = nullptr;
	focalPlaneTexB = nullptr;

	// create shaders
	try {
		LoadShaders();
	}
	catch (std::exception& e) {
		Cleanup();
		throw std::runtime_error(std::string("cPostProcessor: Failed to create shaders:\n") + e.what());
	}

	try {
		ReallocBuffers();
	} 
	catch (std::exception& e) {
		Cleanup();
		throw std::runtime_error(std::string("cPostProcessor: Failed to realloc buffers\n") + e.what());
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
	gApi->SetRenderTargets(1, &outputTexVelocity2D, nullptr);
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
	gApi->SetRenderTargets(1, &outputTexColor, nullptr);
	gApi->SetShaderProgram(shaderMB);
	gApi->SetTexture(L"velocity2DTex", outputTexVelocity2D);
	gApi->SetTexture(L"inputTexture", inputTexColor);
	gApi->Draw(3);
}

void cGraphicsEngine::cPostProcessor::ProcessDOF(const cCamera& cam) {

	// These constants shared across two DOF specific shaders...
	// Pass1 : FocalLength adaption, only uses invViewProj, camPos
	// Pass2 : CoC calc and blur, uses all of these shits
	struct tDofConstants
	{
		Matrix44 invViewProj;
		Vec3 camPos;								float _pad;
		float invRetinaRadiusProductInputTexWidth;	Vec3  _pad1;
		float		invTexWidth;					Vec3  _pad2;
		float		invTexHeight;					Vec3  _pad3;
		float		MinusInvTexWidth;				Vec3  _pad4;
		float		MinusInvTexHeight;				Vec3  _pad5;
		float		aperture;						Vec3  _pad6;
		float		retinaLensDist;					Vec3  _pad7;
	} dofConstants;

	dofConstants.invRetinaRadiusProductInputTexWidth = (float)inputTexColor->GetWidth() * 26.793927f; // That magic number (27.793927) normalizes a CoC that belonging to an average sized human eye and lens into [0,1]
	dofConstants.invViewProj = Matrix44Inverse(cam.GetViewMatrix() *  cam.GetProjMatrix());
	dofConstants.invTexWidth = 1.0f / inputTexColor->GetWidth();
	dofConstants.invTexHeight = 1.0f / inputTexColor->GetHeight();
	dofConstants.MinusInvTexWidth = -dofConstants.invTexWidth;
	dofConstants.MinusInvTexHeight = -dofConstants.invTexHeight;
	dofConstants.retinaLensDist = 0.011f;
	dofConstants.aperture = 0.022f;
	dofConstants.camPos = cam.GetPos();

	// Set it for shaders to use
	gApi->SetPSConstantBuffer(&dofConstants, sizeof(dofConstants), 0);



	//-------------------------------------------------------------------------------------//
	// --------------------------- FIRST PASS : ADAPT FOCAL PLANE -------------------------//
	//-------------------------------------------------------------------------------------//
	gApi->SetShaderProgram(shaderFocalPlaneAdaption);
	gApi->SetRenderTargets(1, &focalPlaneTexB);
	gApi->SetTexture(L"inputFocalPlaneTex", focalPlaneTexA);	// Input focal plane
	gApi->SetTexture(L"depthTexture", inputTexDepth);			// Input depth  (need posW)
	gApi->Draw(3);

	// So the result now in 'A', next time we use that to render again into B, then use a....
	std::swap(focalPlaneTexA, focalPlaneTexB);



	//-------------------------------------------------------------------------------------//
	//----------------Circle Of Confusion Calc  AND  Bluring Based On It ------------------//
	//-------------------------------------------------------------------------------------//
	gApi->SetRenderTargets(1, &outputTexColor, nullptr);
	gApi->SetShaderProgram(shaderDOF);
	gApi->SetTexture(L"adaptedFocalPlaneTex", focalPlaneTexA);	// Input focal plane
	gApi->SetTexture(L"inputTexture", inputTexColor);			// Input color
	gApi->SetTexture(L"depthTexture", inputTexDepth);			// Input depth (need posW)
	gApi->Draw(3);
}


// FXAA
void cGraphicsEngine::cPostProcessor::ProcessFXAA() {
	gApi->SetRenderTargets(1, &outputTexColor, nullptr);
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
	// Release shaders
	UnloadShaders();

	// Release textures
	SAFE_RELEASE(focalPlaneTexA);
	SAFE_RELEASE(focalPlaneTexB);
}

// load shaders
void cGraphicsEngine::cPostProcessor::LoadShaders() {
	auto Create = [this](const wchar_t* shader)->IShaderProgram* {
		return SafeLoadShader(gApi, shader);
	};
	try {
		shaderMB				 = Create(L"shaders/motion_blur.cg");
		shaderMB2DVelocity		 = Create(L"shaders/motion_blur_2dvelocity.cg");
		shaderDOF				 = Create(L"shaders/depth_of_field.cg");
		shaderFocalPlaneAdaption = Create(L"shaders/depth_of_field_focal_plane_adaption.cg");
		shaderFXAA				 = Create(L"shaders/fxaa.cg");
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
	Reload(&shaderMB,					L"shaders/motion_blur.cg");
	Reload(&shaderMB2DVelocity,			L"shaders/motion_blur_2dvelocity.cg");
	Reload(&shaderDOF,					L"shaders/depth_of_field.cg");
	Reload(&shaderFocalPlaneAdaption,	L"shaders/depth_of_field_focal_plane_adaption.cg");
	Reload(&shaderFXAA,					L"shaders/fxaa.cg");
}

eGapiResult cGraphicsEngine::cPostProcessor::ReallocBuffers() {
	SAFE_RELEASE(focalPlaneTexA);
	SAFE_RELEASE(focalPlaneTexB);

	ITexture2D::tDesc d;
		d.arraySize = 1;
		d.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
		d.format = eFormat::R16_FLOAT;
		d.width = 1;
		d.height = 1;
		d.mipLevels = 1;
		d.usage = eUsage::DEFAULT;

	eGapiResult errCode = gApi->CreateTexture(&focalPlaneTexA, d);
	if (errCode != eGapiResult::OK) {
		throw std::runtime_error("cPostProcessor: Failed to realloc 1x1 focalPlane texture A");
		return errCode;
	}

	errCode = gApi->CreateTexture(&focalPlaneTexB, d);
	if (errCode != eGapiResult::OK) {
		throw std::runtime_error("cPostProcessor: Failed to realloc 1x1 focalPlane texture B");
		return errCode;
	}

	// Important to clear Textures because adaption start from dist 0
	gApi->ClearTexture(focalPlaneTexA);
	gApi->ClearTexture(focalPlaneTexB);
	return eGapiResult::OK;
}