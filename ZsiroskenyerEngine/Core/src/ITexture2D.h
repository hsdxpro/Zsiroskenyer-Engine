// ITexture2D.h By Zsíroskenyér Team 2013.10.20 16:06
#pragma once

class ITexture2D {
public:
	virtual size_t GetWidth() = 0;
	virtual size_t GetHeight() = 0;
	virtual void Release() = 0;
};