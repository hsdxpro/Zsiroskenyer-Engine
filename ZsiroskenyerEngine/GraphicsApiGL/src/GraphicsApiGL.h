// GraphicsGL.h By Zsíroskenyér team
#pragma once

#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/common.h"

class cGraphicsApiGL : public IGraphicsApi {
public:
	void Release() override;
	IVertexBuffer*	CreateBufferVertex(size_t nVertices, size_t vertexStride, eBufferUsage usage, void* data = NULL) override;
	IIndexBuffer*	CreateBufferIndex(size_t size, eBufferUsage usage, void* data = NULL) override;
	IConstantBuffer*CreateBufferConstant(size_t size, eBufferUsage usage, void* data = NULL) override;

	ITexture2D*		CreateTexture(const zsString& filePath) override;
	IShaderProgram* CreateShaderProgram(const zsString& shaderPath) override;

	bool WriteBuffer(IIndexBuffer* buffer , void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;
	bool WriteBuffer(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;

	bool ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset = 0) override;
	bool ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;

	void Clear(bool target = true, bool depth = false, bool stencil = false) override;
	void Present() override;

	void Draw(size_t nVertices, size_t idxStartVertex = 0) override;
	void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) override;
	void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) override;
	void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) override;

	void SetVertexBuffer(const IVertexBuffer* vertexBuffer, size_t vertexStride) override;
	void SetIndexBuffer(const IIndexBuffer* indexBuffer) override;
	void SetInstanceData(/*whatever*/) override;
	void SetTexture(const ITexture2D* tex, size_t slotIdx) override;
	void SetShaderProgram(IShaderProgram* shProg) override;

	void SetWindow(IWindow *renderWindow) override;
protected:
};


// DLL accessor
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsApiGL() {
#pragma message("+++++ +++++ Implement opengl class, abstract methots not implemented yet! +++++ +++++")
		//return new cGraphicsApiGL();
		return NULL;
}