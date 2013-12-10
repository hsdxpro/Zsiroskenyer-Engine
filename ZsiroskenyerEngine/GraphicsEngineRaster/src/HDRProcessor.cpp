////////////////////////////////////////////////////////////////////////////////
// Helper class for processing HDR


#include "GraphicsEngine.h"
#include "ShaderManager.h"
#include <cassert>
#include <stdexcept>


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
	for (auto& t : luminanceBuffer)
		t = NULL;

	// create shaders
	shaderLumSample = parent.shaderManager->LoadShader("shaders/hdr_luminance_sample.cg");
	shaderLumAvg = parent.shaderManager->LoadShader("shaders/hdr_luminance_avg.cg");
	if (!shaderLumSample || !shaderLumAvg) {
		Cleanup();
		throw std::runtime_error("failed to create shaders");
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


////////////////////////////////////////////////////////////////////////////////
//	Update
#include <iostream>

void cGraphicsEngine::cHDRProcessor::Update(float elapsedSec) {
	// Calculate average luminance
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

	// TODO: avgLuminance = ...;
	auto r = gApi->CopyResource(luminanceBuffer[9], luminanceStaging);
	gApi->ReadResource(luminanceStaging, &avgLuminance, sizeof(float));

	std::cout << avgLuminance;



}

