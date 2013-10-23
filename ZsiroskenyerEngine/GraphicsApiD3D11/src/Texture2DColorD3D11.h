// Texture2DColorD3D11.h By Zsíroskenyér Team 2013.10.20 16:42
#pragma once

#include "../../Core/src/ITexture2D.h"

struct ID3D11ShaderResourceView;

class cTexture2DColorD3D11 : public ITexture2D {
public:
	size_t GetWidth() override;
	size_t GetHeight() override;

	ID3D11ShaderResourceView* GetSRV();

	cTexture2DColorD3D11(ID3D11ShaderResourceView* srv, size_t width, size_t height);
	void Release() override;
private:
	ID3D11ShaderResourceView* srv;
	size_t width;
	size_t height;
};