#include "../../Core/src/PipelineState.h"

#include "GraphicsApiD3D11.h"

#include "Dx11_SDK/Include/d3dx11.h"

#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "ShaderProgramD3D11.h"
#include "Texture2DD3D11.h"

#include "../../Core/src/common.h"
#include "../../Core/src/StrUtil.h"
#include "../../Core/src/FileUtil.h"

#include "../../Core/src/CgShaderHelper.h"

#include <unordered_map>
#include <fstream>
#include <memory>
#include <memory>
// Ugly create shader last_write_time..
//#include <boost/filesystem.hpp>

////////////////////////////////////////////////////////////////////////////////
// GraphicsApi instance creation
extern "C"
__declspec(dllexport)
IGraphicsApi* CreateGraphicsApiD3D11(IWindow* targetWindow, unsigned backBufferWidth, unsigned backBufferHeight) {
	cGraphicsApiD3D11* gApi = NULL;
	try {
		gApi = new cGraphicsApiD3D11();
		gApi->SetWindow(targetWindow);
		gApi->SetBackBufferSize(backBufferWidth, backBufferHeight);
		return gApi;
	}
	catch (std::exception&) {
		return NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Internal helper functions

// convert stuff to d3d11-native format
DXGI_FORMAT ConvertToNativeFormat(eFormat fmt);
unsigned ConvertToNativeBind(unsigned flags);
D3D11_USAGE ConvertToNativeUsage(eUsage usage);

D3D11_BLEND_OP ConvertToNativeBlendOp(eBlendOp blendOp);
D3D11_BLEND ConvertToNativeBlendFactor(eBlendFactor blendFactor);
uint8_t ConvertToNativeBlendMask(eBlendWriteMask blendMask);
D3D11_BLEND_DESC ConvertToNativeBlend(tBlendDesc blend);

D3D11_COMPARISON_FUNC ConvertToNativeCompFunc(eComparisonFunc compFunc);
D3D11_STENCIL_OP ConvertToNativeStencilOp(eStencilOp stencilOp);
D3D11_DEPTH_STENCIL_DESC ConvertToNativeDepthStencil(const tDepthStencilDesc& depthStencil);
D3D11_SAMPLER_DESC ConvertToNativeSampler(const tSamplerDesc& sDesc);

std::vector<D3D11_INPUT_ELEMENT_DESC> ConvertToNativeVertexFormat(cVertexFormat format);

////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor

cGraphicsApiD3D11::cGraphicsApiD3D11()
: 
d3ddev(NULL),
d3dcon(NULL),
d3dsc(NULL),
defaultRenderTarget(NULL),
activeShaderProg(NULL),
activeVertexBuffer(NULL)
{
	// Create d3ddevice, d3dcontext
	auto r = CreateDevice();
	if (r != eGapiResult::OK) {
		throw std::runtime_error("failed to create direct3d device");
	}

	// Const buffer handling init
	vsConstBuffer = psConstBuffer = NULL;
	vsConstBufferData = psConstBufferData = NULL;
	vsConstBufferSize = psConstBufferSize = 0;
	vsConstBufferStateChanged = psConstBufferStateChanged = false;
	
	// Create default states
	r = CreateDefaultStates();
	if (r != eGapiResult::OK) {
		throw std::runtime_error("failed to create default states");
	}

	memset(activeViewports, 0, 16 * sizeof(D3D11_VIEWPORT));
	memset(activeRTVs, 0, 16 * sizeof(ID3D11RenderTargetView*));
}

void cGraphicsApiD3D11::Release() {
	delete this;
}
cGraphicsApiD3D11::~cGraphicsApiD3D11() {
	for (size_t i = 0; i < depthStencilStates.size(); i++)
		SAFE_RELEASE(depthStencilStates[i].state);

	for (size_t i = 0; i < blendStates.size(); i++)
		SAFE_RELEASE(blendStates[i].state);

	for (size_t i = 0; i < samplerStates.size(); i++)
		SAFE_RELEASE(samplerStates[i].state);

	ID3D11ShaderResourceView* nullSrvs[16] = { 0 };
	d3dcon->PSSetShaderResources(0, 16, nullSrvs);
	d3dcon->VSSetShaderResources(0, 16, nullSrvs);

	ID3D11SamplerState* nullSamplers[16] = { 0 };
	d3dcon->VSSetSamplers(0, 16, nullSamplers);
	d3dcon->PSSetSamplers(0, 16, nullSamplers);

	ID3D11RenderTargetView *nulltargets[4] = { 0 };
	d3dcon->OMSetRenderTargets(4, nulltargets, 0);

	for (size_t i = 0; i < 16; i++)
		SAFE_RELEASE(activeRTVs[i]);

	if (d3dcon)d3dcon->ClearState();
	if (d3dcon)d3dcon->Flush();

	SAFE_DELETE_ARRAY(vsConstBufferData);
	SAFE_DELETE_ARRAY(psConstBufferData);
	SAFE_RELEASE(vsConstBuffer);
	SAFE_RELEASE(psConstBuffer);
	SAFE_RELEASE(defaultRenderTarget);
	SAFE_RELEASE(d3ddev);
	SAFE_RELEASE(d3dcon);
	SAFE_RELEASE(d3dsc);
}


////////////////////////////////////////////////////////////////////////////////
// Direct3D helper functions

eGapiResult cGraphicsApiD3D11::CreateDevice() {
	// create Graphic Infrastructure factory
	IDXGIFactory* fact = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&fact);

	// enumerate adapters
	IDXGIAdapter* mainAdapter = NULL;
	UINT adapterIndex = 0;
	fact->EnumAdapters(adapterIndex, &mainAdapter);

	// feature level array
	const D3D_FEATURE_LEVEL featurelevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// go from high feature level to lower, try to create device with each
	D3D_FEATURE_LEVEL featurelevel;
	int i = 0;
	const int nFeatureLevels = ARRAYSIZE(featurelevels);
	for (; i < nFeatureLevels; i++, featurelevel = featurelevels[i]) {
		HRESULT hr = D3D11CreateDevice(mainAdapter, D3D_DRIVER_TYPE_UNKNOWN, 0, 0, featurelevels, nFeatureLevels, D3D11_SDK_VERSION, &d3ddev, &featurelevel, &d3dcon);

		// Wrap mode... for gpu's not supporting hardware accelerated D3D11
		//HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP , 0, flags, featurelevels, nFeatureLevels, D3D11_SDK_VERSION, &d3ddev, &featurelevel, &d3dcon); // For dx9 machines
		if (!FAILED(hr))
			break;
		else {
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
		}
	}

	if (i == ARRAYSIZE(featurelevels)) {
		ASSERT_MSG(false, L"Can't create DirectX Device");
		return eGapiResult::ERROR_UNKNOWN;
	}

	SAFE_RELEASE(fact);
	SAFE_RELEASE(mainAdapter);

	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::CreateMostAcceptableSwapChain(size_t width, size_t height, HWND windowHandle) {
	if (d3dsc != NULL)
		SAFE_RELEASE(d3dsc);

	DXGI_FORMAT bbFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	// create Graphic Infrastructure factory
	IDXGIFactory* fact = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&fact);

	// enumerate adapters
	IDXGIAdapter* mainAdapter = NULL;
	UINT adapterIndex = 0;
	fact->EnumAdapters(adapterIndex, &mainAdapter);

	// enumerate outputs
	IDXGIOutput* mainOutput = NULL;
	DXGI_MODE_DESC* modeDesc = NULL;
	UINT outputIndex = 0;
	mainAdapter->EnumOutputs(outputIndex, &mainOutput);

	// how many displayModes ?
	UINT numModes = 0;
	mainOutput->GetDisplayModeList(bbFormat, 0, &numModes, 0);

	// get those displayModes
	modeDesc = new DXGI_MODE_DESC[numModes];
	mainOutput->GetDisplayModeList(bbFormat, 0, &numModes, modeDesc);

	// select displayModes that matches our renderWindow params and DirectX Config
	// worst case sized array
	DXGI_MODE_DESC** filteredVideoModes = new DXGI_MODE_DESC*[numModes];

	UINT displayModeIndex = 0;
	for (size_t i = 0; i < numModes; i++) {
		DXGI_MODE_DESC* currMode = &modeDesc[i];
		/*
		// Collect The best resolution that the video card handle
		if (config.createDeviceAtMaxResolution) {
			// add to matched videoModes if that VideoMode have full screen resolution
			if (GetSystemMetrics(SM_CXSCREEN) == currMode->Width && GetSystemMetrics(SM_CYSCREEN) == currMode->Height) {
				filteredVideoModes[displayModeIndex] = currMode;
				displayModeIndex++;
			}
			// Collect videoModes which resoltuion is equal with the window passed to ManagerDx

		}*/
		if (currMode->Width == width && currMode->Height == height) {
			filteredVideoModes[displayModeIndex] = currMode;
			displayModeIndex++;
		}
	}

	// select highest HZ videoMode
	DXGI_MODE_DESC* selectedVideoMode = 0;
	UINT maxHz = 0;
	for (size_t i = 0; i < displayModeIndex; i++) {
		DXGI_MODE_DESC* curr = filteredVideoModes[i];
		if (maxHz < curr->RefreshRate.Numerator) {
			maxHz = curr->RefreshRate.Numerator;
			selectedVideoMode = curr;
		}
	}

	// Use the selected display mode to fill swap chain description
	DXGI_SWAP_CHAIN_DESC sdesc;
	sdesc.BufferCount = 1;
	if (selectedVideoMode != NULL) {
		sdesc.BufferDesc = *selectedVideoMode; // Copy DisplayMode Data
	}
	else {
		ASSERT_MSG(false, L"Using non standard resolution, this may slow down the DirectX application");
		sdesc.BufferDesc.Format = bbFormat;
		sdesc.BufferDesc.Width = width;
		sdesc.BufferDesc.Height = height;
		sdesc.BufferDesc.RefreshRate.Numerator = 1;
		sdesc.BufferDesc.RefreshRate.Denominator = 0;
		sdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	}
	sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	sdesc.OutputWindow = windowHandle;
	sdesc.SampleDesc.Count = 1;// config.multiSampleCount;
	sdesc.SampleDesc.Quality = 0;// config.multiSampleQuality;
	sdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//if (!config.createDeviceFullScreen)
		sdesc.Windowed = true;

	HRESULT hr = fact->CreateSwapChain(d3ddev, &sdesc, &d3dsc);

	// free up everything
	SAFE_RELEASE(mainOutput);
	SAFE_RELEASE(mainAdapter);
	SAFE_RELEASE(fact);

	SAFE_DELETE_ARRAY(filteredVideoModes);
	SAFE_DELETE_ARRAY(modeDesc);

	switch (hr) {
	case S_OK:
		return eGapiResult::OK;
	case E_OUTOFMEMORY:
		return eGapiResult::ERROR_OUT_OF_MEMORY;
	default:
		return eGapiResult::ERROR_UNKNOWN;
	}
}

eGapiResult cGraphicsApiD3D11::CreateViewsForBB() {
	SAFE_RELEASE(defaultRenderTarget);

	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
	ID3D11DepthStencilView* dsv;

	ID3D11Texture2D *backBuffer = NULL;
	HRESULT hr = d3dsc->GetBuffer(NULL, __uuidof(ID3D11Resource), (void**)&backBuffer);
	if (FAILED(hr)) {
		ASSERT_MSG(false, L"Failed to Get SwapChain buffer");
		return eGapiResult::ERROR_UNKNOWN;
	}
	hr = d3ddev->CreateRenderTargetView(backBuffer, 0, &rtv);
	if (FAILED(hr)) {
		ASSERT_MSG(false, L"Failed to create render target view for SwapChain's BackBuffer");
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}

	hr = d3ddev->CreateShaderResourceView(backBuffer, NULL, &srv);
	if (FAILED(hr)) {
		ASSERT_MSG(false, L"Failed to create shader resource view for SwapChain's BackBuffer");
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}

	D3D11_TEXTURE2D_DESC bbDesc;
	backBuffer->GetDesc(&bbDesc);

	// create Depth texture
	ID3D11Texture2D *depthTexture = NULL;
	D3D11_TEXTURE2D_DESC tD;
	tD.ArraySize = 1;
	tD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tD.CPUAccessFlags = 0;
	tD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tD.Width = bbDesc.Width;
	tD.Height = bbDesc.Height;
	tD.MipLevels = 1;
	tD.MiscFlags = 0;
	tD.SampleDesc.Count = 1;// config.multiSampleCount;
	tD.SampleDesc.Quality = 0;// config.multiSampleQuality;
	tD.Usage = D3D11_USAGE_DEFAULT;
	hr = d3ddev->CreateTexture2D(&tD, 0, &depthTexture);
	if (FAILED(hr)) {
		SAFE_RELEASE(backBuffer);

		ASSERT_MSG(false, L"Failed to create depth buffer for swapChain");

		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}

	// create DepthStencilView
	hr = d3ddev->CreateDepthStencilView(depthTexture, NULL, &dsv);

	if (FAILED(hr)) {
		ASSERT_MSG(false, L"Failed to create depth buffer VIEW for swapChain");
		SAFE_RELEASE(depthTexture);
		SAFE_RELEASE(backBuffer);
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}

	defaultRenderTarget = new cTexture2DD3D11(bbDesc.Width, bbDesc.Height, backBuffer, srv, rtv, dsv);

	SAFE_RELEASE(depthTexture);
	SAFE_RELEASE(backBuffer);

	return eGapiResult::OK;
}


eGapiResult cGraphicsApiD3D11::CreateDefaultStates() {

	// Default geometry topology
	d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Default samplers DESC
	D3D11_SAMPLER_DESC d;
	d.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	d.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d.MaxAnisotropy = 16;
	d.MaxLOD = 0;
	d.MinLOD = 0;
	d.MipLODBias = 0;

	static ID3D11SamplerState* defaultSamplers[16];
	// Create 16 default sampler lol..
	for (size_t i = 0; i < 16; i++)
	{
		HRESULT hr = d3ddev->CreateSamplerState(&d, &defaultSamplers[i]);

		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create default sampler state");
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_UNKNOWN;
		}

	}

	// Set default samplers
	d3dcon->PSSetSamplers(0, 16, defaultSamplers);

	return eGapiResult::OK;
}

