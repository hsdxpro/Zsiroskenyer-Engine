// IVertexBuffer.h By Zs�roskeny�r Team 2013.10.11 2:19 last modified: Rich�rd N�meth
// Responsibility: Holding Mesh points of the 3D object
#pragma once

#include "BufferCommon.h"

class IVertexBuffer {
public:
	virtual size_t GetSize() const = 0;
	virtual eBufferUsage GetUsage() const = 0;
	virtual void Release() = 0;
};