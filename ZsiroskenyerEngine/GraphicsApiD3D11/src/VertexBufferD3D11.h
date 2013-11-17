////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/VertexBufferD3D11.h
//	2012.oct.12, zsiroskenyer team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 vertex buffer implementation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/IVertexBuffer.h"
#include "../../Core/src/ResourceProperty.h"

struct ID3D11Buffer;

class cVertexBufferD3D11 : public IVertexBuffer {
public:
	size_t GetSize() const override;
	eUsage GetUsage() const override;
	ID3D11Buffer* GetBufferPointer() const;

	cVertexBufferD3D11(ID3D11Buffer* buffer, size_t size, eUsage usage);
	~cVertexBufferD3D11();
	void Release() override;

public:
	ID3D11Buffer* const buffer;
private:
	size_t size;
	eUsage usage;
};