////////////////////////////////////////////////////////////////////////////////
// Helper class for processing HDR


#include "GraphicsEngine.h"
#include "ShaderManager.h"
#include <cassert>
#include <stdexcept>
#include <algorithm>


////////////////////////////////////////////////////////////////////////////////
//	Constructor & destructor
cGraphicsEngine::cHDRProcessor::cHDRProcessor(cGraphicsEngine& parent) : parent(parent), gApi(parent.gApi) {
	// set vars
	avgLuminance = 0;
	adaptedLuminance = 0;
	sourceWidth = sourceHeight = 0;
	blurBuffer = NULL;
	downSampled = NULL;
	luminanceStaging = NULL;
	source = NULL;
	dest = NULL;
	for (auto& t : luminanceBuffer)
		t = NULL;

	// create shaders
	shaderLumSample = parent.shaderManager->LoadShader("shaders/hdr_luminance_sample.cg");
	shaderLumAvg = parent.shaderManager->LoadShader("shaders/hdr_luminance_avg.cg");
	shaderCompose = parent.shaderManager->LoadShader("shaders/hdr_compose.cg");
	if (!shaderLumSample || !shaderLumAvg) {
		Cleanup();
		throw std::runtime_error("failed to create shaders");
	}

	// create constant buffers
	eGapiResult rcb = gApi->CreateConstantBuffer(&cbCompose, sizeof(float)+12, eUsage::DEFAULT);
	if (rcb != eGapiResult::OK) {
		Cleanup();
		throw std::runtime_error("failed to create shader constant buffers");
	}

	// create luminance textures
	unsigned resolution=1;
	ITexture2D::tDesc desc;
	
	desc.format = eFormat::R32_FLOAT;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.usage = eUsage::DEFAULT;
	for (int i = 0; i < 10; i++) {
		desc.width = desc.height = resolution;
		eGapiResult r = parent.gApi->CreateTexture(&luminanceBuffer[9-i], desc);
		if (r != eGapiResult::OK) {
			Cleanup();
			throw std::runtime_error("failed to create luminance buffers");
		}
		resolution *= 2;
	}

	desc.bind = 0;
	desc.usage = eUsage::STAGING;
	desc.width = desc.height = 1;
	eGapiResult r = parent.gApi->CreateTexture(&luminanceStaging, desc);
	if (r != eGapiResult::OK) {
		Cleanup();
		throw std::runtime_error("failed to create luminance cpu copy buffer");
	}
}

cGraphicsEngine::cHDRProcessor::~cHDRProcessor() {
	// release all textures
	Cleanup();
}

void cGraphicsEngine::cHDRProcessor::Cleanup() {
	// release all textures
	for (auto& t : luminanceBuffer)
		SAFE_RELEASE(t);
	SAFE_RELEASE(blurBuffer);
	SAFE_RELEASE(downSampled);
	SAFE_RELEASE(luminanceStaging);
}



////////////////////////////////////////////////////////////////////////////////
//	Methods

//	Set HDR range float buffer
eGraphicsResult cGraphicsEngine::cHDRProcessor::SetSource(ITexture2D* srcTexture, unsigned sourceWidth, unsigned sourceHeight) {
	// release old downsampled buffers
	SAFE_RELEASE(blurBuffer);
	SAFE_RELEASE(downSampled);

	// set internal vars
	this->sourceWidth = sourceWidth;
	this->sourceHeight = sourceHeight;
	this->source = srcTexture;
	assert(srcTexture != NULL);

	// create downsampled buffers
	ITexture2D::tDesc desc;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.format = eFormat::R16G16B16A16_FLOAT;
	desc.width = sourceWidth / 2;
	desc.height = sourceHeight / 2;
	auto r = parent.gApi->CreateTexture(&downSampled, desc);
	if (r != eGapiResult::OK) {
		return eGraphicsResult::ERROR_UNKNOWN;
	}
	r = parent.gApi->CreateTexture(&blurBuffer, desc);
	if (r != eGapiResult::OK) {
		SAFE_RELEASE(downSampled)
		return eGraphicsResult::ERROR_UNKNOWN;
	}

	return eGraphicsResult::OK;
}

// Set LDR output buffer
void cGraphicsEngine::cHDRProcessor::SetDestination(ITexture2D* dest) {
	this->dest = dest;
}

////////////////////////////////////////////////////////////////////////////////
//	Update
#include <iostream>
#include <chrono>
auto timePrev = std::chrono::high_resolution_clock::now();
float elapsedTotal = 0.0f;
void cGraphicsEngine::cHDRProcessor::Update(float elapsedSec) {
	// get a fucking elapsed time... ugly, remove this or i kill myself
	auto timeNow = std::chrono::high_resolution_clock::now();
	float elapsed = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeNow - timePrev).count() / 1e6f;
	timePrev = timeNow;
	elapsedTotal += elapsed;

	// calculate average luminance
	gApi->SetShaderProgram(shaderLumSample);
	gApi->SetRenderTargets(1, &luminanceBuffer[0], NULL);
	gApi->SetTexture(source, 0);
	gApi->Draw(3);

	parent.gApi->SetShaderProgram(shaderLumAvg);
	for (int i = 1; i < 10; i++) {
		gApi->SetRenderTargets(1, &luminanceBuffer[i], NULL);
		gApi->SetTexture(luminanceBuffer[i - 1], 0);
		gApi->Draw(3);
	}

	// extract average luminance from buffer
	auto r = gApi->CopyResource(luminanceBuffer[9], luminanceStaging);
	gApi->ReadResource(luminanceStaging, &avgLuminance, sizeof(float));

	if (elapsedTotal >= 1.0f) {
		std::cout << "Avg. luminance = " << avgLuminance << ", log10(lum) =  " << log10(avgLuminance) << std::endl;
		elapsedTotal = 0.0f;
	}

	// downsample that bullshit for blurring
	/* TODO */

	// compose to destination buffer
	float speed = 0.9f;
	float t = std::max(0.0f, speed - elapsed) / speed;
	adaptedLuminance = avgLuminance*(1.0 - t) + t*adaptedLuminance;
	float logAvgLum = log10(adaptedLuminance);
	gApi->SetRenderTargets(1, &dest);
	gApi->SetTexture(source, 0);
	gApi->SetShaderProgram(shaderCompose);
	gApi->SetConstantBufferData(cbCompose, &logAvgLum);
	gApi->SetPSConstantBuffer(cbCompose, 0);
	gApi->Draw(3);

}

