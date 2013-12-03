////////////////////////////////////////////////////////////////////////////////
// Helper class for processing HDR


#include "GraphicsEngine.h"
#include "ShaderManager.h"
#include <cassert>
#include <stdexcept>


////////////////////////////////////////////////////////////////////////////////
//	Constructor & destructor
cGraphicsEngine::cHDRProcessor::cHDRProcessor(cGraphicsEngine& parent) : parent(parent) {
	// set vars
	avgLuminance = 0;
	adaptedLuminance = 0;
	sourceWidth = sourceHeight = 0;
	for (auto& t : luminanceBuffer)
		t = NULL;

	// create shaders


	// create luminance textures
	unsigned resolution=1;
	ITexture2D::tDesc desc;
	
	desc.format = eFormat::R16_FLOAT;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	for (int i = 0; i < 9; i++) {
		desc.width = desc.height = resolution;
		eGapiResult r = parent.gApi->CreateTexture(&luminanceBuffer[8-i], desc);
		if (r != eGapiResult::OK) {
			// release all textures
			for (auto& t : luminanceBuffer)
				SAFE_RELEASE(t);
			// throw error
			throw std::runtime_error("failed to create luminance buffers");
		}
		resolution *= 2;
	}
}

cGraphicsEngine::cHDRProcessor::~cHDRProcessor() {
	// release all textures
	for (auto& t : luminanceBuffer)
		SAFE_RELEASE(t);
	SAFE_RELEASE(blurBuffer);
	SAFE_RELEASE(downSampled);
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
	if (r != eGapiResult::OK)
		return eGraphicsResult::ERROR_UNKNOWN;
	r = parent.gApi->CreateTexture(&blurBuffer, desc);
	if (r != eGapiResult::OK) {
		SAFE_RELEASE(downSampled)
		return eGraphicsResult::ERROR_UNKNOWN;
	}

	return eGraphicsResult::OK;
}


////////////////////////////////////////////////////////////////////////////////
//	Update

void cGraphicsEngine::cHDRProcessor::Update(float elapsedSec) {
	// Calculate average luminance
	parent.gApi->SetRenderTargets(1, &luminanceBuffer[0], NULL);
	parent.gApi->SetShaderProgram(parent.shaderManager->GetShaderByName("hdr_luminance_sample"));

	parent.gApi->SetShaderProgram(parent.shaderManager->GetShaderByName("hdr_luminance_avg"));
	for (int i = 1; i < 9; i++) {
		parent.gApi->SetRenderTargets(1, &luminanceBuffer[i], NULL);
	}



}

