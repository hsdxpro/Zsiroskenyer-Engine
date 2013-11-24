////////////////////////////////////////////////////////////////////////////////
//	CreateGAPI.h
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Creates GraphicsApi's
//
////////////////////////////////////////////////////////////////////////////////

#include <cassert>

#include "CreateGAPI.h"
#include "../../Core/src/DynamicLibrary.h"

static DllHandle hDllOpenGL43	= 0;
static DllHandle hDllD3D11		= 0;

IGraphicsApi*(*CreateGraphicsApiD3D11)(IWindow*, unsigned, unsigned) = NULL;
IGraphicsApi*(*CreateGraphicsApiGL43)(IWindow*, unsigned, unsigned)  = NULL;


IGraphicsApi* CreateGraphicsApi(IWindow* targetWindow, unsigned backBufferWidth, unsigned backBufferHeight, eGraphicsApiType type) {
	// try to load the dynamic libraries and return an api
	switch (type) {
		case eGraphicsApiType::GRAPHICS_API_D3D11:
			if (hDllD3D11) {
				if (CreateGraphicsApiD3D11)
					return CreateGraphicsApiD3D11(targetWindow, backBufferWidth, backBufferHeight);
				else
					return 0;
			}
			else {
				hDllD3D11 = LoadDynamicLibrary("GraphicsApiD3D11");
				if (!hDllD3D11) {
					return NULL;
				}
				CreateGraphicsApiD3D11 = (IGraphicsApi*(*)(IWindow*, unsigned, unsigned))GetFunctionAddress(hDllD3D11, "CreateGraphicsApiD3D11");
				if (!CreateGraphicsApiD3D11) {
					assert(false);
					return NULL;
				}
				return CreateGraphicsApiD3D11(targetWindow, backBufferWidth, backBufferHeight);
			}
			break;
		case eGraphicsApiType::GRAPHICS_API_OPENGL43:
			if (hDllOpenGL43) {
				if (CreateGraphicsApiGL43)
					return CreateGraphicsApiGL43(targetWindow, backBufferWidth, backBufferHeight);
				else
					return NULL;
			}
			else {
				hDllOpenGL43 = LoadDynamicLibrary("GraphicsApiGL");
				if (!hDllOpenGL43) {
					return NULL;
				}
				CreateGraphicsApiGL43 = (IGraphicsApi*(*)(IWindow*, unsigned, unsigned))GetFunctionAddress(hDllOpenGL43, "CreateGraphicsApiDL");
				if (!CreateGraphicsApiGL43) {
					assert(false);
					return NULL;
				}
				return CreateGraphicsApiGL43(targetWindow, backBufferWidth, backBufferHeight);
			}
			break;
		default:
			return NULL;
	}
}


std::vector<eGraphicsApiType> EnumGraphicsApiTypes() {
	// TODO: implement this
	assert(false);
	return std::vector<eGraphicsApiType>();
}
