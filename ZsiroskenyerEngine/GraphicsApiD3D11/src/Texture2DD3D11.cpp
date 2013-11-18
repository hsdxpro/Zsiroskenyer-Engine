#include "Texture2DD3D11.h"

#include <d3d11.h>

cTexture2DD3D11::cTexture2DD3D11(size_t width, size_t height, ID3D11ShaderResourceView* srv /*= NULL*/, ID3D11RenderTargetView* rtv /*= NULL*/, ID3D11DepthStencilView* dsv /*= NULL*/)
:width(width), height(height), srv(srv), rtv(rtv), dsv(dsv) {
}

cTexture2DD3D11::~cTexture2DD3D11() {
	SAFE_RELEASE(srv);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(dsv);
}

void cTexture2DD3D11::Release() {
	delete this;
}

size_t cTexture2DD3D11::GetWidth() const {
	return width;
}

size_t cTexture2DD3D11::GetHeight() const {
	return height;
}

ID3D11ShaderResourceView* cTexture2DD3D11::GetSRV() {
	return srv;
}

ID3D11RenderTargetView* cTexture2DD3D11::GetRTV() {
	return rtv;
}

 ID3D11DepthStencilView* cTexture2DD3D11::GetDSV()  {
	return dsv;
}