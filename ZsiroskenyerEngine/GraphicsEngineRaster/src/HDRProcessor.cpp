////////////////////////////////////////////////////////////////////////////////
// Helper class for processing HDR


#include "GraphicsEngine.h"
#include <cassert>
#include <stdexcept>


////////////////////////////////////////////////////////////////////////////////
//	Constructor & destructor
cGraphicsEngine::cHDRProcessor::cHDRProcessor(cGraphicsEngine& parent) : parent(parent) {
	// set vars
	avgLuminance = 0;
	sourceWidth = sourceHeight = 0;
	for (auto& t : luminanceBuffer)
		t = NULL;

	// create luminance textures
	unsigned resolution=1;
	for (int i = 0; i < 9; i++) {
		eGapiResult r = parent.gApi->CreateTexture(&luminanceBuffer[8-i], resolution, resolution, 1, 1, eFormat::R16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
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
}



////////////////////////////////////////////////////////////////////////////////
//	Public interface
eGraphicsResult cGraphicsEngine::cHDRProcessor::SetSource(ITexture2D* srcTexture, unsigned sourceWidth, unsigned sourceHeight) {
	// set internal vars
	this->sourceWidth = sourceWidth;
	this->sourceHeight = sourceHeight;
	this->source = srcTexture;
	assert(srcTexture != NULL);

	// create downsampled buffers
	unsigned width = sourceWidth / 2, height = sourceHeight / 2;
	auto r = parent.gApi->CreateTexture(&downSampled, width, height, 1, 1, eFormat::R16G16B16A16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	if (r != eGapiResult::OK)
		return eGraphicsResult::ERROR_UNKNOWN;
	auto r = parent.gApi->CreateTexture(&blurBuffer, width, height, 1, 1, eFormat::R16G16B16A16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
	if (r != eGapiResult::OK) {
		SAFE_RELEASE(downSampled)
			return eGraphicsResult::ERROR_UNKNOWN;
	}

}

