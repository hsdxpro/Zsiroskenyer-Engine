////////////////////////////////////////////////////////////////////////////////
// Helper class for processing HDR


#include "GraphicsEngine.h"

//	Constructor & destructor
cGraphicsEngine::cHDRProcessor::cHDRProcessor(cGraphicsEngine& parent) : parent(parent) {
	// set vars
	avgLuminance = 0;
	for (auto& t : luminanceBuffer)
		t = NULL;

	// create textures
	unsigned resolution=1;
	for (int i = 0; i < 9; i++) {
		eGapiResult r = parent.gApi->CreateTexture(&luminanceBuffer[8-i], resolution, resolution, 1, 1, eFormat::R16_FLOAT, (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE);
		resolution *= 2;
	}
}

cGraphicsEngine::cHDRProcessor::~cHDRProcessor() {

}

