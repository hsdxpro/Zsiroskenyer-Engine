////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/VertexBufferD3D11.cpp
//	2012.oct.13, zsiroskenyer team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 index buffer implementation
////////////////////////////////////////////////////////////////////////////////


#include "VertexBufferD3D11.h"
#include <d3d11.h>

cVertexBufferD3D11::cVertexBufferD3D11(ID3D11Buffer* buffer, size_t size, eUsage usage)
:buffer(buffer), usage(usage), size(size) {		
}

cVertexBufferD3D11::~cVertexBufferD3D11() {
	buffer->Release();
}

void cVertexBufferD3D11::Release() {
	delete this;
}

void cVertexBufferD3D11::SetFormat(const cVertexFormat& f) {
	format = f;

	stride = f.Size();
}

const cVertexFormat& cVertexBufferD3D11::GetFormat() const {
	return format;
}

size_t cVertexBufferD3D11::GetStride() const
{
	return stride;
}

size_t cVertexBufferD3D11::GetSize() const {
	return size;
}

eUsage cVertexBufferD3D11::GetUsage() const {
	return usage;
}

ID3D11Buffer* cVertexBufferD3D11::GetBufferPointer() const {
	return buffer;
}
