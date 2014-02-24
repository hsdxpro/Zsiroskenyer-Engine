// IVertexBuffer.h By Zsíroskenyér Team 2013.10.11 2:19 last modified: Richárd Németh
// Responsibility: Holding Mesh points of the 3D object
#pragma once

#include "ResourceProperty.h"
#include "VertexFormat.h"

class IVertexBuffer {
public:
	virtual void Release() = 0;

	virtual void SetFormat(cVertexFormat format) = 0;

	virtual cVertexFormat GetFormat() const = 0;
	virtual eUsage		  GetUsage()  const = 0;
	virtual size_t		  GetStride() const = 0;
	virtual size_t		  GetSize()   const = 0;
	
	
};