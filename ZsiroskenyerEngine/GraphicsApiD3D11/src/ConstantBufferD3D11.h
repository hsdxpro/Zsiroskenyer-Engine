// ConstantBufferD3D11.h By Zsíroskenyér Team 2013.10.16 0:03
#pragma once

#include "../../Core/src/IConstantBuffer.h"
#include "../../Core/src/ResourceProperty.h"

struct ID3D11Buffer;

class cConstantBufferD3D11 : public IConstantBuffer {
public:
	size_t GetSize() const override;
	eUsage GetUsage() const override;
	ID3D11Buffer* GetBufferPointer() const;
	
	cConstantBufferD3D11(ID3D11Buffer* buffer, size_t size, eUsage usage);
	~cConstantBufferD3D11();
	void Release() override;
public:
	ID3D11Buffer* const buffer;
private:
	size_t size;
	eUsage usage;
};