////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IndexBufferD3D11.h
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 index buffer implementation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../GraphicsCommon/src/IIndexBuffer.h"
#include "../../GraphicsCommon/src/BufferCommon.h"
#include "Dx11_SDK/Include/D3D11.h"
#include "Dx11_SDK/Include/D3DX11.h"

class cIndexBufferD3D11 : public IIndexBuffer {
public:
	size_t GetSize() const override;
	eBufferUsage GetUsage() const override;

	operator ID3D11Buffer*() const;

	cIndexBufferD3D11(ID3D11Buffer* buffer, size_t size, eBufferUsage usage);
	~cIndexBufferD3D11();

public:
	ID3D11Buffer* const buffer;
private:
	size_t size;
	eBufferUsage usage;
};