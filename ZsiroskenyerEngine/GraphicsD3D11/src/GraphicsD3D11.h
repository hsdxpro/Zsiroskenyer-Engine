// GraphicsD3D11.h By Zsíroskenyér team 2013.10.10 1:32 last modified Németh Richárd
// IGraphicsApi interface implementation based on the 3D graphics SDK DirectX11
#pragma once

#include "Dx11_SDK/Include/D3D11.h"
#include <dxgi.h>
#include "Dx11_SDK/Include/D3Dcompiler.h"
#include "Dx11_SDK/Include/D3DX11.h"

#include "../../GraphicsEngine/src/IGraphicsApi.h"
#include "../../GraphicsEngine/src/IWindow.h"
#include "../../CommonLib/src/common.h"

#include "../../GraphicsEngine/src/IVertexBuffer.h"
#include "../../GraphicsEngine/src/IIndexBuffer.h"

#undef max

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

	// Old interface
	void BBClear(bool clearOnlyDepth = false) override;
	void Draw(uint32 vertexCount);
	void DrawIndexed(uint32 indexCount);
	void DrawIndexedInstanced(uint32 indexCount, uint32 instanceCount);
	void BBPresent() override;	

	void SetWindow(IWindow *renderWindow) override;
	void SetBBRenderTarget();

	// New interface
	// !written by Peti!
	void Clear(bool target=true, bool depth=false, bool stencil=false);
	void Preset();

	void Draw(size_t nVertices, size_t idxStartVertex=0);
	void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex=0, size_t idxStartInstance=0);
	void DrawIndexed(size_t nIndices, size_t idxStartIndex=0);
	void DrawIndexedInstanced(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex=0, size_t idxStartInstance=0);

	void SetVertexData(IVertexBuffer* vertexBuffer);
	void SetIndexData(IIndexBuffer* indexBuffer);
	void SetInstanceData(/*whatever*/);
	

	// Creating Buffers
	IVertexBuffer* CreateVertexBuffer(size_t size, eBufferUsage usage, void* data = NULL) override;
	IIndexBuffer* CreateIndexBuffer(size_t size  , eBufferUsage usage, void* data = NULL) override;
	// Write to buffers
	bool WriteBuffer(IIndexBuffer* buffer , void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;
	bool WriteBuffer(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;
	// Read from buffers
	bool ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset = 0) override;
	bool ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;

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
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsD3D11() {
		return new cGraphicsD3D11();
}