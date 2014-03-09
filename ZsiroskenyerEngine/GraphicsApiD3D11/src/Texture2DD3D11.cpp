#include "Texture2DD3D11.h"

#include <d3d11.h>

// Constructor
cTexture2DD3D11::cTexture2DD3D11(
	size_t width,
	size_t height,
	ID3D11Texture2D* tex,
	ID3D11ShaderResourceView* srv /*= nullptr*/,
	ID3D11RenderTargetView* rtv /*= nullptr*/,
	ID3D11DepthStencilView* dsv /*= nullptr*/,
	ID3D11ShaderResourceView** subsrv,
	ID3D11RenderTargetView** subrtv,
	ID3D11DepthStencilView** subdsv,
	int nSubresource)
:width(width), height(height), tex(tex), srv(srv), rtv(rtv), dsv(dsv) {
	// set sub fucking resources
	for (int i = 0; i < nSubresource; i++) {
		this->subsrv.push_back(subsrv[i]);
	}
	for (int i = 0; i < nSubresource; i++) {
		this->subrtv.push_back(subrtv[i]);
	}
	for (int i = 0; i < nSubresource; i++) {
		this->subdsv.push_back(subdsv[i]);
	}
}
// Destructor
cTexture2DD3D11::~cTexture2DD3D11() {
	if (tex) {
		// release views
		SAFE_RELEASE(srv);
		SAFE_RELEASE(rtv);
		SAFE_RELEASE(dsv);

		// release sub-views
		for (auto v : subsrv)
			SAFE_RELEASE(v);
		for (auto v : subrtv)
			SAFE_RELEASE(v);
		for (auto v : subdsv)
			SAFE_RELEASE(v);

		// release resource
		SAFE_RELEASE(tex);
	}
}
// Release
void cTexture2DD3D11::Release() {
	delete this;
}

// User interface
size_t cTexture2DD3D11::GetWidth() const {
	return width;
}

size_t cTexture2DD3D11::GetHeight() const {
	return height;
}

cTexture2DD3D11* cTexture2DD3D11::GetArraySlice(int idx) {
	if (!(0 <= idx && (size_t)idx < subsrv.size()))
		return nullptr;
	return new cTexture2DD3D11(width, height, nullptr, subsrv[idx], subrtv[idx], subdsv[idx]);
}
const cTexture2DD3D11* cTexture2DD3D11::GetArraySlice(int idx) const {
	if (!(0 <= idx && (size_t)idx < subsrv.size()))
		return nullptr;
	return new cTexture2DD3D11(width, height, nullptr, subsrv[idx], subrtv[idx], subdsv[idx]);
}


// Internal interface
ID3D11Texture2D* const cTexture2DD3D11::Get() const {
	ASSERT(tex);
	return tex;
}

ID3D11ShaderResourceView* const cTexture2DD3D11::GetSRV() const {
	return srv;
}

ID3D11RenderTargetView* const cTexture2DD3D11::GetRTV() const {
	return rtv;
}

 ID3D11DepthStencilView* const cTexture2DD3D11::GetDSV() const {
	return dsv;
}