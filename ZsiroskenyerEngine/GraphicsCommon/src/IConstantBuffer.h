// IConstantBuffer.h By Zsíroskenyér Team 2013.10.16 0:02
// hold values in buffers that will be passed to the shader
#pragma once

#include "../../GraphicsCommon/src/BufferCommon.h"

class IConstantBuffer {
public:
	virtual size_t GetSize() const = 0;
	virtual eBufferUsage GetUsage() const = 0;
	virtual void* GetBuffer() const = 0;
};


