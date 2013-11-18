// Texture2DColorD3D11.h By Zsíroskenyér Team 2013.10.20 16:42
#pragma once

#include "../../Core/src/ITexture2D.h"
#include "../../Core/src/common.h"

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

class cTexture2DD3D11 : public ITexture2D {
public:
	cTexture2DD3D11(size_t width, size_t height, ID3D11ShaderResourceView* srv = NULL, ID3D11RenderTargetView* rtv = NULL, ID3D11DepthStencilView* dsv = NULL);
	~cTexture2DD3D11();
	void Release() override;

	size_t GetWidth() const override;
	size_t GetHeight() const override;

	ID3D11ShaderResourceView* GetSRV();
	ID3D11RenderTargetView* GetRTV();
	ID3D11DepthStencilView* GetDSV();
protected:
	size_t width;
	size_t height;

	ID3D11ShaderResourceView* srv;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;
};