#include "ConstantBufferD3D11.h"

cConstantBufferD3D11::cConstantBufferD3D11(ID3D11Buffer* buffer) 
:buffer(buffer) {	
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

size_t cConstantBufferD3D11::GetSize() const {
	return size;
}

eBufferUsage cConstantBufferD3D11::GetUsage() const {
	return usage;
}

cConstantBufferD3D11::operator ID3D11Buffer*() const {
	return buffer;
}