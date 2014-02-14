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

cShadowMap::cShadowMap(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	Init(gApi, resolution, readFormat, depthFormat, cascades);
}

cShadowMap::~cShadowMap() {
	Clear();
}


////////////////////////////////////////////////////////////////////////////////
//	initialization

// delete old, create new
void cShadowMap::Init(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	// clear old
	Clear();

	// idiot-proof
	assert(cascades >= 1);

	// store vars
	this->resolution = resolution;
	this->gApi = gApi;
	this->readFormat = readFormat;
	this->depthFormat = depthFormat;

	// alloc space
	maps.resize(cascades, nullptr);
	// create textures
	ITexture2D::tDesc desc;
	desc.arraySize = 1;
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
bool cShadowMap::IsCompatible(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	return (this->gApi == gApi &&
		this->resolution == resolution &&
		this->readFormat == readFormat &&
		this->depthFormat == depthFormat &&
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

	// ONLY DIRECTIONAL LIGHTS!
	// rotate them pointz as if lightDir was (0,0,1)
	Vec3 targetLigthDir(0, 0, 1);
	Vec3 lightDir = Normalize(light.GetDirection());
	auto cp = Cross(lightDir, targetLigthDir);
	auto dp = 1.0f + Dot(lightDir, targetLigthDir);
	Quat rot(cp.x, cp.y, cp.z, dp);

	for (auto& v : frustumPoints) {
		v *= rot;
	}
	// this rotation is the view matrix
	Matrix44 viewMat = rot;

	// fuck that, an axis aligned bounding rect's gonna be good
#pragma message("QUALITY WARN: replace this with smallest area arbitrary bounding rectangle")
	Vec3 limitMin = frustumPoints[0],
		limitMax = frustumPoints[0];
	for (auto& v : frustumPoints) {
		limitMin.x = std::min(v.x, limitMin.x);
		limitMin.y = std::min(v.y, limitMin.y);
		limitMin.z = std::min(v.z, limitMin.z);

		limitMax.x = std::max(v.x, limitMax.x);
		limitMax.y = std::max(v.y, limitMax.y);
		limitMax.z = std::max(v.z, limitMax.z);
	}
	// check if limits have real volume or they are just 2D
		// in the latter case, no projection exists
	Vec3 volumeDim = limitMax - limitMin;
	static const float epsilon = 1e-25f;
	if (!(epsilon < volumeDim.x && epsilon < volumeDim.y && epsilon < volumeDim.z)) {
		return false;
	}

	// now make some projection from this
	Matrix44 projMat = Matrix44ProjOrtographic(limitMin.z, limitMax.z, limitMin.x, limitMax.x, limitMin.y, limitMax.y);

	// set output matrices
	viewOut = viewMat;
	projOut = projMat;

	// so far we've succeeded
	return true;
}
