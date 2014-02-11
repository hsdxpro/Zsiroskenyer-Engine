////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Small shadow map texture and misc management helper class
//
////////////////////////////////////////////////////////////////////////////////


#include "ShadowMap.h"
#include "../../Core/src/GAPI.h"


////////////////////////////////////////////////////////////////////////////////
// contructors
cShadowMap::cShadowMap()
:
resolution(0),
gApi(nullptr)
{
}

cShadowMap::cShadowMap(IGraphicsApi* gApi, unsigned resolution, eFormat format, int cascades) {
	Init(gApi, resolution, format, cascades);
}

cShadowMap::~cShadowMap() {
	Clear();
}


////////////////////////////////////////////////////////////////////////////////
//	initialization
void cShadowMap::Init(IGraphicsApi* gApi, unsigned resolution, eFormat format, int cascades) {
	// clear old
	Clear();

	// idiot-proof
	assert(cascades >= 1);

	// store vars
	this->resolution = resolution;
	this->gApi = gApi;
	this->format = format;

	// alloc space
	maps.resize(cascades, nullptr);
	// create textures
	ITexture2D::tDesc desc;
	desc.arraySize = 1;
	desc.bind = (int)eBind::SHADER_RESOURCE || (int)eBind::DEPTH_STENCIL;
	desc.depthFormat = format;
	desc.format = format;
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


void cShadowMap::Clear() {
	for (auto& v : maps) {
		SAFE_RELEASE(v);
	}
	maps.clear();
}

bool cShadowMap::IsCompatible(IGraphicsApi* gApi, unsigned resolution, eFormat format, int cascades) {
	return (this->gApi == gApi &&
		this->resolution == resolution &&
		this->format == format &&
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

eFormat cShadowMap::GetFormat() const {
	return format;
}

const std::vector<ITexture2D*>& cShadowMap::GetMaps() const {
	return maps;
}

int cShadowMap::GetNumCascades() const {
	return maps.size();
}


////////////////////////////////////////////////////////////////////////////////
// fathermucking view and projection matrices
bool cShadowMap::ViewProjMatrix(
	Matrix44& projOut,
	Matrix44& viewOut,
	const cGraphicsLight& light,
	const Matrix44& cameraView,
	const Matrix44& cameraProj,
	float nearClip,
	float farClip)
{
	// check out that bullcrap :)
	assert(nearClip < farClip);
	assert(0.0f <= nearClip);
	assert(farClip <= 1.0f);

	// get the frustum's 8 points
	// layout is like:
		// 3,2 - these are array indices
		// 0,1 - 2d "schematic" when looking from camera
	Vec3 frustumPoints[8] = {
		{-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0},
		{-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1},
	};
	// inverse transform them from clip space
	Matrix44 invCameraViewProj = Matrix44Inverse(cameraView*cameraProj);
	for (auto& v : frustumPoints) {
		Vec4 v4 = v;
		v4 *= invCameraViewProj;
		v = v4 / v4.w;
	}
	// interpolate for near and far clips
	for (int i = 0; i < 4; i++) {
		auto tmp = frustumPoints[i];
		frustumPoints[i] = (1 - nearClip)*frustumPoints[i] + nearClip*frustumPoints[i + 4];
		frustumPoints[i+1] = (1 - farClip)*frustumPoints[i] + farClip*frustumPoints[i + 4];
	}

	// compute matrix from eight points

	// na bazmeg csak hogy forduljon!!!4!
	return false;
}
