#include "GraphicsApiGL.h"

void cGraphicsApiGL::Release() {

}

IVertexBuffer* cGraphicsApiGL::CreateBufferVertex(size_t nVertices, size_t vertexStride, eBufferUsage usage, void* data /*= NULL*/) {
	return NULL;
}

IIndexBuffer* cGraphicsApiGL::CreateBufferIndex(size_t size , eBufferUsage usage, void* data /*= NULL*/) {
	return NULL;
}

IConstantBuffer* cGraphicsApiGL::CreateBufferConstant(size_t size, eBufferUsage usage, void* data /*= NULL*/) {
	return NULL;
}

ITexture2D*	cGraphicsApiGL::CreateTexture(const zsString& filePath) {
	return NULL;
}

bool cGraphicsApiGL::WriteBuffer(IIndexBuffer* buffer , void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsApiGL::WriteBuffer(IVertexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsApiGL::ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsApiGL::ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	return false;
}

void cGraphicsApiGL::Clear(bool target /*= true*/, bool depth /*= false*/, bool stencil /*= false*/) {

}

void cGraphicsApiGL::Present() {
	
}

void cGraphicsApiGL::Draw(size_t nVertices, size_t idxStartVertex /*= 0*/) {
	
}

void cGraphicsApiGL::DrawIndexed(size_t nIndices, size_t idxStartIndex /*= 0*/) {
	
}

void cGraphicsApiGL::DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	
}

void cGraphicsApiGL::DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	
}

void cGraphicsApiGL::SetVertexData(const IVertexBuffer* vertexBuffer, size_t vertexStride) {
	
}

void cGraphicsApiGL::SetIndexData(const IIndexBuffer* indexBuffer) {
	
}

void cGraphicsApiGL::SetInstanceData() {

}

void cGraphicsApiGL::SetShaderProgram(IShaderProgram* shProg) {

}

IShaderProgram* cGraphicsApiGL::CreateShaderProgram(const zsString& shaderPath) {
	return NULL;
}

void cGraphicsApiGL::SetWindow(IWindow *renderWindow) {

}