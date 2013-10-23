// Texture2DColorD3D11.h By Zsíroskenyér Team 2013.10.20 16:42
#pragma once

#include "../../GraphicsApi/src/ITexture2D.h"

struct ID3D11ShaderResourceView;

class cTexture2DColorD3D11 : public ITexture2D {
public:
	size_t GetWidth() override;
	size_t GetHeight() override;

	cTexture2DColorD3D11(ID3D11ShaderResourceView* srv, size_t width, size_t height);
private:
	ID3D11ShaderResourceView* srv;
	size_t width;
	size_t height;
};