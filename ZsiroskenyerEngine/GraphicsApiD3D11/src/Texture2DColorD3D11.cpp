#include "Texture2DColorD3D11.h"

#include <d3d11.h>

cTexture2DColorD3D11::cTexture2DColorD3D11(ID3D11ShaderResourceView* srv, size_t width, size_t height)
:srv(srv), width(width), height(height) {
}

size_t cTexture2DColorD3D11::GetWidth() {
	return width;
}

size_t cTexture2DColorD3D11::GetHeight() {
	return height;
}

ID3D11ShaderResourceView* cTexture2DColorD3D11::GetSRV() {
	return srv;
}

void cTexture2DColorD3D11::Release() {
	srv->Release();
}