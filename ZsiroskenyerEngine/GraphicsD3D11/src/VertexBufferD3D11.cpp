////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/VertexBufferD3D11.cpp
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 index buffer implementation
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IndexBufferD3D11.cpp
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 vertex buffer implementation
////////////////////////////////////////////////////////////////////////////////

#include "VertexBufferD3D11.h"
#include "Dx11_SDK/Include/D3D11.h"
#include "Dx11_SDK/Include/D3DX11.h"

cVertexBufferD3D11::cVertexBufferD3D11(ID3D11Buffer* buffer, size_t size, eBufferUsage usage)
:buffer(buffer), usage(usage), size(size) {		
}

cVertexBufferD3D11::~cVertexBufferD3D11() {
	buffer->Release();
}

size_t cVertexBufferD3D11::GetSize() const {
	return size;
}

eBufferUsage cVertexBufferD3D11::GetUsage() const {
	return usage;
}

ID3D11Buffer* cVertexBufferD3D11::GetBufferPointer() const {
	return buffer;
}