void cGraphicsApiD3D11::ApplyConstantBuffers() {
	D3D11_MAPPED_SUBRESOURCE mapped;
	// Update vertex shader constants
	if (vsConstBufferStateChanged && vsConstBuffer) {
		d3dcon->Map(vsConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
			memcpy(mapped.pData, vsConstBufferData, vsConstBufferSize);
		d3dcon->Unmap(vsConstBuffer, 0);
	}
	
	if (psConstBufferStateChanged && psConstBuffer) {
		// Update pixel shader constants
		d3dcon->Map(psConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
			memcpy(mapped.pData, psConstBufferData, psConstBufferSize);
		d3dcon->Unmap(psConstBuffer, 0);
	}

	vsConstBufferStateChanged = psConstBufferStateChanged = false;
}

void cGraphicsApiD3D11::ApplySamplerStates() {
	// Set VertexShader samplers
	auto vsSamplerStates = activeShaderProg->GetSamplerStatesVS();
	for (size_t i = 0; i < vsSamplerStates.size(); i++) {
		d3dcon->VSSetSamplers(vsSamplerStates[i].slotIdx, 1, &samplerStates[vsSamplerStates[i].gApiSamplerIdx].state);
	}

	// Set PixelShader samplers
	auto psSamplerStates = activeShaderProg->GetSamplerStatesPS();
	for (size_t i = 0; i < psSamplerStates.size(); i++) {
		d3dcon->PSSetSamplers(psSamplerStates[i].slotIdx, 1, &samplerStates[psSamplerStates[i].gApiSamplerIdx].state);
	}
}

////////////////////////////////////////////////////////////////////////////////
//	Manage graphics resources

// Create index and vertex buffes for indexed poly-meshes
eGapiResult	cGraphicsApiD3D11::CreateVertexBuffer(IVertexBuffer** resource, eUsage usage, cVertexFormat format, size_t size, void* data/*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = ConvertToNativeUsage(usage);
	desc.CPUAccessFlags = 0;
	if (desc.Usage == D3D11_USAGE_DYNAMIC)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if (desc.Usage == D3D11_USAGE_STAGING)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA resData;
	resData.pSysMem = data;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	switch (hr) {
	case S_OK:
		*resource = new cVertexBufferD3D11(buffer, usage, format, desc.ByteWidth);
		return eGapiResult::OK;
	case E_OUTOFMEMORY:
		return eGapiResult::ERROR_OUT_OF_MEMORY;
	default:
		return eGapiResult::ERROR_UNKNOWN;
	}
}

eGapiResult	cGraphicsApiD3D11::CreateIndexBuffer(IIndexBuffer** resource, eUsage usage, size_t size, void* data/*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = ConvertToNativeUsage(usage);
	desc.CPUAccessFlags = 0;
	if (desc.Usage == D3D11_USAGE_DYNAMIC)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if (desc.Usage == D3D11_USAGE_STAGING)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA resData;
	resData.pSysMem = data;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	switch (hr) {
	case S_OK:
		*resource = new cIndexBufferD3D11(buffer, desc.ByteWidth, usage);
		return eGapiResult::OK;
	case E_OUTOFMEMORY:
		return eGapiResult::ERROR_OUT_OF_MEMORY;
	default:
		return eGapiResult::ERROR_UNKNOWN;
	}
}

// Create texture from file
eGapiResult cGraphicsApiD3D11::CreateTexture(ITexture2D** resource, const wchar_t* filePath) {
	// Shader Resource View of texture
	ID3D11ShaderResourceView* srv;
	D3DX11_IMAGE_LOAD_INFO info;
	info.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(d3ddev, filePath, &info, 0, &srv, 0);

	switch (hr) {
		case S_OK: {
			// Get Width, Height
			size_t width;
			size_t height;

			ID3D11Texture2D* tex2D;
			srv->GetResource((ID3D11Resource**)&tex2D);

			D3D11_TEXTURE2D_DESC texDesc; tex2D->GetDesc(&texDesc);
			width = texDesc.Width;
			height = texDesc.Height;
			tex2D->Release();

			// return
			*resource = new cTexture2DD3D11(width, height, tex2D, srv);
			return eGapiResult::OK;
		}
		case D3D11_ERROR_FILE_NOT_FOUND:
			return eGapiResult::ERROR_FILE_NOT_FOUND;
		case E_OUTOFMEMORY:
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		default:
			return eGapiResult::ERROR_UNKNOWN;
	}
}

// Create texture in memory
eGapiResult cGraphicsApiD3D11::CreateTexture(ITexture2D** resource, ITexture2D::tDesc desc, void* data /*= NULL*/) {
	ID3D11Texture2D* tex = NULL;
	// Outputs
	ID3D11RenderTargetView* rtv = NULL;
	ID3D11ShaderResourceView* srv = NULL;
	ID3D11DepthStencilView* dsv = NULL;

	bool isRenderTarget = 0 != ((int)desc.bind & (int)eBind::RENDER_TARGET);
	bool isShaderBindable = 0 != ((int)desc.bind & (int)eBind::SHADER_RESOURCE);
	bool hasDepthStencil = 0 != ((int)desc.bind & (int)eBind::DEPTH_STENCIL);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = desc.arraySize;
	texDesc.BindFlags = ConvertToNativeBind(desc.bind);
	texDesc.Format = ConvertToNativeFormat(desc.format);
	texDesc.Height = desc.height;
	texDesc.Width = desc.width;
	texDesc.MipLevels = desc.mipLevels;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = ConvertToNativeUsage(desc.usage); // TODO ( PARAMETERIZE )
	texDesc.CPUAccessFlags = [&]()->UINT{
		if (texDesc.Usage == D3D11_USAGE_DYNAMIC)
			return D3D11_CPU_ACCESS_WRITE;
		else if (texDesc.Usage == D3D11_USAGE_STAGING)
			return D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		else
			return 0;
	}();

	HRESULT hr = S_OK;
	// make the format typeless if it has depth:
	if (hasDepthStencil) {
		switch (desc.depthFormat) {
		case eFormat::D16_UNORM: texDesc.Format = DXGI_FORMAT_R16_TYPELESS; break;
		case eFormat::D24_UNORM_S8_UINT: texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; break;
		case eFormat::D32_FLOAT: texDesc.Format = DXGI_FORMAT_R32_TYPELESS; break;
		case eFormat::D32_FLOAT_S8X24_UINT: texDesc.Format = DXGI_FORMAT_R32G32_TYPELESS; break;
		}
	}

	// create texture resource
	hr = d3ddev->CreateTexture2D(&texDesc, NULL, &tex);
	if (FAILED(hr)) {
		lastErrorMsg = L"failed to create resource";
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_INVALID_ARG;
	}

	// view descriptors
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC* pSrvDesc = NULL;
	// create views as needed
	if (hasDepthStencil) {
		// fill ds view desc
		memset(&dsvDesc, 0, sizeof(dsvDesc));
		dsvDesc.Format = ConvertToNativeFormat(desc.depthFormat);
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		// fill sr view desc
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = ConvertToNativeFormat(desc.format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		pSrvDesc = &srvDesc;

		hr = d3ddev->CreateDepthStencilView(tex, &dsvDesc, &dsv);
		if (FAILED(hr)) {
			SAFE_RELEASE(rtv);
			SAFE_RELEASE(srv);
			SAFE_RELEASE(tex);
			lastErrorMsg = L"failed to create depth-stencil view";
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_INVALID_ARG;
		}
	}
	if (isRenderTarget) {
		hr = d3ddev->CreateRenderTargetView(tex, NULL, &rtv);
		if (FAILED(hr)) {
			SAFE_RELEASE(tex);
			lastErrorMsg = L"failed to create render target view";
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_INVALID_ARG;
		}
	}
	if (isShaderBindable) {
		hr = d3ddev->CreateShaderResourceView(tex, pSrvDesc, &srv);
		if (FAILED(hr)) {
			SAFE_RELEASE(rtv);
			SAFE_RELEASE(tex);
			lastErrorMsg = L"failed to create shader resource view";
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_INVALID_ARG;
		}
	}

	*resource = new cTexture2DD3D11(desc.width, desc.height, tex, srv, rtv, dsv);
	return eGapiResult::OK;
}


// Create shader program
HRESULT cGraphicsApiD3D11::CompileShaderFromFile(const zsString& fileName, const zsString& entry, const zsString& profile, zsString* compilerMessage, ID3DBlob** ppBlobOut) {
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_PREFER_FLOW_CONTROL;// D3D10_SHADER_OPTIMIZATION_LEVEL3;

	ID3DBlob* pErrorBlob;
	char ansiEntry[256];
	char ansiProfile[256];
	wcstombs(ansiEntry, entry.c_str(), 256);
	wcstombs(ansiProfile, profile.c_str(), 256);

	hr = D3DX11CompileFromFileW(fileName.c_str(), NULL, NULL, ansiEntry, ansiProfile,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr) && pErrorBlob) {
		char* errorStr = (char*)pErrorBlob->GetBufferPointer();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, errorStr, strlen(errorStr), NULL, 0);
		zsString errorStrW(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, errorStr, strlen(errorStr), &errorStrW[0], size_needed);

		*compilerMessage = errorStrW;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return hr;
}

eGapiResult cGraphicsApiD3D11::CreateShaderProgram(IShaderProgram** resource, const wchar_t* shaderPath_) {
	const zsString shaderPath(shaderPath_);
	
	// File not exists
	std::ifstream is(shaderPath.c_str());
	if (!is.is_open())
	{
		lastErrorMsg = L"Shader file doesn't exists:  " + shaderPath;
		return eGapiResult::ERROR_FILE_NOT_FOUND;
	}
	is.close();

	// TODO read last write of that shader if last_write < curr_last_write (elavult)
	//last_write_time()
	//boost::filesystem::last_write_time(boost::filesystem::path(shaderPath.c_str()));
	//LINK : fatal error LNK1104: cannot open file 'libboost_filesystem-vc120-mt-sgd-1_55.lib'

	zsString pathNoExt = shaderPath.substr(0, shaderPath.size() - 3);

	// For array indexing convenience
	enum eDomainIdx {
		VS = 0,
		DS = 1,
		HS = 2,
		GS = 3,
		PS = 4,
	};
	const size_t nDomains = 5;
	
	cCgShaderHelper cgHelper;
	cCgShaderHelper::tCgInfo cgInfo = cgHelper.LoadCgShader(shaderPath_);
	if (cgHelper.GetLastErrorMsg() != NULL) {
		lastErrorMsg = cgHelper.GetLastErrorMsg();
		return eGapiResult::ERROR_UNKNOWN;
	}

	bool						existingEntries	[nDomains]; memset(existingEntries, 0, nDomains * sizeof(bool));
	zsString					entryNames		[nDomains]; // BULLSHIT Only CG need that for parsing
	zsString					binPaths		[nDomains];
	zsString					profileNames	[nDomains];
	cCgShaderHelper::eProfileCG cgProfiles		[nDomains];

	// Texture slots per domain
	std::unordered_map<zsString, uint16_t> shaderTextureSlots[nDomains];

	// Samppler states per domain
	std::vector<cShaderProgramD3D11::tSamplerInfo> shaderSamplerStates[nDomains];

	// VertexShader entry
	existingEntries[VS] = cgInfo.vsExists;
	entryNames[VS] = cgInfo.vsEntryName;
	binPaths[VS] = pathNoExt + L"_vs.bin";
	profileNames[VS] = L"vs_4_0";
	cgProfiles[VS] = cCgShaderHelper::eProfileCG::VS_4_0;

	// GeometryShader entry
	existingEntries[GS] = cgInfo.gsExists;
	entryNames[GS] = cgInfo.gsEntryName;
	binPaths[GS] = pathNoExt + L"_gs.bin";
	profileNames[GS] = L"gs_4_0";
	cgProfiles[GS] = cCgShaderHelper::eProfileCG::GS_4_0;

	// PixelShader entry
	existingEntries[PS] = cgInfo.psExists;
	entryNames[PS] = cgInfo.psEntryName;
	binPaths[PS] = pathNoExt + L"_ps.bin";
	profileNames[PS] = L"ps_4_0";
	cgProfiles[PS] = cCgShaderHelper::eProfileCG::PS_4_0;

	// TODO FORCING ALWAYS GENERATE SHADERS FROM CG's 
	bool binExistences[nDomains]; memset(binExistences, 0, nDomains * sizeof(bool));

	// ALWAYS LOAD THE FUCKING BINARY PLEASE :)
	//bool binExistences[nDomains]; memset(binExistences, 1, nDomains * sizeof(bool));

	// Shader Output data
	ID3D11InputLayout*		inputLayout = NULL;
	ID3D11VertexShader*		vs = NULL;
	ID3D11HullShader*		hs = NULL;
	ID3D11DomainShader*		ds = NULL;
	ID3D11GeometryShader*	gs = NULL;
	ID3D11PixelShader*		ps = NULL;

	// Shader ByteCodes
	ID3DBlob* blobs[nDomains];			memset(blobs, 0, nDomains * sizeof(ID3DBlob*));
	void* byteCodes[nDomains];			memset(byteCodes, 0, nDomains* sizeof(size_t));
	uint32_t byteCodeSizes[nDomains];	memset(byteCodeSizes, 0, nDomains * sizeof(size_t));

	std::vector<std::shared_ptr<char>> tmpByteCodes;
	for (size_t i = 0; i < nDomains; i++) tmpByteCodes.push_back(std::shared_ptr<char>(new char[512000]));

	for (size_t i = 0; i < nDomains; i++) {
		// not existing entry point skip it
		if (!existingEntries[i])
			continue;

		// Binary exists, read it
		if (binExistences[i]) {
			
			std::ifstream binFile(binPaths[i].c_str(), std::ios::binary);

			// Read shader byte code size
			cFileUtil::Read(binFile, byteCodeSizes[i]);

			// Read shader byte code
			cFileUtil::Read(binFile, tmpByteCodes[i].get(), byteCodeSizes[i]);

			byteCodes[i] = tmpByteCodes[i].get();

			// Read textureSlots "descriptor"
			uint8_t nTextureSlots;
			cFileUtil::Read(binFile, nTextureSlots);

			// Read each slot
			wchar_t slotName[512];
			
			//zsString slotName;
			uint16_t slotNameLength;
			uint16_t slotIdx;
			for (uint8_t j = 0; j < nTextureSlots; j++) {
				// Read slot name length
				cFileUtil::Read(binFile, slotNameLength);

				// Read slot name
				cFileUtil::Read(binFile, slotName, slotNameLength * sizeof(wchar_t));
				slotName[slotNameLength] = '\0';

				// Read slot index
				cFileUtil::Read(binFile, slotIdx);

				zsString name = slotName;
				shaderTextureSlots[i][name] = slotIdx;
			}

			binFile.close();
			
		}
		else { // binary doesn't exists
			
			// Compile cg to (hlsl)
			cgHelper.CompileCg(shaderPath, binPaths[i], cgProfiles[i]);

			// Compile (hlsl) to bytecode
			zsString compMessage;
			HRESULT hr = CompileShaderFromFile(binPaths[i], L"main", profileNames[i], &compMessage, &blobs[i]);
			if (FAILED(hr)) {
				lastErrorMsg = L"Failed to compile hlsl file, something is wrong with the CG file: " + shaderPath;
				return eGapiResult::ERROR_UNKNOWN;
			}
			
			// Bytecode info
			byteCodes[i] = blobs[i]->GetBufferPointer();
			byteCodeSizes[i] = blobs[i]->GetBufferSize();

			// Gather samplers etc..
			shaderTextureSlots[i] = cgHelper.GetHLSLTextureSlots(binPaths[i]);

			// OverWrite file with
			std::ofstream binFile(binPaths[i], std::ios::trunc | std::ios::binary);

			// Shader byte code size
			cFileUtil::Write(binFile, byteCodeSizes[i]);
				
			// Shader byte code
			cFileUtil::Write(binFile, byteCodes[i], byteCodeSizes[i]);

			// nTextureSlots
			cFileUtil::Write(binFile, (uint8_t)shaderTextureSlots[i].size());

			// each texture slots
			for (auto p : shaderTextureSlots[i]) {
				// Slot name length
				cFileUtil::Write(binFile, (uint16_t)p.first.size());

				// Slot name
				cFileUtil::Write(binFile, p.first.c_str());

				// Slot index
				cFileUtil::Write(binFile, p.second);
			}
			binFile.close();
		}
	}

	HRESULT hr = S_OK;
	if (byteCodeSizes[VS] != 0) {
		// Create VERTEX_SHADER from byteCode
		hr = d3ddev->CreateVertexShader(byteCodes[VS], byteCodeSizes[VS], NULL, &vs);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create vertex shader from bytecode: " + binPaths[VS]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[HS] != 0) {
		// Create HULL_SHADER from byteCode
		hr = d3ddev->CreateHullShader(byteCodes[HS], byteCodeSizes[HS], NULL, &hs);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create hull shader shader from bytecode: " + binPaths[HS]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[DS] != 0) {
		// Create DOMAIN_SHADER from byteCode
		hr = d3ddev->CreateDomainShader(byteCodes[DS], byteCodeSizes[DS], NULL, &ds);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create domain shader from bytecode: " + binPaths[DS]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[GS] != 0) {
		// Create GEOMETRY_SHADER from byteCode
		hr = d3ddev->CreateGeometryShader(byteCodes[GS], byteCodeSizes[GS], NULL, &gs);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create geometry shader from bytecode: " + binPaths[GS]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[PS] != 0) {
		// Create PIXEL_SHADER from byteCode
		hr = d3ddev->CreatePixelShader(byteCodes[PS], byteCodeSizes[PS], NULL, &ps);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create pixel shader from bytecode: " + binPaths[PS]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}

	// Get Samplers from cg
	std::ifstream cgFile(shaderPath.c_str());
	auto cgFileLines = cFileUtil::GetLines(cgFile);

	std::unordered_map<zsString, tSamplerDesc> samplerPairs = cgHelper.GetSamplerStates(cgFileLines);

	for (auto it = samplerPairs.begin(); it != samplerPairs.end(); it++) {
		D3D11_SAMPLER_DESC sDesc = ConvertToNativeSampler(it->second);

		size_t i = 0;
		for (; i < samplerStates.size(); i++) {
			if (memcmp(&sDesc, &samplerStates[i].desc, sizeof(D3D11_SAMPLER_DESC) == 0))
				break;
		}

		// Not found that sampler..
		if (i == samplerStates.size()) {
			ID3D11SamplerState* state = NULL;
			HRESULT hr = d3ddev->CreateSamplerState(&sDesc, &state);
			if (FAILED(hr)) {
				lastErrorMsg = L"Can't create SamplerState";
				return eGapiResult::ERROR_UNKNOWN;
			}

			tSamplerInfo info;
				info.desc = sDesc;
				info.state = state;
			samplerStates.push_back(info);
		}

		// Iterate through all samplers and check whether sampler exists in that domain, if it, add to specfic sampler domain
		for (size_t j = 0; j < nDomains; j++) {
			std::unordered_map<zsString, uint16_t> textureSlotsPerDomain = shaderTextureSlots[j];

			// sampler found in domain
			auto texIt = textureSlotsPerDomain.find(it->first);
			if (texIt != textureSlotsPerDomain.end()) {
				shaderSamplerStates[j].push_back(cShaderProgramD3D11::tSamplerInfo(i, texIt->second));
			}
		}
	}

	// Parse input Layout... from VERTEX_SHADER
	// - 1. search for vertexShader Entry name ex:"VS_MAIN(", get return value, for example VS_OUT
	// - 2. search for VS_OUT, get lines under that, while line != "};"
	// - 3. extract VERTEX DECLARATION from those lines

	zsString vsInStructName = cStrUtil::GetWordAfter(cgFileLines, entryNames[VS] + L"(");
	std::list<zsString> vsInStructLines = cStrUtil::GetLinesBetween(cgFileLines, vsInStructName, L"};");
	cgFile.close();

	int nVertexAttributes = 0;

	// Count vertexAttributes
	auto iter = vsInStructLines.begin();
	while (iter != vsInStructLines.end()) {
		// not empty line... Parse Vertex Declaration
		if (iter->size() != 0)
			nVertexAttributes++;

		iter++;
	}

	// (inputLayout)  Vertex shader input format
	std::vector<cVertexFormat::VertexAttrib> attribs;
	cVertexFormat::VertexAttrib tmpAttrib;
	uint16_t attribIdx = 0;
	iter = vsInStructLines.begin();
	while (iter != vsInStructLines.end()) {
		// not empty line... Parse Vertex Declaration
		if (iter->size() != 0) {
			char semanticNames[10][32]; // Max 10 semantic, each 32 word length
			char semanticIndex[3]; // 999 max

			cStrUtil::GetWordBetween(*iter, ':', ';', semanticNames[attribIdx]);
			cStrUtil::GetNumberFromEnd(semanticNames[attribIdx], semanticIndex);
			cStrUtil::CutNumberFromEnd(semanticNames[attribIdx]);

			if (strcmp(semanticNames[attribIdx], "POSITION") == 0)
				tmpAttrib.semantic = cVertexFormat::POSITION;
			else if (strcmp(semanticNames[attribIdx], "NORMAL") == 0)
				tmpAttrib.semantic = cVertexFormat::NORMAL;
			else if (strcmp(semanticNames[attribIdx], "COLOR") == 0)
				tmpAttrib.semantic = cVertexFormat::COLOR;
			else if (strcmp(semanticNames[attribIdx], "TEXCOORD") == 0)
				tmpAttrib.semantic = cVertexFormat::TEXCOORD;
			else
				ASSERT_MSG(false, L"Cg file parsing : " + shaderPath + L", Failed to match shader semantic name with our cVertexFormat class");

			
			if (iter->find(L"float4") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 4;
				tmpAttrib.type = cVertexFormat::FLOAT;
			}
			else if (iter->find(L"float3") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 3;
				tmpAttrib.type = cVertexFormat::FLOAT;
			}
			else if (iter->find(L"float2") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 2;
				tmpAttrib.type = cVertexFormat::FLOAT;
			}
			else if (iter->find(L"float") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 1;
				tmpAttrib.type = cVertexFormat::FLOAT; 
			}
			else
				ASSERT_MSG(false, L"Cg file parsing : " + shaderPath + L", Failed to determine vertex shader input format type (float1, 2, 3, 4) supported lol");

			attribs.push_back(tmpAttrib);
		}

		attribIdx++;
		iter++;
	}

	// Vertex Shader input format
	cVertexFormat vsInputFormat;
	vsInputFormat.Create(attribs);

	// Create shader program
	cShaderProgramD3D11* shProg = new cShaderProgramD3D11(byteCodes[VS], byteCodeSizes[VS], vsInputFormat, vs, hs, ds, gs, ps);

	// Set look up maps
	shProg->SetTextureSlotsVS(shaderTextureSlots[VS]);
	shProg->SetTextureSlotsPS(shaderTextureSlots[PS]);

	shProg->SetSamplerStatesVS(shaderSamplerStates[VS]);
	shProg->SetSamplerStatesPS(shaderSamplerStates[PS]);

	// FREE UP
	for (size_t i = 0; i < nDomains; i++)
		SAFE_RELEASE(blobs[i]);

	// return and tell everyone dat success
	*resource = shProg;
	return eGapiResult::OK;
}


//	Write to various resources
eGapiResult cGraphicsApiD3D11::WriteResource(IIndexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ASSERT(buffer != NULL);

	if (buffer->GetSize() < size + offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes);
	if (hr != S_OK)
		return eGapiResult::ERROR_INVALID_ARG;

	memcpy((void*)(size_t(mappedRes.pData) + offset), source, size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::WriteResource(IVertexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ASSERT(buffer != NULL);

	if (buffer->GetSize() < size + offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes);
	if (hr != S_OK)
		return eGapiResult::ERROR_INVALID_ARG;

	memcpy((void*)(size_t(mappedRes.pData) + offset), source, size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

// Read from various resources
eGapiResult cGraphicsApiD3D11::ReadResource(IIndexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	ASSERT(buffer != NULL);

	if (buffer->GetSize() < size + offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr != S_OK)
		return eGapiResult::ERROR_INVALID_ARG;


	memcpy(dest, (void*)(size_t(mappedRes.pData) + offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::ReadResource(IVertexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	ASSERT(buffer != NULL);

	if (buffer->GetSize() < size + offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr != S_OK) {
		return eGapiResult::ERROR_INVALID_ARG;
	}

	memcpy(dest, (void*)(size_t(mappedRes.pData) + offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::ReadResource(ITexture2D* texture, void* dest, size_t size, size_t offset /*= 0*/) {
	ASSERT(texture != NULL);

	// TODO OUT OF RANGE CHECK
	//if (buffer->GetSize() < size + offset)
	//return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr = S_OK;
	ID3D11Texture2D* d3dBuffer = ((cTexture2DD3D11*)texture)->Get();
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr != S_OK) {
		return eGapiResult::ERROR_INVALID_ARG;
	}

	memcpy(dest, (void*)(size_t(mappedRes.pData) + offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

// Copy among resources
eGapiResult cGraphicsApiD3D11::CopyResource(ITexture2D* src, ITexture2D* dst) {
	ASSERT(src != NULL && dst != NULL);

	d3dcon->CopyResource((ID3D11Resource*)((cTexture2DD3D11*)dst)->Get(), (ID3D11Resource*)((cTexture2DD3D11*)src)->Get());
	return eGapiResult::OK;
}




////////////////////////////////////////////////////////////////////////////////
//	Draw and rendering

// Clear render-target
void cGraphicsApiD3D11::Clear(bool target /*= true*/, bool depth /*= false*/, bool stencil /*= false*/, const Vec4& clearColor /*= Vec4(0.3f, 0.3f, 0.3f, 1.0f)*/) {

	// Setup clear flags
	UINT clearFlags = 0;
	if (depth)
		clearFlags |= D3D11_CLEAR_DEPTH;
	if (stencil)
		clearFlags |= D3D11_CLEAR_STENCIL;

	// Clear depth, stencil if needed
	if (depth || stencil)
		d3dcon->ClearDepthStencilView((ID3D11DepthStencilView*)defaultRenderTarget->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Clear BackBuffer
	if (target)
		d3dcon->ClearRenderTargetView((ID3D11RenderTargetView*)defaultRenderTarget->GetRTV(), (FLOAT*)&clearColor);
}

// Clear texture
void cGraphicsApiD3D11::ClearTexture(ITexture2D* t, unsigned clearFlag /*= 0*/, const Vec4& clearColor /*= Vec4()*/, float depthVal /*= 1.0f*/, size_t stencilVal /*= 0*/) {
	ASSERT(t != NULL);
	
	ID3D11DepthStencilView* dsv = ((cTexture2DD3D11*)t)->GetDSV();
	if (dsv)
		d3dcon->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthVal, (UINT8)stencilVal);

	ID3D11RenderTargetView* rtv = ((cTexture2DD3D11*)t)->GetRTV();
	if (rtv)
		d3dcon->ClearRenderTargetView(rtv, (FLOAT*)&clearColor);
}

// Present
void cGraphicsApiD3D11::Present() {
	if (d3dsc){
		d3dsc->Present(0, 0);
	}
}


// Draw functions
void cGraphicsApiD3D11::Draw(size_t nVertices, size_t idxStartVertex /*= 0*/) {
	ApplySamplerStates();
	ApplyConstantBuffers();
	d3dcon->Draw(nVertices, idxStartVertex);
}

void cGraphicsApiD3D11::DrawIndexed(size_t nIndices, size_t idxStartIndex /*= 0*/) {
	ApplySamplerStates();
	ApplyConstantBuffers();
	d3dcon->DrawIndexed(nIndices, idxStartIndex, 0);
}

void cGraphicsApiD3D11::DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	ApplySamplerStates();
	ApplyConstantBuffers();
	d3dcon->DrawInstanced(nVerticesPerInstance, nInstances, idxStartVertex, idxStartInstance);
}

void cGraphicsApiD3D11::DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	ApplySamplerStates();
	ApplyConstantBuffers();
	d3dcon->DrawIndexedInstanced(nIndicesPerInstance, nInstances, idxStartIndex, 0, idxStartInstance);
}


////////////////////////////////////////////////////////////////////////////////
//	Pipeline state

// Set indexed poly-mesh buffers
void cGraphicsApiD3D11::SetVertexBuffer(const IVertexBuffer* vb) {
	ASSERT(vb != NULL);
	activeVertexBuffer = (cVertexBufferD3D11*)vb;

	const UINT strides = vb->GetStride();
	const UINT offset = 0;
	ID3D11Buffer* vertices = ((cVertexBufferD3D11*)vb)->GetBufferPointer();
	d3dcon->IASetVertexBuffers(0, 1, &vertices, &strides, &offset);
	AutoSetInputLayout(activeShaderProg, activeVertexBuffer);
}
void cGraphicsApiD3D11::SetIndexBuffer(const IIndexBuffer* ib) {
	ASSERT(ib != NULL);

	d3dcon->IASetIndexBuffer(((cIndexBufferD3D11*)ib)->GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0);
}

// Set instance data for instanced rendering
void cGraphicsApiD3D11::SetInstanceData() {
}

// Set shader texture resource
eGapiResult cGraphicsApiD3D11::SetTexture(const ITexture2D* t, size_t slotIdx) {
	ASSERT(t != NULL);

	const ID3D11ShaderResourceView* srv = ((cTexture2DD3D11*)t)->GetSRV();
	if (srv != NULL) {
		d3dcon->PSSetShaderResources(slotIdx, 1, (ID3D11ShaderResourceView**)&srv);
		return eGapiResult::OK;
	} else {
		return eGapiResult::ERROR_INVALID_ARG;
	}
}

// Set shader texture resource
eGapiResult cGraphicsApiD3D11::SetTexture(const wchar_t* varName, const ITexture2D* t) {
	ASSERT(t != NULL);

	const ID3D11ShaderResourceView* srv = ((cTexture2DD3D11*)t)->GetSRV();
	if (srv != NULL) {
		int slot = ((cShaderProgramD3D11*)activeShaderProg)->GetTextureSlotVS(varName);
		if (slot < 0) {
			slot = ((cShaderProgramD3D11*)activeShaderProg)->GetTextureSlotPS(varName);
			if (slot < 0)
				return eGapiResult::ERROR_INVALID_ARG;

			// PS texture found
			d3dcon->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&srv);
			return eGapiResult::OK;
		}
		
		// VS texture found
		d3dcon->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)&srv);
		return eGapiResult::OK;
	} 
	else
		return eGapiResult::ERROR_INVALID_ARG;
}

// Set compiled-linked shader program
void cGraphicsApiD3D11::SetShaderProgram(IShaderProgram* shProg) {
	ASSERT(shProg != NULL);

	activeShaderProg = (cShaderProgramD3D11*)shProg;
	const cShaderProgramD3D11* shProgD3D11 = (cShaderProgramD3D11*)shProg;

	AutoSetInputLayout(activeShaderProg, activeVertexBuffer);

	d3dcon->VSSetShader(const_cast<ID3D11VertexShader*>(shProgD3D11->GetVS()), 0, 0);
	d3dcon->PSSetShader(const_cast<ID3D11PixelShader*>(shProgD3D11->GetPS()), 0, 0);
}

// Set primitive topology
void cGraphicsApiD3D11::SetPrimitiveTopology(ePrimitiveTopology t) {
	switch (t) {
	case ePrimitiveTopology::LINE_LIST:
		d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case ePrimitiveTopology::TRIANGLE_LIST:
		d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	}
}

eGapiResult cGraphicsApiD3D11::SetVSConstantBuffer(const void* data, size_t size, size_t slotIdx) {
	vsConstBufferStateChanged = true;

	// 16 means one register byte size... (vec4)
	size_t dstByteOffset = slotIdx * 16;  // slotIdx * sizeof(float) * 4
	// Need resize for constant buffer
	if (vsConstBufferSize < dstByteOffset + size)
	{
		// Save old size
		size_t oldSize = vsConstBufferSize;

		// Determine new size
		// Not multiple of 16, for ex. 24, then size + 16 - (size % 16) = 32
		vsConstBufferSize = dstByteOffset + size;
		if (((dstByteOffset + size) % 16) != 0)
			vsConstBufferSize = vsConstBufferSize + 16 - (vsConstBufferSize % 16);

		// Resize our void*
		Realloc(vsConstBufferData, oldSize, vsConstBufferSize);

		// Recreate constant buffer
		SAFE_RELEASE(vsConstBuffer);
		D3D11_BUFFER_DESC desc;
			desc.ByteWidth = vsConstBufferSize;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HRESULT hr = d3ddev->CreateBuffer(&desc, NULL, &vsConstBuffer);
		if (FAILED(hr)) {
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		}
		d3dcon->VSSetConstantBuffers(0, 1, &vsConstBuffer); // Set Buffer
	}
	memcpy((unsigned char*)vsConstBufferData + dstByteOffset, data, size);
	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::SetPSConstantBuffer(const void* data, size_t size, size_t slotIdx) {
	psConstBufferStateChanged = true;

	// 16 means one register byte size... (vec4)
	size_t dstByteOffset = slotIdx * 16;  // slotIdx * sizeof(float) * 4
	// Need resize for constant buffer
	if (psConstBufferSize < dstByteOffset + size)
	{
		// Save old size
		size_t oldSize = psConstBufferSize;

		// Determine new size
		psConstBufferSize = dstByteOffset + size;
		if (((dstByteOffset + size) % 16) != 0)
			psConstBufferSize = psConstBufferSize + 16 - (psConstBufferSize % 16);
			
		// Resize our void*
		Realloc(psConstBufferData, oldSize, psConstBufferSize);

		// Recreate constant buffer
		SAFE_RELEASE( psConstBuffer );
		D3D11_BUFFER_DESC desc;
			desc.ByteWidth = psConstBufferSize;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HRESULT hr = d3ddev->CreateBuffer(&desc, NULL, &psConstBuffer);
		if (FAILED(hr)) {
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		}
		d3dcon->PSSetConstantBuffers(0, 1, &psConstBuffer); // Set Buffer
	}
	memcpy((unsigned char*)psConstBufferData + dstByteOffset, data, size);
	return eGapiResult::OK;
}

// Set (multiple) render targets
eGapiResult cGraphicsApiD3D11::SetRenderTargets(unsigned nTargets, const ITexture2D* const* renderTargets, ITexture2D* depthStencilTarget /* = NULL */) {
	for (unsigned i = 0; i < nTargets; i++) {
		activeRTVs[i] = ((const cTexture2DD3D11*)(renderTargets[i]))->GetRTV();
		if (activeRTVs[i]) {
			activeViewports[i].Width = ((const cTexture2DD3D11*)(renderTargets[i]))->GetWidth();
			activeViewports[i].Height = ((const cTexture2DD3D11*)(renderTargets[i]))->GetHeight();
			activeViewports[i].TopLeftX = 0;
			activeViewports[i].TopLeftY = 0;
			activeViewports[i].MinDepth = 0.0f;
			activeViewports[i].MaxDepth = 1.0f;
		}
	}

	// DSV
	ID3D11DepthStencilView* dsv = (depthStencilTarget) ? ((cTexture2DD3D11*)depthStencilTarget)->GetDSV() : NULL;

	// Set Viewports
	d3dcon->RSSetViewports(nTargets, activeViewports);

	// Set RTVS
	d3dcon->OMSetRenderTargets(nTargets, activeRTVs, dsv);
	return eGapiResult::OK;
}

// Resize swap chain
eGapiResult cGraphicsApiD3D11::SetBackBufferSize(unsigned width, unsigned height) {
	// Release default render target (BackBuffer)
	SAFE_DELETE(defaultRenderTarget);

	HRESULT hr = d3dsc->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_EFFECT_DISCARD | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	if (!FAILED(hr)) {
		defaultVP.Width = width;
		defaultVP.Height = height;
		return CreateViewsForBB();
	}
	switch (hr) {
	case S_OK: return eGapiResult::OK;   break;
	case E_OUTOFMEMORY: return eGapiResult::ERROR_OUT_OF_MEMORY; break;
	case E_INVALIDARG:	return eGapiResult::ERROR_INVALID_ARG; break;
	default:
		return eGapiResult::ERROR_UNKNOWN;
	}
}

// Get default render target == backbuffer
ITexture2D* cGraphicsApiD3D11::GetDefaultRenderTarget() const {
	return defaultRenderTarget;
}
// Set backBuffer as render-target
eGapiResult cGraphicsApiD3D11::SetRenderTargetDefault() {
	d3dcon->RSSetViewports(1, &defaultVP);
	ID3D11RenderTargetView* rtv = defaultRenderTarget->GetRTV();
	d3dcon->OMSetRenderTargets(1, &rtv, (ID3D11DepthStencilView*)defaultRenderTarget->GetDSV());
	return eGapiResult::OK;
}

// Set blend state
eGapiResult cGraphicsApiD3D11::SetBlendState(tBlendDesc desc) {
	D3D11_BLEND_DESC bsDesc = ConvertToNativeBlend(desc);

	ID3D11BlendState* state = NULL;

	size_t i = 0;
	for (; i < blendStates.size(); i++)
		if (memcmp(&bsDesc, &blendStates[i].desc, sizeof(D3D11_BLEND_DESC)) == 0)
			break;

	if (i == blendStates.size()) {
		HRESULT hr = d3ddev->CreateBlendState(&bsDesc, &state);
		if (FAILED(hr)) {
			switch (hr) {
			case E_OUTOFMEMORY: return eGapiResult::ERROR_OUT_OF_MEMORY;
			default: return eGapiResult::ERROR_UNKNOWN;
			}
		}

		tBlendInfo info;
			info.desc = bsDesc;
			info.state = state;
		blendStates.push_back(info);

		//SAFE_RELEASE(state);

	} else {
		state = blendStates[i].state;
	}

	d3dcon->OMSetBlendState(state, NULL, 0xFFFFFFFF);
	return eGapiResult::OK;
}

// Set depth-stencil state
eGapiResult cGraphicsApiD3D11::SetDepthStencilState(tDepthStencilDesc desc, uint8_t stencilRef) {
	D3D11_DEPTH_STENCIL_DESC dsDesc = ConvertToNativeDepthStencil(desc);

	size_t i = 0;
	for (; i < depthStencilStates.size(); i++)
		if (memcmp(&dsDesc, &depthStencilStates[i].desc, sizeof(D3D11_DEPTH_STENCIL_DESC)) == 0)
			break;

	ID3D11DepthStencilState* state = NULL;

	// Not existing
	if ( i == depthStencilStates.size()) {
		HRESULT hr = d3ddev->CreateDepthStencilState(&dsDesc, &state);
		if (FAILED(hr)) {
			switch (hr) {
			case E_OUTOFMEMORY: return eGapiResult::ERROR_OUT_OF_MEMORY;
			default: return eGapiResult::ERROR_UNKNOWN;
			}
		}
		tDepthStencilInfo info;
			info.desc = dsDesc;
			info.state = state;
		depthStencilStates.push_back(info);

		//SAFE_RELEASE(state);
	} else {
		state = depthStencilStates[i].state;
	}
	
	d3dcon->OMSetDepthStencilState(state, stencilRef);
	return eGapiResult::OK;
}

// Set Target Window
eGapiResult cGraphicsApiD3D11::SetWindow(IWindow *renderWindow) {
	ASSERT(renderWindow != NULL);

	size_t clientWidth = renderWindow->GetClientWidth();
	size_t clientHeight = renderWindow->GetClientHeight();
	// Same window size : don't need new swap chain
	if (defaultRenderTarget != NULL)
	if (clientWidth == defaultRenderTarget->GetWidth() && clientHeight == defaultRenderTarget->GetHeight())
		return eGapiResult::OK;

	// Create swap chain for device
	eGapiResult r = CreateMostAcceptableSwapChain(clientWidth, clientHeight, (HWND)(renderWindow->GetHandle()));
	if (r != eGapiResult::OK) return r;

	// Create main render target (BackBuffer)
	r = CreateViewsForBB();
	if (r != eGapiResult::OK) return r;

	// Create default viewport for swapChain rendering
	defaultVP.TopLeftX = 0,
		defaultVP.TopLeftY = 0;
	defaultVP.Width = clientWidth;
	defaultVP.Height = clientHeight;
	defaultVP.MaxDepth = 1.0f;
	defaultVP.MinDepth = 0.0f;

	// BackBuffer will be the render target in default
	r = SetRenderTargetDefault();
	return r;
}


////////////////////////////////////////////////////////////////////////////////
// Misc
const wchar_t* cGraphicsApiD3D11::GetLastErrorMsg() const {
	return lastErrorMsg.c_str();
}


////////////////////////////////////////////////////////////////////////////////
// Internal helper functions


////////////////////////////////////////
// Create input layouts
ID3D11InputLayout* cGraphicsApiD3D11::GetInputLayout(cShaderProgramD3D11* shader, cVertexFormat bufferFormat) {
	cVertexFormat shaderFormat = shader->GetVSInputFormat();

	std::pair<cVertexFormat, cVertexFormat> key(shaderFormat, bufferFormat);

	auto it = inputLayoutStore.find(key);
	if (it == inputLayoutStore.end()) {
		// create new input layout
		ID3D11InputLayout* layout = nullptr;

		auto vertexDesc = ConvertToNativeVertexFormat(bufferFormat);

		d3ddev->CreateInputLayout(vertexDesc.data(), vertexDesc.size(), shader->GetVSByteCode(), shader->GetVSByteCodeSize(), &layout);

		// add input layout to stuff
		if (layout == nullptr) {
			return nullptr;
		}
		inputLayoutStore.insert(InputLayoutMapT::value_type(key, layout));
		return layout;
	}
	else {
		return it->second;
	}
}

// Set said input layout
void cGraphicsApiD3D11::AutoSetInputLayout(cShaderProgramD3D11* shader, cVertexBufferD3D11* buffer) {
	if (buffer == nullptr || shader == nullptr) {
		return;
	}

	auto bufferFormat = buffer->GetFormat();

	auto inputLayout = GetInputLayout(shader, bufferFormat);
	if (inputLayout) {
		d3dcon->IASetInputLayout(inputLayout);
	}
	else {
#pragma message("WATTAFAKK LESZ MOST???")
	}
}

////////////////////////////////////////
// Convert stuff to native format
#include <unordered_map>

// vertex decl
std::vector<D3D11_INPUT_ELEMENT_DESC> ConvertToNativeVertexFormat(cVertexFormat format) {
	auto elems = format.Decode();
	std::vector<D3D11_INPUT_ELEMENT_DESC> ret;

	unsigned pos = 0, color = 0, normal = 0, tex = 0;

	for (auto& v : elems) {
		D3D11_INPUT_ELEMENT_DESC desc;

		// semantic and semantic index
		switch (v.semantic) {
			case cVertexFormat::POSITION:
				desc.SemanticName =  "POSITION";
				desc.SemanticIndex = pos;
				pos++;
				break;
			case cVertexFormat::NORMAL:
				desc.SemanticName = "NORMAL";
				desc.SemanticIndex = normal;
				normal++;
				break;
			case cVertexFormat::TEXCOORD:
				desc.SemanticName = "TEXCOORD";
				desc.SemanticIndex = tex;
				tex++;
				break;
			// default is color
			default:
				desc.SemanticName = "COLOR";
				desc.SemanticIndex = color;
				color++;
				break;
		}

		desc.InputSlot = 0;
		desc.AlignedByteOffset = 0;
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;

		// format
		desc.Format = [&]() ->DXGI_FORMAT {
			if (v.nComponents == 1) {
				if (v.bitsPerComponent == cVertexFormat::_8_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R8_SINT;
					else if (v.type == cVertexFormat::NORM)
						return DXGI_FORMAT_R8_SNORM;
				}
				if (v.bitsPerComponent == cVertexFormat::_16_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R16_SINT;
					else if (v.type == cVertexFormat::NORM)
						return DXGI_FORMAT_R16_SNORM;
					else if (v.type == cVertexFormat::FLOAT)
						return DXGI_FORMAT_R16_FLOAT;
				}
				if (v.bitsPerComponent == cVertexFormat::_32_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R32_SINT;
					else if (v.type == cVertexFormat::FLOAT)
						return DXGI_FORMAT_R32_FLOAT;
				}
			}
			else if (v.nComponents == 2) {
				if (v.bitsPerComponent == cVertexFormat::_8_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R8G8_SINT;
					else if (v.type == cVertexFormat::NORM)
						return DXGI_FORMAT_R8G8_SNORM;
				}
				if (v.bitsPerComponent == cVertexFormat::_16_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R16G16_SINT;
					else if (v.type == cVertexFormat::NORM)
						return DXGI_FORMAT_R16G16_SNORM;
					else if (v.type == cVertexFormat::FLOAT)
						return DXGI_FORMAT_R16G16_FLOAT;
				}
				if (v.bitsPerComponent == cVertexFormat::_32_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R32G32_SINT;
					else if (v.type == cVertexFormat::FLOAT)
						return DXGI_FORMAT_R32G32_FLOAT;
				}
			}
			else if (v.nComponents == 3) {
				if (v.bitsPerComponent == cVertexFormat::_32_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R32G32B32_SINT;
					else if (v.type == cVertexFormat::FLOAT)
						return DXGI_FORMAT_R32G32B32_FLOAT;
				}
			}
			else if (v.nComponents == 4) {
				if (v.bitsPerComponent == cVertexFormat::_8_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R8G8B8A8_SINT;
					else if (v.type == cVertexFormat::NORM)
						return DXGI_FORMAT_R8G8B8A8_SNORM;
				}
				if (v.bitsPerComponent == cVertexFormat::_16_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R16G16B16A16_SINT;
					else if (v.type == cVertexFormat::NORM)
						return DXGI_FORMAT_R16G16B16A16_SNORM;
					else if (v.type == cVertexFormat::FLOAT)
						return DXGI_FORMAT_R16G16B16A16_FLOAT;
				}
				if (v.bitsPerComponent == cVertexFormat::_32_BIT) {
					if (v.type == cVertexFormat::INT)
						return DXGI_FORMAT_R32G32B32A32_SINT;
					else if (v.type == cVertexFormat::FLOAT)
						return DXGI_FORMAT_R32G32B32A32_FLOAT;
				}
			}
			return DXGI_FORMAT_UNKNOWN;
		}();
		
		ret.push_back(desc);
	}
	return ret;
}

// bind
unsigned ConvertToNativeBind(unsigned flags) {
	unsigned ret = 0;
	ret |= ((flags&(unsigned)eBind::CONSTANT_BUFFER) != 0 ? D3D11_BIND_CONSTANT_BUFFER : 0);
	ret |= ((flags&(unsigned)eBind::INDEX_BUFFER) != 0 ? D3D11_BIND_INDEX_BUFFER : 0);
	ret |= ((flags&(unsigned)eBind::DEPTH_STENCIL) != 0 ? D3D11_BIND_DEPTH_STENCIL : 0);
	ret |= ((flags&(unsigned)eBind::RENDER_TARGET) != 0 ? D3D11_BIND_RENDER_TARGET : 0);
	ret |= ((flags&(unsigned)eBind::SHADER_RESOURCE) != 0 ? D3D11_BIND_SHADER_RESOURCE : 0);
	ret |= ((flags&(unsigned)eBind::VERTEX_BUFFER) != 0 ? D3D11_BIND_VERTEX_BUFFER : 0);

	return ret;
}

// usage
D3D11_USAGE ConvertToNativeUsage(eUsage usage) {
	static const std::unordered_map<eUsage, D3D11_USAGE> lookupTable = {
		{ eUsage::DEFAULT, D3D11_USAGE_DEFAULT },
		{ eUsage::DYNAMIC, D3D11_USAGE_DYNAMIC },
		{ eUsage::IMMUTABLE, D3D11_USAGE_IMMUTABLE },
		{ eUsage::STAGING, D3D11_USAGE_STAGING },
	};
	auto it = lookupTable.find(usage);
	assert(it != lookupTable.end());
	return it->second;
}

// format
DXGI_FORMAT ConvertToNativeFormat(eFormat fmt) {
	static const std::unordered_map<eFormat, DXGI_FORMAT> lookupTable = {
		{ eFormat::UNKNOWN, DXGI_FORMAT_UNKNOWN },
		{ eFormat::R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS },
		{ eFormat::R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ eFormat::R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_UINT },
		{ eFormat::R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_SINT },
		{ eFormat::R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_TYPELESS },
		{ eFormat::R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT },
		{ eFormat::R32G32B32_UINT, DXGI_FORMAT_R32G32B32_UINT },
		{ eFormat::R32G32B32_SINT, DXGI_FORMAT_R32G32B32_SINT },
		{ eFormat::R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_TYPELESS },
		{ eFormat::R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ eFormat::R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM },
		{ eFormat::R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT },
		{ eFormat::R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM },
		{ eFormat::R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT },
		{ eFormat::R32G32_TYPELESS, DXGI_FORMAT_R32G32_TYPELESS },
		{ eFormat::R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT },
		{ eFormat::R32G32_UINT, DXGI_FORMAT_R32G32_UINT },
		{ eFormat::R32G32_SINT, DXGI_FORMAT_R32G32_SINT },
		{ eFormat::R32G8X24_TYPELESS, DXGI_FORMAT_R32G8X24_TYPELESS },
		{ eFormat::D32_FLOAT_S8X24_UINT, DXGI_FORMAT_D32_FLOAT_S8X24_UINT },
		{ eFormat::R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS },
		{ eFormat::X32_TYPELESS_G8X24_UINT, DXGI_FORMAT_X32_TYPELESS_G8X24_UINT },
		{ eFormat::R10G10B10A2_TYPELESS, DXGI_FORMAT_R10G10B10A2_TYPELESS },
		{ eFormat::R10G10B10A2_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM },
		{ eFormat::R10G10B10A2_UINT, DXGI_FORMAT_R10G10B10A2_UINT },
		{ eFormat::R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT },
		{ eFormat::R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_TYPELESS },
		{ eFormat::R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM },
		{ eFormat::R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
		{ eFormat::R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_UINT },
		{ eFormat::R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM },
		{ eFormat::R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_SINT },
		{ eFormat::R16G16_TYPELESS, DXGI_FORMAT_R16G16_TYPELESS },
		{ eFormat::R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT },
		{ eFormat::R16G16_UNORM, DXGI_FORMAT_R16G16_UNORM },
		{ eFormat::R16G16_UINT, DXGI_FORMAT_R16G16_UINT },
		{ eFormat::R16G16_SNORM, DXGI_FORMAT_R16G16_SNORM },
		{ eFormat::R16G16_SINT, DXGI_FORMAT_R16G16_SINT },
		{ eFormat::R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS },
		{ eFormat::D32_FLOAT, DXGI_FORMAT_D32_FLOAT },
		{ eFormat::R32_FLOAT, DXGI_FORMAT_R32_FLOAT },
		{ eFormat::R32_UINT, DXGI_FORMAT_R32_UINT },
		{ eFormat::R32_SINT, DXGI_FORMAT_R32_SINT },
		{ eFormat::R24G8_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS },
		{ eFormat::D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT },
		{ eFormat::R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
		{ eFormat::X24_TYPELESS_G8_UINT, DXGI_FORMAT_X24_TYPELESS_G8_UINT },
		{ eFormat::R8G8_TYPELESS, DXGI_FORMAT_R8G8_TYPELESS },
		{ eFormat::R8G8_UNORM, DXGI_FORMAT_R8G8_UNORM },
		{ eFormat::R8G8_UINT, DXGI_FORMAT_R8G8_UINT },
		{ eFormat::R8G8_SNORM, DXGI_FORMAT_R8G8_SNORM },
		{ eFormat::R8G8_SINT, DXGI_FORMAT_R8G8_SINT },
		{ eFormat::R16_TYPELESS, DXGI_FORMAT_R16_TYPELESS },
		{ eFormat::R16_FLOAT, DXGI_FORMAT_R16_FLOAT },
		{ eFormat::D16_UNORM, DXGI_FORMAT_D16_UNORM },
		{ eFormat::R16_UNORM, DXGI_FORMAT_R16_UNORM },
		{ eFormat::R16_UINT, DXGI_FORMAT_R16_UINT },
		{ eFormat::R16_SNORM, DXGI_FORMAT_R16_SNORM },
		{ eFormat::R16_SINT, DXGI_FORMAT_R16_SINT },
		{ eFormat::R8_TYPELESS, DXGI_FORMAT_R8_TYPELESS },
		{ eFormat::R8_UNORM, DXGI_FORMAT_R8_UNORM },
		{ eFormat::R8_UINT, DXGI_FORMAT_R8_UINT },
		{ eFormat::R8_SNORM, DXGI_FORMAT_R8_SNORM },
		{ eFormat::R8_SINT, DXGI_FORMAT_R8_SINT },
		{ eFormat::A8_UNORM, DXGI_FORMAT_A8_UNORM },
		{ eFormat::R1_UNORM, DXGI_FORMAT_R1_UNORM },
		{ eFormat::R9G9B9E5_SHAREDEXP, DXGI_FORMAT_R9G9B9E5_SHAREDEXP },
		{ eFormat::R8G8_B8G8_UNORM, DXGI_FORMAT_R8G8_B8G8_UNORM },
		{ eFormat::G8R8_G8B8_UNORM, DXGI_FORMAT_G8R8_G8B8_UNORM },
		{ eFormat::BC1_TYPELESS, DXGI_FORMAT_BC1_TYPELESS },
		{ eFormat::BC1_UNORM, DXGI_FORMAT_BC1_UNORM },
		{ eFormat::BC1_UNORM_SRGB, DXGI_FORMAT_BC1_UNORM_SRGB },
		{ eFormat::BC2_TYPELESS, DXGI_FORMAT_BC2_TYPELESS },
		{ eFormat::BC2_UNORM, DXGI_FORMAT_BC2_UNORM },
		{ eFormat::BC2_UNORM_SRGB, DXGI_FORMAT_BC2_UNORM_SRGB },
		{ eFormat::BC3_TYPELESS, DXGI_FORMAT_BC3_TYPELESS },
		{ eFormat::BC3_UNORM, DXGI_FORMAT_BC3_UNORM },
		{ eFormat::BC3_UNORM_SRGB, DXGI_FORMAT_BC3_UNORM_SRGB },
		{ eFormat::BC4_TYPELESS, DXGI_FORMAT_BC4_TYPELESS },
		{ eFormat::BC4_UNORM, DXGI_FORMAT_BC4_UNORM },
		{ eFormat::BC4_SNORM, DXGI_FORMAT_BC4_SNORM },
		{ eFormat::BC5_TYPELESS, DXGI_FORMAT_BC5_TYPELESS },
		{ eFormat::BC5_UNORM, DXGI_FORMAT_BC5_UNORM },
		{ eFormat::BC5_SNORM, DXGI_FORMAT_BC5_SNORM },
		{ eFormat::B5G6R5_UNORM, DXGI_FORMAT_B5G6R5_UNORM },
		{ eFormat::B5G5R5A1_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM },
		{ eFormat::B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM },
		{ eFormat::B8G8R8X8_UNORM, DXGI_FORMAT_B8G8R8X8_UNORM },
		{ eFormat::R10G10B10_XR_BIAS_A2_UNORM, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
		{ eFormat::B8G8R8A8_TYPELESS, DXGI_FORMAT_B8G8R8A8_TYPELESS },
		{ eFormat::B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },
		{ eFormat::B8G8R8X8_TYPELESS, DXGI_FORMAT_B8G8R8X8_TYPELESS },
		{ eFormat::B8G8R8X8_UNORM_SRGB, DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },
		{ eFormat::BC6H_TYPELESS, DXGI_FORMAT_BC6H_TYPELESS },
		{ eFormat::BC6H_UF16, DXGI_FORMAT_BC6H_UF16 },
		{ eFormat::BC6H_SF16, DXGI_FORMAT_BC6H_SF16 },
		{ eFormat::BC7_TYPELESS, DXGI_FORMAT_BC7_TYPELESS },
		{ eFormat::BC7_UNORM, DXGI_FORMAT_BC7_UNORM },
		{ eFormat::BC7_UNORM_SRGB, DXGI_FORMAT_BC7_UNORM_SRGB },
		{ eFormat::AYUV, DXGI_FORMAT_AYUV },
		{ eFormat::Y410, DXGI_FORMAT_Y410 },
		{ eFormat::Y416, DXGI_FORMAT_Y416 },
		{ eFormat::NV12, DXGI_FORMAT_NV12 },
		{ eFormat::P010, DXGI_FORMAT_P010 },
		{ eFormat::P016, DXGI_FORMAT_P016 },
		{ eFormat::_420_OPAQUE, DXGI_FORMAT_420_OPAQUE },
		{ eFormat::YUY2, DXGI_FORMAT_YUY2 },
		{ eFormat::Y210, DXGI_FORMAT_Y210 },
		{ eFormat::Y216, DXGI_FORMAT_Y216 },
		{ eFormat::NV11, DXGI_FORMAT_NV11 },
		{ eFormat::AI44, DXGI_FORMAT_AI44 },
		{ eFormat::IA44, DXGI_FORMAT_IA44 },
		{ eFormat::P8, DXGI_FORMAT_P8 },
		{ eFormat::A8P8, DXGI_FORMAT_A8P8 },
		{ eFormat::B4G4R4A4_UNORM, DXGI_FORMAT_B4G4R4A4_UNORM },
	};
	auto it = lookupTable.find(fmt);
	assert(it != lookupTable.end());
	return it->second;
}


// blend to native
D3D11_BLEND_OP ConvertToNativeBlendOp(eBlendOp blendOp) {
	switch (blendOp) {
	case eBlendOp::ADD: return D3D11_BLEND_OP_ADD;
	case eBlendOp::SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
	case eBlendOp::REV_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
	case eBlendOp::MAX: return D3D11_BLEND_OP_MAX;
	case eBlendOp::MIN: return D3D11_BLEND_OP_MIN;
	default: return D3D11_BLEND_OP_ADD;
	}
}
D3D11_BLEND ConvertToNativeBlendFactor(eBlendFactor blendFactor) {
	switch (blendFactor) {
	case eBlendFactor::ZERO:				return D3D11_BLEND_ZERO;
	case eBlendFactor::ONE:					return D3D11_BLEND_ONE;
	case eBlendFactor::SRC_COLOR:			return D3D11_BLEND_SRC_COLOR;
	case eBlendFactor::INV_SRC_COLOR:		return D3D11_BLEND_INV_SRC_COLOR;
	case eBlendFactor::SRC_ALPHA:			return D3D11_BLEND_SRC_ALPHA;
	case eBlendFactor::INV_SRC_ALPHA:		return D3D11_BLEND_INV_SRC_ALPHA;
	case eBlendFactor::DEST_ALPHA:			return D3D11_BLEND_DEST_ALPHA;
	case eBlendFactor::INV_DEST_ALPHA:		return D3D11_BLEND_INV_DEST_ALPHA;
	case eBlendFactor::DEST_COLOR:			return D3D11_BLEND_DEST_COLOR;
	case eBlendFactor::INV_DEST_COLOR:		return D3D11_BLEND_INV_DEST_COLOR;
	case eBlendFactor::SRC_ALPHA_SAT:		return D3D11_BLEND_SRC_ALPHA_SAT;
	case eBlendFactor::BLEND_FACTOR:		return D3D11_BLEND_BLEND_FACTOR;
	case eBlendFactor::INV_BLEND_FACTOR:	return D3D11_BLEND_INV_BLEND_FACTOR;
	case eBlendFactor::SRC1_COLOR:			return D3D11_BLEND_SRC1_COLOR;
	case eBlendFactor::INV_SRC1_COLOR:		return D3D11_BLEND_INV_SRC1_COLOR;
	case eBlendFactor::SRC1_ALPHA:			return D3D11_BLEND_SRC1_ALPHA;
	case eBlendFactor::INV_SRC1_ALPHA:		return D3D11_BLEND_INV_SRC1_ALPHA;
	default:								return D3D11_BLEND_ONE;
	}
}
uint8_t ConvertToNativeBlendMask(uint8_t blendMask) {
	uint8_t ret = 0u;
	if (blendMask & (uint8_t)eBlendWriteMask::ALPHA)
		ret |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	if (blendMask & (uint8_t)eBlendWriteMask::RED)
		ret |= D3D11_COLOR_WRITE_ENABLE_RED;
	if (blendMask & (uint8_t)eBlendWriteMask::GREEN)
		ret |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	if (blendMask & (uint8_t)eBlendWriteMask::BLUE)
		ret |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	if (blendMask & (uint8_t)eBlendWriteMask::ALL)
		ret |= D3D11_COLOR_WRITE_ENABLE_ALL;

	return ret;
}
D3D11_BLEND_DESC ConvertToNativeBlend(tBlendDesc blend) {
	D3D11_BLEND_DESC ret;
	ret.AlphaToCoverageEnable = (blend.alphaToCoverageEnable ? TRUE : FALSE);
	ret.IndependentBlendEnable = (blend.independentBlendEnable ? TRUE : FALSE);
	for (int i = 0; i < 8; i++) {
		ret.RenderTarget[i].BlendEnable = blend[i].enable;
		ret.RenderTarget[i].BlendOp = ConvertToNativeBlendOp(blend[i].blendOp);
		ret.RenderTarget[i].BlendOpAlpha = ConvertToNativeBlendOp(blend[i].blendOpAlpha);
		ret.RenderTarget[i].DestBlend = ConvertToNativeBlendFactor(blend[i].destBlend);
		ret.RenderTarget[i].DestBlendAlpha = ConvertToNativeBlendFactor(blend[i].destBlendAlpha);
		ret.RenderTarget[i].RenderTargetWriteMask = ConvertToNativeBlendMask(blend[i].writeMask);
		ret.RenderTarget[i].SrcBlend = ConvertToNativeBlendFactor(blend[i].srcBlend);
		ret.RenderTarget[i].SrcBlendAlpha = ConvertToNativeBlendFactor(blend[i].srcBlendAlpha);
	}

	return ret;
}

// depth-stencil to native
D3D11_COMPARISON_FUNC ConvertToNativeCompFunc(eComparisonFunc compFunc) {
	switch (compFunc) {
	case eComparisonFunc::ALWAYS:		return D3D11_COMPARISON_ALWAYS;
	case eComparisonFunc::EQUAL:		return D3D11_COMPARISON_EQUAL;
	case eComparisonFunc::GREATER:		return D3D11_COMPARISON_GREATER;
	case eComparisonFunc::GREATER_EQUAL:return D3D11_COMPARISON_GREATER_EQUAL;
	case eComparisonFunc::LESS:			return D3D11_COMPARISON_LESS;
	case eComparisonFunc::LESS_EQUAL:	return D3D11_COMPARISON_LESS_EQUAL;
	case eComparisonFunc::NEVER:		return D3D11_COMPARISON_NEVER;
	case eComparisonFunc::NOT_EQUAL:	return D3D11_COMPARISON_NOT_EQUAL;
	default: return D3D11_COMPARISON_LESS_EQUAL;
	}
}
D3D11_STENCIL_OP ConvertToNativeStencilOp(eStencilOp stencilOp) {
	switch (stencilOp) {
	case eStencilOp::DECR:		return D3D11_STENCIL_OP_DECR;
	case eStencilOp::DECR_SAT:	return D3D11_STENCIL_OP_DECR_SAT;
	case eStencilOp::INCR:		return D3D11_STENCIL_OP_INCR;
	case eStencilOp::INCR_SAT:	return D3D11_STENCIL_OP_INCR_SAT;
	case eStencilOp::INVERT:	return D3D11_STENCIL_OP_INVERT;
	case eStencilOp::KEEP:		return D3D11_STENCIL_OP_KEEP;
	case eStencilOp::REPLACE:	return D3D11_STENCIL_OP_REPLACE;
	case eStencilOp::ZERO:		return D3D11_STENCIL_OP_ZERO;
	default: return D3D11_STENCIL_OP_REPLACE;
	}
}
D3D11_DEPTH_STENCIL_DESC ConvertToNativeDepthStencil(const tDepthStencilDesc& depthStencil) {
	D3D11_DEPTH_STENCIL_DESC ret;
	ret.DepthEnable = (depthStencil.depthEnable ? TRUE : FALSE);
	ret.DepthFunc = ConvertToNativeCompFunc(depthStencil.depthCompare);
	ret.DepthWriteMask = (depthStencil.depthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO);
	ret.StencilEnable = (depthStencil.stencilEnable ? TRUE : FALSE);
	ret.StencilReadMask = depthStencil.stencilReadMask;
	ret.StencilWriteMask = depthStencil.stencilWriteMask;

	ret.BackFace.StencilDepthFailOp = ConvertToNativeStencilOp(depthStencil.stencilOpBackFace.stencilPassDepthFail);
	ret.BackFace.StencilFailOp = ConvertToNativeStencilOp(depthStencil.stencilOpBackFace.stencilFail);
	ret.BackFace.StencilPassOp = ConvertToNativeStencilOp(depthStencil.stencilOpBackFace.stencilPass);
	ret.BackFace.StencilFunc = ConvertToNativeCompFunc(depthStencil.stencilOpBackFace.stencilCompare);

	ret.FrontFace.StencilDepthFailOp = ConvertToNativeStencilOp(depthStencil.stencilOpFrontFace.stencilPassDepthFail);
	ret.FrontFace.StencilFailOp = ConvertToNativeStencilOp(depthStencil.stencilOpFrontFace.stencilFail);
	ret.FrontFace.StencilPassOp = ConvertToNativeStencilOp(depthStencil.stencilOpFrontFace.stencilPass);
	ret.FrontFace.StencilFunc = ConvertToNativeCompFunc(depthStencil.stencilOpFrontFace.stencilCompare);

	return ret;
}

D3D11_SAMPLER_DESC ConvertToNativeSampler(const tSamplerDesc& sDesc) {
	D3D11_SAMPLER_DESC r; memset(&r, 0, sizeof(r));

	switch (sDesc.addressU) {
		case eAddress::CLAMP:	r.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;	break;
		case eAddress::WRAP:	r.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	break;
		case eAddress::MIRROR:	r.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;	break;
	}

	switch (sDesc.addressV) {
		case eAddress::CLAMP:	r.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;	break;
		case eAddress::WRAP:	r.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	break;
		case eAddress::MIRROR:	r.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;	break;
	}

	// TODO these 5 shits
	r.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	r.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	r.MinLOD = 0.0f;
	r.MaxLOD = 1.0f;
	r.MipLODBias = 1;

	if (sDesc.filterMag == sDesc.filterMin && sDesc.filterMag == sDesc.filterMip) {
		switch (sDesc.filterMag) {
			case eFilter::POINT:		r.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	break;
			case eFilter::LINEAR:		r.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
			case eFilter::ANISOTROPIC:
			{
				r.Filter = D3D11_FILTER_ANISOTROPIC;
				r.MaxAnisotropy = 16; // TODO
				break;
			}
				
		}
	} else {
		// TODOO
	}
	
	return r;
}