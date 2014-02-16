////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Small shadow map texture and misc management helper class
//
////////////////////////////////////////////////////////////////////////////////


#include "ShadowMap.h"
#include "../../Core/src/GAPI.h"
#include <algorithm>


////////////////////////////////////////////////////////////////////////////////
// contructors
cShadowMap::cShadowMap()
:
resolution(0),
gApi(nullptr)
{
}

cShadowMap::cShadowMap(IGraphicsApi* gApi, cGraphicsLight::eLightType type, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	Init(gApi, type, resolution, readFormat, depthFormat, cascades);
}

cShadowMap::~cShadowMap() {
	Clear();
}


////////////////////////////////////////////////////////////////////////////////
//	initialization

// delete old, create new
void cShadowMap::Init(IGraphicsApi* gApi, cGraphicsLight::eLightType type, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	// clear old
	Clear();

	// idiot-proof
	assert(cascades >= 1);

	// store vars
	this->resolution = resolution;
	this->gApi = gApi;
	this->readFormat = readFormat;
	this->depthFormat = depthFormat;
	this->type = type;

	// alloc space
	maps.resize(cascades, nullptr);
	// create textures
	ITexture2D::tDesc desc;
	desc.arraySize = type==cGraphicsLight::POINT ? 6 : 1;
	desc.bind = (int)eBind::SHADER_RESOURCE || (int)eBind::DEPTH_STENCIL;
	desc.depthFormat = depthFormat;
	desc.format = readFormat;
	desc.width = resolution;
	desc.height = resolution;
	desc.mipLevels = 1;
	desc.usage = eUsage::DEFAULT;
	for (auto& v : maps) {
		auto r = gApi->CreateTexture(&v, desc);
		if (r != eGapiResult::OK) {
			Clear();
			throw std::runtime_error("unknown error");
		}
	}
}

// release buffers
void cShadowMap::Clear() {
	for (auto& v : maps) {
		SAFE_RELEASE(v);
	}
	maps.clear();
}

// return whether we can just reuse it, or we must create new buffers
bool cShadowMap::IsCompatible(IGraphicsApi* gApi, cGraphicsLight::eLightType type, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	return (this->gApi == gApi &&
		this->resolution == resolution &&
		this->readFormat == readFormat &&
		this->depthFormat == depthFormat &&
		this->type == type &&
		maps.size() == cascades);
}

////////////////////////////////////////////////////////////////////////////////
// is it initialized at all?
cShadowMap::operator bool() {
	return (maps.size() > 0);
}

////////////////////////////////////////////////////////////////////////////////
// getters
IGraphicsApi* cShadowMap::GetGraphicsApi() const {
	return gApi;
}

eFormat cShadowMap::GetReadFormat() const {
	return readFormat;
}
eFormat cShadowMap::GetDepthFormat() const {
	return depthFormat;
}

const std::vector<ITexture2D*>& cShadowMap::GetMaps() const {
	return maps;
}

int cShadowMap::GetNumCascades() const {
	return maps.size();
}
