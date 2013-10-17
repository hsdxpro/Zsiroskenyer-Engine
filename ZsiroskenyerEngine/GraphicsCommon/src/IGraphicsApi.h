// IGraphicsApi.h By Zsíroskenyér Team 2013.10.10 1:37
// interface for different 3D graphics libraries like DirectX11 or OpenGL ..
#pragma once

#include "IWindow.h"
#include "BufferCommon.h"
#include <climits>
#include "../../CommonLib/src/common.h"
#include "../../CommonLib/src/zsString.h"

#ifdef max
#define _377995BD_F40A_4067_A633_46AAA084AC7F
#undef max
#endif

class IVertexBuffer;
class IIndexBuffer;
class IConstantBuffer;
class IShaderProgram;

class IGraphicsApi {
public:
	//buffers
	virtual IVertexBuffer* CreateVertexBuffer(size_t nVertices, size_t vertexStride, eBufferUsage usage, void* data = NULL) = 0;
	virtual IIndexBuffer* CreateIndexBuffer(size_t size, eBufferUsage usage, void* data = NULL) = 0;
	virtual IConstantBuffer* CreateConstantBuffer(size_t size, eBufferUsage usage, void* data = NULL) = 0;

	virtual bool WriteBuffer(IIndexBuffer* buffer , void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) = 0;
	virtual bool WriteBuffer(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) = 0;

	virtual bool ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset = 0) = 0;
	virtual bool ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) = 0;

	virtual void LoadConstantBuffer(IConstantBuffer* buffer, size_t slotIdx) = 0;

	// draw
	virtual void Clear(bool target = true, bool depth = false, bool stencil = false) = 0;
	virtual void Present() = 0;

	virtual void Draw(size_t nVertices, size_t idxStartVertex = 0) = 0;
	virtual void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) = 0;
	virtual void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) = 0;
	virtual void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) = 0;

	virtual void SetRenderTargetDefault() = 0;


	virtual void SetVertexData(const IVertexBuffer* vertexBuffer, size_t vertexStride) = 0;
	virtual void SetIndexData(const IIndexBuffer* indexBuffer) = 0;
	virtual void SetInstanceData(/*whatever*/) = 0;

	virtual void SetShaderProgram(IShaderProgram* shProg) = 0;

	// shader
	virtual IShaderProgram* CreateShaderProgram(const zsString& shaderPath) = 0;

	// misc
	virtual void SetWindow(IWindow *renderWindow) = 0;
};



#ifdef _377995BD_F40A_4067_A633_46AAA084AC7F
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#undef _377995BD_F40A_4067_A633_46AAA084AC7F
#endif