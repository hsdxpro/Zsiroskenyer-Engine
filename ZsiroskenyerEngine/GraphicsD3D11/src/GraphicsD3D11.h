// GraphicsD3D11.h By Zsíroskenyér team 2013.10.10 1:32 last modified Németh Richárd
// IGraphicsApi interface implementation based on the 3D graphics SDK DirectX11
#pragma once

#include "Dx11_SDK/Include/D3D11.h"
#include <dxgi.h>
#include "Dx11_SDK/Include/D3Dcompiler.h"
#include "Dx11_SDK/Include/D3DX11.h"

#include "../../GraphicsCommon/src/IGraphicsApi.h"
#include "../../GraphicsCommon/src/IWindow.h"
#include "../../CommonLib/src/common.h"

#include "../../GraphicsCommon/src/IVertexBuffer.h"
#include "../../GraphicsCommon/src/IIndexBuffer.h"
#include "../../GraphicsCommon/src/IConstantBuffer.h"
#include "../../GraphicsCommon/src/IShaderProgram.h"

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

	// Compiling profiles for cg
	enum class eProfileCG {
		VS_5_0,
		PS_5_0,
		HS_5_0,
		DS_5_0,
		GS_5_0,
	};

	// buffers
	IVertexBuffer* CreateVertexBuffer(size_t nVertices, size_t vertexStride, eBufferUsage usage, void* data = NULL) override;
	IIndexBuffer* CreateIndexBuffer(size_t size, eBufferUsage usage, void* data = NULL) override;
	IConstantBuffer* CreateConstantBuffer(size_t size, eBufferUsage usage, void* data = NULL) override;

	bool WriteBuffer(IIndexBuffer* buffer , void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;
	bool WriteBuffer(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;

	bool ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset = 0) override;
	bool ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;

	void LoadConstantBuffer(IConstantBuffer* buffer, size_t slotIdx);

	// draw
	void Clear(bool target = true, bool depth = false, bool stencil = false) override;
	void Present() override;

	void Draw(size_t nVertices, size_t idxStartVertex = 0) override;
	void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) override;
	void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) override;
	void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) override;

	void SetRenderTargetDefault() override;

	void SetVertexData(const IVertexBuffer* vertexBuffer, size_t vertexSize) override;
	void SetIndexData(const IIndexBuffer* indexBuffer) override;
	void SetInstanceData(/*whatever*/) override;

	void cGraphicsD3D11::SetShaderProgram(IShaderProgram* shProg);

	// shader
	IShaderProgram* CreateShaderProgram(const zsString& shaderPath) override;

	// misc
	void SetWindow(IWindow *renderWindow) override;

	// constructor
	cGraphicsD3D11();
	~cGraphicsD3D11();
private:
	void CreateDevice();
	void CreateMostAcceptableSwapChain(uint16 width, uint16 height, HWND windowHandle, const tDxConfig& config);
	void CreateRenderTargetViewForBB(const tDxConfig& config);
	void CreateDefaultStates(const D3D11_CULL_MODE& cullMode, const D3D11_FILL_MODE& fillMode);
	HRESULT CompileShaderFromFile(const zsString& fileName, const zsString& entry, const zsString& profile, ID3DBlob** ppBlobOut);
	void CompileCgToHLSL(const zsString& cgFileName, const zsString& hlslFileName, eProfileCG compileProfile);
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