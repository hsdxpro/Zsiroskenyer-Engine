#include "GraphicsGL.h"

void cGraphicsGL::BBClear(bool clearOnlyDepth) {

}

void cGraphicsGL::BBPresent() {

}

void cGraphicsGL::SetWindow(IWindow *renderWindow) {

}

// Creating Buffers
IVertexBuffer* cGraphicsGL::CreateVertexBuffer(size_t size, eBufferUsage usage, void* data /*= NULL*/) {
	return NULL;
}

IIndexBuffer* cGraphicsGL::CreateIndexBuffer(size_t size, eBufferUsage usage, void* data /*= NULL*/) {
	return NULL;
}

bool cGraphicsGL::Write(IIndexBuffer* buffer , void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsGL::Write(IVertexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsGL::Read(IIndexBuffer* buffer , void* dest, size_t size, size_t offset /*= 0*/) {
	return false;
}

bool cGraphicsGL::Read(IVertexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	return false;
}