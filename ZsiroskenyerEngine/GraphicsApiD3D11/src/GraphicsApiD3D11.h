// GraphicsD3D11.h By Zsíroskenyér team 2013.10.10 1:32 last modified Németh Richárd
// IGraphicsApi interface implementation based on the 3D graphics SDK DirectX11
#pragma once
#ifdef _MSC_VER
#pragma warning(disable: 4244)
#pragma warning(disable: 4005)
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#undef ERROR_FILE_NOT_FOUND // fucking dx collides with eGapiResult::ERROR_FILE_NOT_FOUND
#undef max // fasz kivan már ezzel a buzi windowssal

#include "../../Core/src/GAPI.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/common.h"

#include <unordered_map>

class IVertexBuffer;
class IIndexBuffer;
class IConstantBuffer;
class IShaderProgram;
class IGraphicsApi;
class cTexture2DD3D11;
class cShaderProgramD3D11;
class cVertexBufferD3D11;

// DLL accessor
extern "C"
__declspec(dllexport)
IGraphicsApi* CreateGraphicsApiD3D11(IWindow* targetWindow, unsigned backBufferWidth, unsigned backBufferHeight);



class cGraphicsApiD3D11 : public IGraphicsApi {
public:
	cGraphicsApiD3D11();
	~cGraphicsApiD3D11();
	void Release() override;

	// --- resources --- //
	eGapiResult	CreateVertexBuffer(IVertexBuffer** resource, eUsage usage, cVertexFormat format, size_t size, void* data = nullptr) override;
	eGapiResult	CreateIndexBuffer(IIndexBuffer** resource, eUsage usage, size_t size, void* data = nullptr) override;
	eGapiResult CreateTexture(ITexture2D** resource, const wchar_t* filePath) override;
	eGapiResult CreateTexture(ITexture2D** resource, const ITexture2D::tDesc& desc, void* data = nullptr) override;
	eGapiResult CreateShaderProgram(IShaderProgram** resource, const wchar_t* shaderPath) override;

	eGapiResult WriteResource(IIndexBuffer* buffer, void* source, size_t size = ZS_NUMERIC_MAX(size_t), size_t offset = 0) override;
	eGapiResult WriteResource(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMERIC_MAX(size_t), size_t offset = 0) override;

	eGapiResult ReadResource(IIndexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;
	eGapiResult ReadResource(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;
	eGapiResult ReadResource(ITexture2D* texture, void* dest, size_t size, size_t offset = 0) override;

	eGapiResult CopyResource(ITexture2D* src, ITexture2D* dst) override;

	// --- render --- //
	void Clear(bool target = true, bool depth = false, bool stencil = false, const Vec4& clearColor = Vec4(0.3f, 0.3f, 0.3f, 1.0f)) override;
	void ClearTexture(ITexture2D* t, eClearFlag clearFlag = eClearFlag::COLOR_DEPTH_STENCIL, const Vec4& clearColor = Vec4(), float depthVal = 1.0f, size_t stencilVal = 0);

	void Draw(size_t nVertices, size_t idxStartVertex = 0) override;
	void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) override;
	void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) override;
	void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) override;

	void Present() override;

	// --- state --- //
	eGapiResult SetRenderTargetDefault() override;
	eGapiResult SetBackBufferSize(unsigned width, unsigned height) override;
	eGapiResult SetRenderTargets(unsigned nTargets, const ITexture2D* const* renderTargets, ITexture2D* depthStencilTarget = nullptr) override;

	eGapiResult SetVSConstantBuffer(const void* data, size_t size, size_t slotIdx) override;
	eGapiResult SetPSConstantBuffer(const void* data, size_t size, size_t slotIdx) override;
	eGapiResult SetTextureArray(const wchar_t* varName, const ITexture2D* const * t, uint8_t nTextures = 1) override;
	eGapiResult SetTexture(int slotIdx, const ITexture2D* t) override;
	eGapiResult SetTexture(const wchar_t* varName, const ITexture2D* t) override;
	void SetPrimitiveTopology(ePrimitiveTopology t) override;
	void SetShaderProgram(IShaderProgram* shProg) override;
	void SetInstanceData(/*whatever*/) override;
	void SetVertexBuffer(const IVertexBuffer* vb) override;
	void SetIndexBuffer(const IIndexBuffer* ib) override;

	eGapiResult SetDepthStencilState(const tDepthStencilDesc& desc, uint8_t stencilRef) override;
	eGapiResult SetBlendState(const tBlendDesc& desc) override;

	// --- misc --- //
	eGapiResult SetWindow(IWindow *renderWindow) override;


	// --- misc --- //
	ITexture2D* GetDefaultRenderTarget() const override;
	const wchar_t* GetLastErrorMsg() const override;

	// --- utility --- //
	eGapiResult SaveTextureToFile(ITexture2D* t, ITexture2D::eImageFormat f, const char* filePath) override;

