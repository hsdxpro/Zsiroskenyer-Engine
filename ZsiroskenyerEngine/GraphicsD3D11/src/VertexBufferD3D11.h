////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/VertexBufferD3D11.h
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 vertex buffer implementation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../GraphicsCommon/src/IVertexBuffer.h"
#include "../../GraphicsCommon/src/BufferCommon.h"
#include "Dx11_SDK/Include/D3D11.h"
#include "Dx11_SDK/Include/D3DX11.h"

class cVertexBufferD3D11 : public IVertexBuffer {
public:
	size_t GetSize() const override;
	eBufferUsage GetUsage() const override;
	ID3D11Buffer* GetBufferPointer() const;

	cVertexBufferD3D11(ID3D11Buffer* buffer, size_t size, eBufferUsage usage);
	void Release();

public:
	ID3D11Buffer* const buffer;
private:
	size_t size;
	eBufferUsage usage;
};