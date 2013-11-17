// IIndexBuffer.h By Zs�roskeny�r Team 2013.10.11 2:19 last modified: Rich�rd N�meth
// Responsibility: Holding Indices for Mesh points of the 3D object
#pragma once

#include "ResourceProperty.h"

class IIndexBuffer {
public:
	virtual size_t GetSize() const = 0;
	virtual eUsage GetUsage() const = 0;
	virtual void Release() = 0;
};