// IGraphicsApi.h By Zsíroskenyér Team 2013.10.10 1:37
// interface for different 3D graphics libraries like DirectX11 or OpenGL ..
#pragma once

#include "IWindow.h"
#include "BufferCommon.h"
#include <climits>
#include "../../CommonLib/src/common.h"

#ifdef max
#define _377995BD_F40A_4067_A633_46AAA084AC7F
#undef max
#endif

class IVertexBuffer;
class IIndexBuffer;

class IGraphicsApi {
public:
	// buffers
	virtual IVertexBuffer* CreateVertexBuffer(size_t size, eBufferUsage usage, void* data = NULL) = 0;
	virtual IIndexBuffer* CreateIndexBuffer(size_t size, eBufferUsage usage, void* data = NULL) = 0;
	virtual bool Write(IIndexBuffer* buffer , void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) = 0;
	virtual bool Write(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset=0) = 0;

	virtual bool Read(IIndexBuffer* buffer , void* dest, size_t size, size_t offset = 0) = 0;
	virtual bool Read(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) = 0;

	// pipeline
	virtual void BBClear(bool clearOnlyDepth = false) = 0;
	virtual void BBPresent() = 0;

	// window target
	virtual void SetWindow(IWindow *renderWindow) = 0;
};



#ifdef _377995BD_F40A_4067_A633_46AAA084AC7F
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#undef _377995BD_F40A_4067_A633_46AAA084AC7F
#endif