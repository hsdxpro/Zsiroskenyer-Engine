////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IndexBufferD3D11.cpp
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 index buffer implementation
////////////////////////////////////////////////////////////////////////////////

#include "IndexBufferD3D11.h"

cIndexBufferD3D11::cIndexBufferD3D11(ID3D11Buffer* buffer) : buffer(buffer) {	
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	buffer->GetDesc(&desc);

	size = desc.ByteWidth;

	switch (desc.Usage) {
	case D3D11_USAGE_IMMUTABLE:
		usage = eBufferUsage::IMMUTABLE;
		break;
	case D3D11_USAGE_DYNAMIC:
		usage = eBufferUsage::DYNAMIC;
		break;
	case D3D11_USAGE_STAGING:
		usage = eBufferUsage::STAGING;
		break;
	default:
		MessageBoxA(NULL,__FILE__":""28""\nSomething's fucked up...\nApplication will now stop.", "FATAL ERROR", MB_OK);
		exit(1);
	}
}

size_t cIndexBufferD3D11::GetSize() {
	return size;
}

eBufferUsage cIndexBufferD3D11::GetUsage() {
	return usage;
}
