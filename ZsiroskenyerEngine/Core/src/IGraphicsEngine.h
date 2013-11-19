////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IGraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "zsString.h"
#include <cstdint>

class IGraphicsApi;
class IShaderManager;
class cGraphicsEntity;
class cCamera;
class IWindow;

////////////////////////////////////////////////////////////////////////////////
//	Configuration
union tGraphicsConfig {
	enum eGraphicsApi : uint32_t {
		OPENGL_43 = 1,
		D3D11 = 2,
	};
	struct RasterEngine {
		eGraphicsApi gxApi;
	};
};


////////////////////////////////////////////////////////////////////////////////
//	Errors
enum class eGraphicsResult : signed int {
	OK = 0,
	ERROR_UNKNOWN,
	ERROR_OUT_OF_MEMORY,
	ERROR_FILE_NOT_FOUND,
	ERROR_INVALID_CALL,
	ERROR_INVALID_ARG
};

////////////////////////////////////////////////////////////////////////////////
//	Graphics Engine interface
class IGraphicsEngine {
public:
	virtual void Release() = 0;
	
	virtual eGraphicsResult Resize(unsigned width, unsigned height) = 0;
	virtual eGraphicsResult Update() = 0;
	virtual eGraphicsResult ReloadResources() = 0;
	virtual eGraphicsResult SetConfig(tGraphicsConfig config) = 0;
	virtual void SetActiveCamera(cCamera* cam) = 0;
	virtual cCamera* GetActiveCamera() = 0;

	virtual cGraphicsEntity* CreateEntity(const zsString& geomPath, const zsString& mtlPath) = 0;
	virtual IGraphicsApi*		GetGraphicsApi() = 0;
	virtual IShaderManager*		GetShaderManager() = 0;
};
