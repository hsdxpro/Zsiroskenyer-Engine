// GraphicsGL.h By Zsíroskenyér team
#pragma once

#include "../../GraphicsCommon\src\IGraphicsApi.h"
#include "../../GraphicsCommon\src\IWindow.h"
#include "..\..\CommonLib\src\common.h"

class cGraphicsGL : public IGraphicsApi {
public:
	IVertexBuffer* CreateVertexBuffer(size_t size, eBufferUsage usage, void* data = NULL) override;
	IIndexBuffer* CreateIndexBuffer(size_t size, eBufferUsage usage, void* data = NULL) override;

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

	void SetVertexData(IVertexBuffer* vertexBuffer) override;
	void SetIndexData(IIndexBuffer* indexBuffer) override;
	void SetInstanceData(/*whatever*/) override;

	void SetWindow(IWindow *renderWindow) override;
protected:
};


// DLL accessor
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsGL() {
#pragma message("+++++ +++++ Implement opengl class, abstract methots not implemented yet! +++++ +++++")
		//return new cGraphicsGL();
		return NULL;
}