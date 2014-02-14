////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Small shadow map texture and misc management helper class
//
////////////////////////////////////////////////////////////////////////////////

// DOC
////////////////////////////////////////
// static ViewProjMatrix
//	@return: if it could create the matrices
//	@param: light: takes positional & type information from this object
//	@param: nearClip, farClip [0.0, 1.0]: clips the projection's frustum linearly
//			Use these params for cascaded shadow maps.
//			Example: nearClip=0.0f, farClip=0.5f -> close to camera part of frustum

#pragma once
 
#include <vector>
#include "GraphicsLight.h"
#include "../../core/src/math/matrix44.h"
#include "../../core/src/math/vec3.h"
#include "../../Core/src/ResourceProperty.h"

class ITexture2D;
class cGraphicsLight;
class IGraphicsApi;

////////////////////////////////////////////////////////////////////////////////
//	Shadow map helper class
class cShadowMap {
public:
	// ctor stuff
	cShadowMap();
	cShadowMap(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades = 1);
	~cShadowMap();
	cShadowMap(const cShadowMap&) = delete;
	cShadowMap& operator=(const cShadowMap&) = delete;

	// initialize automatically clears previous "session"
	void Init(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades = 1);
	// should we reinit it?
	bool IsCompatible(IGraphicsApi* gApi, unsigned resolution, eFormat readFormat, eFormat depthFormat, int cascades = 1);
	void Clear();

	// getters
	IGraphicsApi* GetGraphicsApi() const;
	eFormat GetReadFormat() const;
	eFormat GetDepthFormat() const;
	const std::vector<ITexture2D*>& GetMaps() const;
	int GetNumCascades() const;

	// light space view and projection matrices
	static bool ViewProjMatrix(
		Matrix44& projOut,
		Matrix44& viewOut,
		const cGraphicsLight& light,
		const Matrix44& cameraView,
		const Matrix44& cameraProj,
		float nearClip = 0.0f,
		float farClip = 1.0f);

	// check if there's a shadow map
	operator bool();
private:
	unsigned resolution;
	IGraphicsApi* gApi;
	eFormat readFormat, depthFormat;
	std::vector<ITexture2D*> maps;
};