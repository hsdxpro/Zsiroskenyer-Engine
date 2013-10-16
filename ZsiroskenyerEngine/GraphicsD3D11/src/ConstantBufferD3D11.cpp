#include "ConstantBufferD3D11.h"

cConstantBufferD3D11::cConstantBufferD3D11(ID3D11Buffer* buffer, size_t size, eBufferUsage usage) 
:buffer(buffer), usage(usage), size(size) {	
}

cConstantBufferD3D11::~cConstantBufferD3D11() {
	buffer->Release();
}

size_t cConstantBufferD3D11::GetSize() const {
	return size;
}

eBufferUsage cConstantBufferD3D11::GetUsage() const {
	return usage;
}

cConstantBufferD3D11::operator ID3D11Buffer*() const {
	return buffer;
}