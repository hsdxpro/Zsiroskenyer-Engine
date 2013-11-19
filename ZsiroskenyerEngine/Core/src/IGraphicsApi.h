// IGraphicsApi.h By Zsíroskenyér Team 2013.10.10 1:37
// interface for different 3D graphics libraries like DirectX11 or OpenGL ..
#pragma once

#include "../../Core/src/IWindow.h"
#include "../../Core/src/common.h"
#include "../../Core/src/zsString.h"

#include "ResourceProperty.h"
#include "GapiResult.h"

#include <climits>

class IVertexBuffer;
class IIndexBuffer;
class IConstantBuffer;
class IShaderProgram;
class ITexture2D;


enum class ePrimitiveTopology {
	LINE_LIST,
	TRIANGLE_LIST,
};

class IGraphicsApi {
public:
	virtual void Release() = 0;

	// resources
	virtual eGapiResult	CreateVertexBuffer(IVertexBuffer** resource, size_t size, eUsage usage, void* data = NULL) = 0;
	virtual eGapiResult	CreateIndexBuffer(IIndexBuffer** resource, size_t size, eUsage usage, void* data = NULL) = 0;
	virtual eGapiResult CreateConstantBuffer(IConstantBuffer** resource, size_t size, eUsage usage, void* data = NULL) = 0;
	virtual eGapiResult CreateTexture(ITexture2D** resource, const zsString& filePath) = 0;
	virtual eGapiResult CreateTexture(ITexture2D** resource, unsigned width, unsigned height, unsigned mipLevels, unsigned arraySize, eFormat format, unsigned bind, eFormat depthStencilFormat = eFormat::UNKNOWN) = 0;
	virtual eGapiResult CreateShaderProgram(IShaderProgram** resource, const zsString& shaderPath) = 0;

	virtual eGapiResult WriteBuffer(IIndexBuffer* buffer , void* source, size_t size = ZS_MAX(size_t), size_t offset = 0) = 0;
	virtual eGapiResult WriteBuffer(IVertexBuffer* buffer, void* source, size_t size = ZS_MAX(size_t), size_t offset = 0) = 0;

	virtual eGapiResult ReadBuffer(IIndexBuffer* buffer, void* dest, size_t size, size_t offset = 0) = 0;
	virtual eGapiResult ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) = 0;

	// rendering
	virtual void Clear(bool target = true, bool depth = false, bool stencil = false) = 0;
	virtual void Present() = 0;

	virtual void Draw(size_t nVertices, size_t idxStartVertex = 0) = 0;
	virtual void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) = 0;
	virtual void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) = 0;
	virtual void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) = 0;

	virtual eGapiResult SetRenderTarget(ITexture2D* target, unsigned slotIdx) = 0;
	virtual void SetRenderTargetDefault() = 0;

	virtual void SetVertexBuffer(const IVertexBuffer* vertexBuffer, size_t vertexStride) = 0;
	virtual void SetIndexBuffer(const IIndexBuffer* indexBuffer) = 0;
	virtual void SetInstanceData(/*whatever*/) = 0;
	virtual void SetVSConstantBuffer(IConstantBuffer* buffer, size_t slotIdx) = 0;
	virtual void SetPSConstantBuffer(IConstantBuffer* buffer, size_t slotIdx) = 0;
	virtual void SetTexture(const ITexture2D* tex, size_t slotIdx) = 0;
	virtual void SetShaderProgram(IShaderProgram* shProg) = 0;
	virtual void SetPrimitiveTopology(ePrimitiveTopology t) = 0;

	// misc
	virtual void SetWindow(IWindow *renderWindow) = 0;

	virtual ITexture2D* GetDefaultRenderTarget() const = 0;
};