// IIndexBuffer.h By Zsíroskenyér Team 2013.10.11 2:19 last modified: Richárd Németh
// Responsibility: Holding Indices for Mesh points of the 3D object
#pragma once

#include "BufferCommon.h"

class IIndexBuffer {
public:
	virtual size_t GetSize() const = 0;
	virtual eBufferUsage GetUsage() const = 0;
	virtual void Release() = 0;
};