// ConstantBufferD3D11.h By Zsíroskenyér Team 2013.10.16 0:03
#pragma once

#include "../../GraphicsCommon/src/IConstantBuffer.h"
#include "Dx11_SDK/Include/D3D11.h"
#include "Dx11_SDK/Include/D3DX11.h"

class cConstantBufferD3D11 : public IConstantBuffer {
public:
	cConstantBufferD3D11::cConstantBufferD3D11(ID3D11Buffer* buffer, size_t size, eBufferUsage usage);
	~cConstantBufferD3D11();

	size_t GetSize() const override;
	eBufferUsage GetUsage() const override;
	void* GetBuffer() const;

	ID3D11Buffer* const buffer;
private:
	size_t size;
	eBufferUsage usage;
};