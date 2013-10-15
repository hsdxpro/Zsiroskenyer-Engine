#include "GraphicsGL.h"

IVertexBuffer* cGraphicsGL::CreateVertexBuffer(size_t size, eBufferUsage usage, void* data /*= NULL*/) {
	return NULL;
}

IIndexBuffer* cGraphicsGL::CreateIndexBuffer(size_t size  , eBufferUsage usage, void* data /*= NULL*/) {
	return NULL;
}

bool cGraphicsGL::WriteBuffer(IIndexBuffer* buffer , void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsGL::WriteBuffer(IVertexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsGL::ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsGL::ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	return false;
}

void cGraphicsGL::Clear(bool target /*= true*/, bool depth /*= false*/, bool stencil /*= false*/) {

}

void cGraphicsGL::Present() {
	
}

void cGraphicsGL::Draw(size_t nVertices, size_t idxStartVertex /*= 0*/) {
	
}

void cGraphicsGL::DrawIndexed(size_t nIndices, size_t idxStartIndex /*= 0*/) {
	
}

void cGraphicsGL::DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	
}

void cGraphicsGL::DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	
}

void cGraphicsGL::SetVertexData(const IVertexBuffer* vertexBuffer) {
	
}

void cGraphicsGL::SetIndexData(const IIndexBuffer* indexBuffer) {
	
}

void cGraphicsGL::SetInstanceData() {

}

void cGraphicsGL::SetShaderProgram(IShaderProgram* shProg) {

}

IShaderProgram* cGraphicsGL::CreateShaderProgram(const zsString& shaderPath) {
	return NULL;
}

void cGraphicsGL::SetWindow(IWindow *renderWindow) {

}