// IGraphicsApi.h By Zsíroskenyér Team 2013.10.10 1:37
// interface for different 3D graphics libraries like DirectX11 or OpenGL ..
#pragma once

#include "../../Core/src/IWindow.h"
#include "../../Core/src/common.h"
#include "../../Core/src/zsString.h"
#include "../../Core/src/math/Vec4.h"

#include "ResourceProperty.h"
#include "PipelineState.h"
#include "GapiResult.h"
#include "ITexture2D.h"

#include <climits>
#include <cstdint>


class IVertexBuffer;
class IIndexBuffer;
class IConstantBuffer;
class IShaderProgram;

enum class ePrimitiveTopology {
	LINE_LIST,
	TRIANGLE_LIST,
};

class IGraphicsApi {
public:
	virtual void Release() = 0;

	// --- resources --- //
	virtual eGapiResult	CreateVertexBuffer(IVertexBuffer** resource, size_t size, eUsage usage, void* data = NULL) = 0;
	virtual eGapiResult	CreateIndexBuffer(IIndexBuffer** resource, size_t size, eUsage usage, void* data = NULL) = 0;

	// shader constants are multiple of 16 byte
	virtual eGapiResult CreateTexture(ITexture2D** resource, const zsString& filePath) = 0;
	virtual eGapiResult CreateTexture(ITexture2D** resource, ITexture2D::tDesc desc, void* data = NULL) = 0;
	virtual eGapiResult CreateShaderProgram(IShaderProgram** resource, const zsString& shaderPath) = 0;


	virtual eGapiResult WriteResource(IIndexBuffer* buffer , void* source, size_t size = ZS_MAX(size_t), size_t offset = 0) = 0;
	virtual eGapiResult WriteResource(IVertexBuffer* buffer, void* source, size_t size = ZS_MAX(size_t), size_t offset = 0) = 0;

	virtual eGapiResult ReadResource(IIndexBuffer* buffer, void* dest, size_t size, size_t offset = 0) = 0;
	virtual eGapiResult ReadResource(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) = 0;
	virtual eGapiResult ReadResource(ITexture2D* texture, void* dest, size_t size, size_t offset = 0) = 0;

	virtual eGapiResult CopyResource(ITexture2D* src, ITexture2D* dst) = 0;

	// --- rendering --- //
	virtual void Clear(bool target = true, bool depth = false, bool stencil = false) = 0;
	virtual void ClearTexture(ITexture2D* t, unsigned clearFlag = 0, const Vec4& clearColor = Vec4(), float depthVal = 1.0f, size_t stencilVal = 0) = 0;

	virtual void Draw(size_t nVertices, size_t idxStartVertex = 0) = 0;
	virtual void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) = 0;
	virtual void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) = 0;
	virtual void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) = 0;

	virtual void Present() = 0;

	// --- state --- //
	virtual eGapiResult SetRenderTargetDefault() = 0;
	virtual eGapiResult SetRenderTargets(unsigned nTargets, const ITexture2D* const* renderTargets, ITexture2D* depthStencilTarget = NULL) = 0;
	virtual eGapiResult SetBackBufferSize(unsigned width, unsigned height) = 0;

	virtual void SetVertexBuffer(const IVertexBuffer* vertexBuffer, size_t vertexStride) = 0;
	virtual void SetIndexBuffer(const IIndexBuffer* indexBuffer) = 0;
	virtual void SetInstanceData(/*whatever*/) = 0;
	virtual void SetVSConstantBuffer(const void* data, size_t size, size_t slotIdx) = 0;
	virtual void SetPSConstantBuffer(const void* data, size_t size, size_t slotIdx) = 0;
	virtual void SetTexture(const ITexture2D* t, size_t slotIdx) = 0;
	virtual void SetTexture(const zsString& varName, const ITexture2D* t) = 0;
	virtual void SetShaderProgram(IShaderProgram* shProg) = 0;
	virtual void SetPrimitiveTopology(ePrimitiveTopology t) = 0;

	virtual eGapiResult SetBlendState(tBlendDesc desc) = 0;
	virtual eGapiResult SetDepthStencilState(tDepthStencilDesc desc, uint8_t stencilRef) = 0;


	// --- misc --- //
	virtual eGapiResult SetWindow(IWindow *renderWindow) = 0;

	virtual ITexture2D* GetDefaultRenderTarget() const = 0;
};