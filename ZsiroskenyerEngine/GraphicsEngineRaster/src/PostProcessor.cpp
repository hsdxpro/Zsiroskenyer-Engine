////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer tEAM
////////////////////////////////////////////////////////////////////////////////
//	General post-processor for the renderer
//	Supported effects:
//		- depth of field
//		- motion blur
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEngine.h"

cGraphicsEngine::cPostProcessor::cPostProcessor(cGraphicsEngine& parent)
:parent(parent), gApi(parent.gApi)
{
	shaderMotionBlur = nullptr;
	shaderDof = nullptr;
	compositionBuffer = nullptr;
	bufferA = nullptr;
	inputColorTex = nullptr;
	inputDepthTex = nullptr;

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

void cGraphicsEngine::cPostProcessor::RenderComposition(float frameDeltaTime, const cCamera& cam)
{
	//-------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- MOTION BLUR --- --- --- --- --- --- --- --- //
	//-------------------------------------------------------------------------//
	gApi->SetRenderTargets(1, &bufferA, NULL);

	gApi->SetShaderProgram(shaderMotionBlur);

	struct s
	{
	Matrix44 invViewProj;
	Matrix44 prevViewProj;
	float	 frameDeltaTime; float _pad[3];
	} mbConstants;

	Matrix44 viewMat = cam.GetViewMatrix();
	Matrix44 projMat = cam.GetProjMatrix();
	Matrix44 invViewProjMat = Matrix44Inverse(viewMat * projMat);
	mbConstants.invViewProj = invViewProjMat;
	mbConstants.prevViewProj = prevViewMat * projMat;
	mbConstants.frameDeltaTime = frameDeltaTime;

	gApi->SetPSConstantBuffer(&mbConstants, sizeof(mbConstants), 0);

	gApi->SetTexture(L"textureInput", inputColorTex);
	gApi->SetTexture(L"depthTexture", inputDepthTex);

	// asd new
	prevViewMat = viewMat;

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);

	//---------------------------------------
	//-------------------------------------//
	// --- --- --- --- --- --- --- DEPTH OF FIELD --- --- --- --- --- --- --- --- //
	//----------------------------------------------------------------------------//
	gApi->SetRenderTargets(1, &compositionBuffer, NULL);
	gApi->SetShaderProgram(shaderDof);

	struct tDofConstants
	{
	Matrix44 invViewProj;
	Vec3 camPos; float _pad;
	} dofConstants;

	dofConstants.invViewProj = invViewProjMat;
	dofConstants.camPos = cam.GetPos();

	gApi->SetPSConstantBuffer(&dofConstants, sizeof(dofConstants), 0);

	gApi->SetTexture(L"inputTexture", bufferA);
	gApi->SetTexture(L"depthTexture", inputDepthTex);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
}

// Need call before "RenderComposition"
void cGraphicsEngine::cPostProcessor::SetInputBuffers(ITexture2D* srcColor, ITexture2D* srcDepth) {

	// Lazy creation
	if (!inputDepthTex && !inputDepthTex  // There are no textures
		
		// Or size not match
		|| (inputColorTex && inputColorTex->GetWidth() != srcColor->GetWidth() && 
			inputColorTex->GetHeight() != srcColor->GetHeight()))
	{
		ITexture2D::tDesc d;
			d.arraySize = 1;
			d.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
			d.format = eFormat::R8G8B8A8_UNORM;
			d.height = srcColor->GetHeight();
			d.width = srcColor->GetWidth();
			d.mipLevels = 1;
			d.usage = eUsage::DEFAULT;
		eGapiResult errCode = gApi->CreateTexture(&bufferA, d);

		if (errCode != eGapiResult::OK)
		{
			Cleanup();
			throw std::runtime_error("failed to create texture buffers");
		}

		errCode = gApi->CreateTexture(&compositionBuffer, d);

		if (errCode != eGapiResult::OK) {
			Cleanup();
			throw std::runtime_error("failed to create texture buffers");
		}
		
		inputColorTex = srcColor;
		inputDepthTex = srcDepth;
	}
}

ITexture2D* cGraphicsEngine::cPostProcessor::GetCompositionBuffer() {
	return compositionBuffer;
}





// INNNNERR
void cGraphicsEngine::cPostProcessor::Cleanup() {
	// release shaders
	UnloadShaders();

	// release all textures
	SAFE_RELEASE(compositionBuffer);
	SAFE_RELEASE(bufferA);
}

// load shaders
void cGraphicsEngine::cPostProcessor::LoadShaders() {
	auto Create = [this](const wchar_t* shader)->IShaderProgram* {
		return SafeLoadShader(gApi, shader);
	};
	try {
		shaderMotionBlur = Create(L"shaders/motion_blur.cg");
		shaderDof = Create(L"shaders/depth_of_field.cg");
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
}
