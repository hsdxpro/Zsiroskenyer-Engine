// ConstantBufferD3D11.h By Zsíroskenyér Team 2013.10.16 0:03
#pragma once

#include "../../GraphicsApi/src/IConstantBuffer.h"
#include "Dx11_SDK/Include/D3D11.h"
#include "Dx11_SDK/Include/D3DX11.h"

class cConstantBufferD3D11 : public IConstantBuffer {
public:
	size_t GetSize() const override;
	eBufferUsage GetUsage() const override;
	ID3D11Buffer* GetBufferPointer() const;
	
	cConstantBufferD3D11::cConstantBufferD3D11(ID3D11Buffer* buffer, size_t size, eBufferUsage usage);
	void Release() override;

public:
	ID3D11Buffer* const buffer;
private:
	size_t size;
	eBufferUsage usage;
};