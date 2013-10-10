// GraphicsD3D11.h By Zsíroskenyér team 2013.10.10 1:32 last modified Németh Richárd
// IGraphicsApi interface implementation based on the 3D graphics SDK DirectX11
#pragma once

#include "Dx11_SDK/Include/D3D11.h"
#include <dxgi.h>
#include "Dx11_SDK/Include/D3Dcompiler.h"
#include "Dx11_SDK/Include/D3DX11.h"

#include "..\..\GraphicsEngine\src\IGraphicsApi.h"
#include "..\..\CommonLib\src\common.h"

class cGraphicsD3D11 : public IGraphicsApi {
public:
	// Configuration to construct with
	struct tDxConfig {
		tDxConfig();

		static tDxConfig DEFAULT;
		static tDxConfig MEDIUM;
		static tDxConfig HIGH;

		int multiSampleQuality;
		int multiSampleCount;

		// Create backbuffer at max screen resolution
		bool createDeviceAtMaxResolution;

		// FullScreen Dx App?
		bool createDeviceFullScreen;

	};

	void CreateVertexBuffer(uint32 vertexStride, uint32 nVertex, void *vertices, ID3D11Buffer **vb);
	void CreateIndexBuffer(uint32 indexStride, uint32 nIndex, void *indices, ID3D11Buffer **ib);

	void ClearBB(bool clearOnlyDSV);
	void Draw(uint32 vertexCount);
	void DrawIndexed(uint32 indexCount);
	void DrawIndexedInstanced(uint32 indexCount, uint32 instanceCount);
	void Present();	

	void SetWindow(uint16 swapChainWidth, uint16 swapChainHeight, HWND windowHandle, const tDxConfig& config = swapChainConfig);
	void SetBBRenderTarget();

	cGraphicsD3D11();
	~cGraphicsD3D11();
private:
	void CreateDevice();
	void CreateMostAcceptableSwapChain(uint16 width, uint16 height, HWND windowHandle, const tDxConfig& config);
	void CreateRenderTargetViewForBB(const tDxConfig& config);
	void CreateDefaultStates(const D3D11_CULL_MODE& cullMode, const D3D11_FILL_MODE& fillMode);
protected:
	// backBuffer will be the main render target
	ID3D11RenderTargetView *backBufferRTV;
	ID3D11DepthStencilView *backBufferDSV;
	D3D11_VIEWPORT backBufferVP;
	uint32 bbWidth;
	uint32 bbHeight;

	// Main Dx interfaces
	ID3D11DeviceContext *d3dcon;
	ID3D11Device *d3ddev;
	IDXGISwapChain *d3dsc;
	static tDxConfig swapChainConfig;

};

// DLL accessor
#ifdef __cplusplus
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsD3D11() {
		return new cGraphicsD3D11();
#endif
}