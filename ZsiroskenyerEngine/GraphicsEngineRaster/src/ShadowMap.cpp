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
// Directional light

// Contructor
cShadowMapDir::cShadowMapDir() {

}
cShadowMapDir::cShadowMapDir(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	Init(gApi, resolution, readFormat, depthFormat, cascades);
}
cShadowMapDir::~cShadowMapDir() {
	Clear();
}

// Init
void cShadowMapDir::Init(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	assert(0 < cascades);

	Clear();

	// create textures and init stuff
	this->cascades = cascades;
	this->depthFormat = depthFormat;
	this->readFormat = readFormat;
	this->resolution = resolution;
	this->gApi = gApi;

	maps.resize(cascades);

	ITexture2D::tDesc desc;
	desc.arraySize = 1;
	desc.bind = (int)eBind::DEPTH_STENCIL | (int)eBind::SHADER_RESOURCE;
	desc.format = readFormat;
	desc.depthFormat = depthFormat;
	desc.width = desc.height = resolution;
	desc.mipLevels = 1;
	desc.usage = eUsage::DEFAULT;

	for (auto& v : maps) {
		auto r = gApi->CreateTexture(&v.texture, desc);
		if (r != eGapiResult::OK) {
			Clear();
			throw std::runtime_error("failed to create textures");
		}
	}
}
bool cShadowMapDir::IsValid(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades) {
	return (
		this->cascades == cascades &&
		this->depthFormat == depthFormat &&
		this->readFormat == readFormat &&
		this->resolution == resolution &&
		this->gApi == gApi &&
		operator bool()
	);
}
void cShadowMapDir::Clear() {
	for (auto& v : maps) {
		SAFE_RELEASE(v.texture);
	}
	maps.clear();
}

cShadowMapDir::operator bool() {
	return maps.size() != 0;
}

// Get
unsigned cShadowMapDir::GetResolution() const {
	return resolution;
}
eFormat cShadowMapDir::GetReadFormat() const {
	return readFormat;
}
eFormat cShadowMapDir::GetDepthFormat() const {
	return depthFormat;
}
auto cShadowMapDir::GetMaps() const -> const std::vector<tMap>& {
	return maps;
}



bool cShadowMapDir::Transform(
	Matrix44& projOut,
	Matrix44& viewOut,
	const Vec3& lightDir,
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
		v4 /= v4.w;
		v = v4;
	}
	// interpolate for near and far clips
	for (int i = 0; i < 4; i++) {
		auto tmp = frustumPoints[i];
		frustumPoints[i] = (1 - nearClip)*frustumPoints[i] + nearClip*frustumPoints[i + 4];
		frustumPoints[i + 1] = (1 - farClip)*frustumPoints[i] + farClip*frustumPoints[i + 4];
	}

	// ONLY DIRECTIONAL LIGHTS!
	// rotate them pointz as if lightDir was (0,0,1)
	Vec3 targetLigthDir(0, 0, 1);
	Vec3 ld = Normalize(lightDir);
	auto cp = Cross(ld, targetLigthDir);
	auto dp = 1.0f + Dot(ld, targetLigthDir);
	Quat rot(cp.x, cp.y, cp.z, dp);
	rot.Normalize();

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
