#include "Texture2DColorD3D11.h"

#include "Dx11_SDK/Include/d3d11.h"
#include "Dx11_SDK/Include/d3dx11.h"

cTexture2DColorD3D11::cTexture2DColorD3D11(ID3D11ShaderResourceView* srv, size_t width, size_t height)
:srv(srv), width(width), height(height) {
}

size_t cTexture2DColorD3D11::GetWidth() {
	return width;
}

size_t cTexture2DColorD3D11::GetHeight() {
	return height;
}