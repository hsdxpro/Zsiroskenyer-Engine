// ITexture2D.h By Zs�roskeny�r Team 2013.10.20 16:06
#pragma once

#include "ResourceProperty.h"


class ITexture2D {
public:
	virtual void Release() = 0;

	virtual size_t GetWidth() const = 0;
	virtual size_t GetHeight() const = 0;

	virtual ITexture2D* GetArraySlice(int idx) = 0;
	virtual const ITexture2D* GetArraySlice(int idx) const = 0;

	struct tDesc {
		tDesc(unsigned width, unsigned height, eFormat format, unsigned bind, eUsage usage, unsigned mipLevels = 1, unsigned arraySize = 1, eFormat depthFormat = eFormat::UNKNOWN)
		: width(width), height(height), format(format), bind(bind), usage(usage), mipLevels(mipLevels), arraySize(arraySize), depthFormat(depthFormat) { }
		tDesc() : tDesc(1, 1, eFormat::UNKNOWN, (int)eBind::SHADER_RESOURCE, eUsage::DEFAULT, 1, 1, eFormat::UNKNOWN) { }
		unsigned width, height;
		unsigned mipLevels;
		unsigned arraySize;
		eFormat format;
		unsigned bind;
		eUsage usage;
		eFormat depthFormat;
	};
};