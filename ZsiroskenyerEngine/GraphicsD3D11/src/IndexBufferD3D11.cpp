////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IndexBufferD3D11.cpp
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 index buffer implementation
////////////////////////////////////////////////////////////////////////////////

#include "IndexBufferD3D11.h"

cIndexBufferD3D11::cIndexBufferD3D11(ID3D11Buffer* buffer, size_t size, eBufferUsage usage)
:buffer(buffer), usage(usage), size(size) {		
}

cIndexBufferD3D11::~cIndexBufferD3D11() {
	buffer->Release();
}

size_t cIndexBufferD3D11::GetSize() const {
	return size;
}

eBufferUsage cIndexBufferD3D11::GetUsage() const {
	return usage;
}

cIndexBufferD3D11::operator ID3D11Buffer*() const {
	return buffer;
}