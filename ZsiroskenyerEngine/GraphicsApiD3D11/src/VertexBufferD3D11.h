////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/VertexBufferD3D11.h
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 vertex buffer implementation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/IVertexBuffer.h"
#include "../../Core/src/ResourceProperty.h"
#include "../../Core/src/VertexFormat.h"

struct ID3D11Buffer;

class cVertexBufferD3D11 : public IVertexBuffer {
public:
	cVertexBufferD3D11(ID3D11Buffer* buffer, eUsage usage, cVertexFormat f, size_t size);
	~cVertexBufferD3D11();
	void Release() override;

	void SetFormat(cVertexFormat f) override;

	cVertexFormat GetFormat() const override;
	size_t		  GetStride() const override;
	eUsage		  GetUsage() const override;
	size_t		  GetSize() const override;

	ID3D11Buffer* GetBufferPointer() const;

public:
	ID3D11Buffer* const buffer;

protected:
	cVertexFormat format;
	size_t size;
	size_t stride;
	eUsage usage;
};