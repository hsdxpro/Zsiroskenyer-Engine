////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Small shadow map texture and misc management helper class
//
////////////////////////////////////////////////////////////////////////////////

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
	cShadowMap(IGraphicsApi* gApi, unsigned resolution, eFormat format, int cascades = 1);
	~cShadowMap();
	cShadowMap(const cShadowMap&) = delete;
	cShadowMap& operator=(const cShadowMap&) = delete;

	// initialize automatically clears previous "session"
	void Init(IGraphicsApi* gApi, unsigned resolution, eFormat format, int cascades = 1);
	// should we reinit it?
	bool IsCompatible(IGraphicsApi* gApi, unsigned resolution, eFormat format, int cascades = 1);
	void Clear();

	// getters
	IGraphicsApi* GetGraphicsApi() const;
	eFormat GetFormat() const;
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
	eFormat format;
	std::vector<ITexture2D*> maps;
};