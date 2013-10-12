////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IndexBufferD3D11.h
//	2012.oct.12, zsiroskenyer team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Direct3D 11 index buffer implementation
////////////////////////////////////////////////////////////////////////////////

#define NOMINMAX
#include "../../GraphicsEngine/src/IIndexBuffer.h"
#include "../../GraphicsEngine/src/BufferCommon.h"
#include "GraphicsD3D11.h"
#include <climits>

class cIndexBufferD3D11 : public IIndexBuffer {
public:
	cIndexBufferD3D11(ID3D11Buffer* buffer);

	size_t GetSize();
	eBufferUsage GetUsage();

	bool Read(void* dest, size_t nBytes=std::numeric_limits<size_t>::max(), size_t offset=0);
	bool Write(void* source, size_t nBytes, size_t offset=0);

	ID3D11Buffer* const buffer;
private:
	size_t size;
	eBufferUsage usage;
};