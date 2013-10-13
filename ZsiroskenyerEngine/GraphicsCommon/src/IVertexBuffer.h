// IVertexBuffer.h By Zsíroskenyér Team 2013.10.11 2:19 last modified: Richárd Németh
// Responsibility: Holding Mesh points of the 3D object
#pragma once

#include "BufferCommon.h"

class IVertexBuffer {
public:
	virtual size_t GetSize() = 0;
	virtual eBufferUsage GetUsage() = 0;
	virtual size_t GetStrides() = 0;
};