private:
	eGapiResult CreateDevice();
	eGapiResult CreateMostAcceptableSwapChain(size_t width, size_t height, HWND windowHandle);
	eGapiResult CreateViewsForBB();
	eGapiResult CreateDefaultStates();
	void ApplyConstantBuffers();
	void ApplySamplerStates();

	HRESULT CompileShaderFromFile(const zsString& fileName, const zsString& entry, const zsString& profile, zsString* compilerMessage, ID3DBlob** ppBlobOut);
	void AutoSetInputLayout(cShaderProgramD3D11* shader, cVertexBufferD3D11* buffer);
	ID3D11InputLayout* GetInputLayout(cShaderProgramD3D11* shader, cVertexFormat bufferFormat);

	// Buffer convert
	DXGI_FORMAT ConvertToNativeFormat(eFormat fmt);
	unsigned ConvertToNativeBind(unsigned flags);
	D3D11_USAGE ConvertToNativeUsage(eUsage usage);

	// Blend convert
	D3D11_BLEND_OP ConvertToNativeBlendOp(eBlendOp blendOp);
	D3D11_BLEND ConvertToNativeBlendFactor(eBlendFactor blendFactor);
	uint8_t ConvertToNativeBlendMask(eBlendWriteMask blendMask);
	D3D11_BLEND_DESC ConvertToNativeBlend(tBlendDesc blend);

	// Depthstencil convert
	D3D11_COMPARISON_FUNC ConvertToNativeCompFunc(eComparisonFunc compFunc);
	D3D11_STENCIL_OP ConvertToNativeStencilOp(eStencilOp stencilOp);
	D3D11_DEPTH_STENCIL_DESC ConvertToNativeDepthStencil(const tDepthStencilDesc& depthStencil);

	// Sampler convert
	D3D11_SAMPLER_DESC ConvertToNativeSampler(const tSamplerStateDesc& sDesc);

	// Vertex format
	std::vector<D3D11_INPUT_ELEMENT_DESC> ConvertToNativeVertexFormat(cVertexFormat format);

protected:
	// Error handling
	zsString lastErrorMsg;
	
	// backBuffer will be the main render target
	cTexture2DD3D11* defaultRenderTarget;
	D3D11_VIEWPORT defaultVP;

	// Main Dx interfaces
	ID3D11DeviceContext *d3dcon;
	ID3D11Device *d3ddev;
	IDXGISwapChain *d3dsc;


	// Draw state stuff
	cShaderProgramD3D11* activeShaderProg;
	cVertexBufferD3D11* activeVertexBuffer;

	// Viewports and RTs
	D3D11_VIEWPORT	activeViewports[16];
	ID3D11RenderTargetView* activeRTVs[16];

	// Constant buffers handling
	void* vsConstBufferData; // These updated every ex. SetPSConstantBuffer(.....) call
	void* psConstBufferData;
	size_t vsConstBufferSize;
	size_t psConstBufferSize;
	ID3D11Buffer* vsConstBuffer; // Then these gets updates when Draw called
	ID3D11Buffer* psConstBuffer;
	bool vsConstBufferStateChanged; // Don't want to apply constant buffers when it's not changed
	bool psConstBufferStateChanged;

	// Inner state descriptor, for avoid hashing
	struct tDepthStencilLink {
		D3D11_DEPTH_STENCIL_DESC desc;
		ID3D11DepthStencilState* state;
	};
	struct tBlendLink {
		D3D11_BLEND_DESC desc;
		ID3D11BlendState* state;
	};
	struct tSamplerLink {
		D3D11_SAMPLER_DESC desc;
		ID3D11SamplerState* state;
	};

	std::vector<tDepthStencilLink> depthStencilStates;
	std::vector<tBlendLink> blendStates;
	std::vector<tSamplerLink> samplerStates;

	// input layout lazy creation
	struct ILHasher {
		size_t operator()(std::pair<cVertexFormat, cVertexFormat> obj){
			return std::hash<int64_t>()(obj.first.Raw() ^ obj.second.Raw());
		}
	};
	using InputLayoutMapT = std::unordered_map<std::pair<cVertexFormat, cVertexFormat>, ID3D11InputLayout*, ILHasher>;
	InputLayoutMapT inputLayoutStore;
};