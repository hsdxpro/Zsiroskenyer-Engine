////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
//
////////////////////////////////////////////////////////////////////////////////
//	Contains everything related to graphics resource creation, like usage, 
//	pipeline binding, and other bullshit.
////////////////////////////////////////////////////////////////////////////////

#pragma once

enum class eUsage {
	DEFAULT = 1,
	IMMUTABLE,
	DYNAMIC,
	STAGING,
};

enum class eBind {
	VERTEX_BUFFER	= 0x1,
	INDEX_BUFFER	= 0x2,
	CONSTANT_BUFFER	= 0x04,
	SHADER_RESOURCE	= 0x8,
	RENDER_TARGET	= 0x20,
	DEPTH_STENCIL	= 0x40,
};

enum class eCPUAccess {
	WRITE = 1,
	READ,
};

enum class eFormat {

};