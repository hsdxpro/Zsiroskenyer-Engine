// GraphireatedefaultsGL.h By Zsíroskenyér team
#pragma once

#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/common.h"

class cGraphicsApiGL : public IGraphicsApi {
public:
	/* BAZZEG ÚGYIS NAPONTA VÁLTOZIK, SZARNI BELE, MAJD MEGÍRJUK HA FIX LETT
	cGraphicsApiGL();
	~cGraphicsApiGL();
	void Release() override;

	// --- resources --- //
	eGapiResult	CreateVertexBuffer(IVertexBuffer** resource, size_t size, eUsage usage, void* data = nullptr) override;
	eGapiResult	CreateIndexBuffer(IIndexBuffer** resource, size_t size, eUsage usage, void* data = nullptr) override;
	eGapiResult CreateTexture(ITexture2D** resource, const zsString& filePath) override;
	eGapiResult CreateTexture(ITexture2D** resource, ITexture2D::tDesc desc, void* data = nullptr) override;
	eGapiResult CreateShaderProgram(IShaderProgram** resource, const zsString& shaderPath) override;

	eGapiResult WriteResource(IIndexBuffer* buffer, void* source, size_t size = ZS_NUMERIC_MAX(size_t), size_t offset = 0) override;
	eGapiResult WriteResource(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMERIC_MAX(size_t), size_t offset = 0) override;

	eGapiResult ReadResource(IIndexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;
	eGapiResult ReadResource(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;
	eGapiResult ReadResource(ITexture2D* texture, void* dest, size_t size, size_t offset = 0) override;

	eGapiResult CopyResource(ITexture2D* src, ITexture2D* dst) override;

	// --- render --- //
	void Clear(bool target = true, bool depth = false, bool stencil = false, const Vec4& clearColor = Vec4(0.3f, 0.3f, 0.3f, 1.0f)) override;
	void ClearTexture(ITexture2D* t, unsigned clearFlag = 0, const Vec4& clearColor = Vec4(), float depthVal = 1.0f, size_t stencilVal = 0);

	void Draw(size_t nVertices, size_t idxStartVertex = 0) override;
	void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) override;
	void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) override;
	void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) override;

	void Present() override;

	// --- render state --- //
	eGapiResult SetRenderTargetDefault() override;
	eGapiResult SetRenderTargets(unsigned nTargets, const ITexture2D* const* renderTargets, ITexture2D* depthStencilTarget = nullptr) override;
	eGapiResult SetBackBufferSize(unsigned width, unsigned height) override;

	void SetVertexBuffer(const IVertexBuffer* vertexBuffer, size_t vertexStride) override;
	void SetIndexBuffer(const IIndexBuffer* indexBuffer) override;
	void SetInstanceData(whatever) override;
	eGapiResult SetTexture(const ITexture2D* t, size_t slotIdx) override;
	eGapiResult SetTexture(const zsString& varName, const ITexture2D* t) override;
	void SetVSConstantBuffer(const void* data, size_t size, size_t slotIdx) override;
	void SetPSConstantBuffer(const void* data, size_t size, size_t slotIdx) override;
	void SetShaderProgram(IShaderProgram* shProg) override;
	void SetPrimitiveTopology(ePrimitiveTopology t) override;
	eGapiResult SetWindow(IWindow *renderWindow) override;

	eGapiResult SetBlendState(tBlendDesc desc) override;
	eGapiResult SetDepthStencilState(tDepthStencilDesc desc, uint8_t stencilRef) override;

	// --- misc --- //
	ITexture2D* GetDefaultRenderTarget() const override;
	*/
protected:
};


// DLL accessor
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsApiGL() {
		//return new cGraphicsApiGL();
		return nullptr;
}