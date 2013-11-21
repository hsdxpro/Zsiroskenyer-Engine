#include "GraphicsApiD3D11.h"

#include "Dx11_SDK/Include/d3dx11.h"

#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "ConstantBufferD3D11.h"
#include "ShaderProgramD3D11.h"
#include "Texture2DD3D11.h"

#include "../../Core/src/IFile.h"


cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::tDxConfig::DEFAULT = cGraphicsApiD3D11::tDxConfig();
cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::tDxConfig::MEDIUM = cGraphicsApiD3D11::tDxConfig();
cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::tDxConfig::HIGH = cGraphicsApiD3D11::tDxConfig();
cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::swapChainConfig = cGraphicsApiD3D11::tDxConfig::DEFAULT;

cGraphicsApiD3D11::tDxConfig::tDxConfig()
:multiSampleQuality(0), multiSampleCount(1), createDeviceAtMaxResolution(false), createDeviceFullScreen(false) {
}

cGraphicsApiD3D11::cGraphicsApiD3D11()
: d3ddev(NULL), d3dcon(NULL), d3dsc(NULL), defaultRenderTarget(NULL) {
	// Create d3ddevice, d3dcontext
	CreateDevice();

	// Create default states
	CreateDefaultStates(D3D11_CULL_MODE::D3D11_CULL_BACK, D3D11_FILL_MODE::D3D11_FILL_SOLID);

	// If you want WIREFRAME MODE
	//CreateDefaultStates(D3D11_CULL_MODE::D3D11_CULL_NONE,D3D11_FILL_MODE::D3D11_FILL_WIREFRAME);
}

void cGraphicsApiD3D11::SetWindow(IWindow *renderWindow) {
	size_t clientWidth = renderWindow->GetClientWidth();
	size_t clientHeight = renderWindow->GetClientHeight();
	// Same window size : don't need new swap chain
	if(defaultRenderTarget != NULL)
	if( clientWidth == defaultRenderTarget->GetWidth() && clientHeight == defaultRenderTarget->GetHeight())
		return;

	// Create swap chain for device
	CreateMostAcceptableSwapChain(clientWidth, clientHeight, (HWND)(renderWindow->GetHandle()), cGraphicsApiD3D11::swapChainConfig);

	// Create main render target (BackBuffer)
	CreateRenderTargetViewForBB(swapChainConfig);

	// Create default viewport for swapChain rendering
	defaultVP.TopLeftX = 0,
	defaultVP.TopLeftY = 0;
	defaultVP.Width = clientWidth;
	defaultVP.Height = clientHeight;
	defaultVP.MaxDepth = 1.0f;
	defaultVP.MinDepth = 0.0f;

	// BackBuffer will be the render target in default
	SetRenderTargetDefault();
}

void cGraphicsApiD3D11::Release() {
	delete this;
}
cGraphicsApiD3D11::~cGraphicsApiD3D11() {
	ID3D11RenderTargetView *nulltarget[] = {0};
	d3dcon->OMSetRenderTargets(1, nulltarget, 0);

	if (d3dcon)d3dcon->ClearState();
	if (d3dcon)d3dcon->Flush();

	SAFE_RELEASE(defaultRenderTarget);
	SAFE_RELEASE(d3ddev);
	SAFE_RELEASE(d3dcon);
	SAFE_RELEASE(d3dsc);
}


eGapiResult cGraphicsApiD3D11::CreateDevice() {
	// create Graphic Infrastructure factory
	IDXGIFactory* fact = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&fact);

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
	for(; i < nFeatureLevels; i++, featurelevel = featurelevels[i]) {
		HRESULT hr = D3D11CreateDevice(mainAdapter, D3D_DRIVER_TYPE_UNKNOWN , 0, 0, featurelevels, nFeatureLevels, D3D11_SDK_VERSION, &d3ddev, &featurelevel, &d3dcon);

		// Wrap mode... for gpu's not supporting hardware accelerated D3D11
		//HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP , 0, flags, featurelevels, nFeatureLevels, D3D11_SDK_VERSION, &d3ddev, &featurelevel, &d3dcon); // For dx9 machines
		if(!FAILED(hr))
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

eGapiResult cGraphicsApiD3D11::CreateMostAcceptableSwapChain(size_t width, size_t height, HWND windowHandle, const tDxConfig& config) {
	if(d3dsc != NULL)
		SAFE_RELEASE(d3dsc);

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
	mainOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes,0);

	// get those displayModes
	modeDesc = new DXGI_MODE_DESC[numModes];
	mainOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modeDesc);

	// select displayModes that matches our renderWindow params and DirectX Config
	// worst case sized array
	DXGI_MODE_DESC** filteredVideoModes = new DXGI_MODE_DESC* [numModes];

	UINT displayModeIndex = 0;
	for(size_t i = 0; i < numModes; i++) {
		DXGI_MODE_DESC* currMode = &modeDesc[i];
		// Collect The best resolution that the video card handle
 		if(config.createDeviceAtMaxResolution) {
			// add to matched videoModes if that VideoMode have full screen resolution
			if(GetSystemMetrics(SM_CXSCREEN) == currMode->Width && GetSystemMetrics(SM_CYSCREEN) == currMode->Height) {
				filteredVideoModes[displayModeIndex] = currMode;
				displayModeIndex++;
			}		
			// Collect videoModes which resoltuion is equal with the window passed to ManagerDx
		}else if(currMode->Width == width && currMode->Height == height) {
			filteredVideoModes[displayModeIndex] = currMode;
			displayModeIndex++;
		}
	}

	// select highest HZ videoMode
	DXGI_MODE_DESC* selectedVideoMode = 0;
	UINT maxHz = 0;
	for(size_t i = 0; i < displayModeIndex;i++) {
		DXGI_MODE_DESC* curr = filteredVideoModes[i];
		if(maxHz < curr->RefreshRate.Numerator) {
			maxHz = curr->RefreshRate.Numerator;
			selectedVideoMode = curr;
		}
	}

	// Use the selected display mode to fill swap chain description
	DXGI_SWAP_CHAIN_DESC sdesc;
	sdesc.BufferCount = 1;
	if(selectedVideoMode != NULL) {
		sdesc.BufferDesc = *selectedVideoMode; // Copy DisplayMode Data
	} else {
		ASSERT_MSG(false, L"Using non standard resolution, this may slow down the DirectX application");
		sdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sdesc.BufferDesc.Width = width;
		sdesc.BufferDesc.Height = height;
		sdesc.BufferDesc.RefreshRate.Numerator = 1;
		sdesc.BufferDesc.RefreshRate.Denominator = 0;
		sdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	}
	sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sdesc.OutputWindow = windowHandle;
	sdesc.SampleDesc.Count = config.multiSampleCount;
	sdesc.SampleDesc.Quality = config.multiSampleQuality;
	sdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if(!config.createDeviceFullScreen)
		sdesc.Windowed = true;

	HRESULT hr = fact->CreateSwapChain(d3ddev, &sdesc, &d3dsc);
	
	// free up everything
	SAFE_RELEASE(mainOutput);
	SAFE_RELEASE(mainAdapter);
	SAFE_RELEASE(fact);
	
	delete[] filteredVideoModes;
	delete[] modeDesc;

	switch (hr) {
	case S_OK:
		return eGapiResult::OK;
	case E_OUTOFMEMORY:
		return eGapiResult::ERROR_OUT_OF_MEMORY;
	default:
		return eGapiResult::ERROR_UNKNOWN;
	}
}

eGapiResult cGraphicsApiD3D11::CreateRenderTargetViewForBB(const tDxConfig& config) {
	SAFE_RELEASE(defaultRenderTarget);

	ID3D11RenderTargetView* rtv;
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
		tD.SampleDesc.Count = config.multiSampleCount;
		tD.SampleDesc.Quality = config.multiSampleQuality;
		tD.Usage = D3D11_USAGE_DEFAULT;
	hr = d3ddev->CreateTexture2D(&tD, 0, &depthTexture);
	if (FAILED(hr)) {
		SAFE_RELEASE(backBuffer);

		ASSERT_MSG(false, "Failed to create depth buffer for swapChain");

		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}

	// create DepthStencilView
	hr = d3ddev->CreateDepthStencilView(depthTexture, NULL, &dsv);
	SAFE_RELEASE(depthTexture);
	SAFE_RELEASE(backBuffer);

	if (FAILED(hr)) {
		ASSERT_MSG(false, "Failed to create depth buffer VIEW for swapChain");
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}

	defaultRenderTarget = new cTexture2DD3D11(bbDesc.Width, bbDesc.Height, NULL, rtv, dsv);
	return eGapiResult::OK;
}


eGapiResult cGraphicsApiD3D11::CreateDefaultStates(const D3D11_CULL_MODE& cullMode, const D3D11_FILL_MODE& fillMode) {
	// default geometry topology!!
	d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*
	// default rasterizer state
	static D3D11_RASTERIZER_DESC rDesc;
	rDesc.AntialiasedLineEnable = false;
	rDesc.CullMode = cullMode;
	rDesc.DepthBias = 0;
	rDesc.DepthBiasClamp = 0.0f;
	rDesc.DepthClipEnable = true;
	rDesc.FillMode = fillMode;
	rDesc.FrontCounterClockwise = false;
	rDesc.MultisampleEnable = false;
	rDesc.ScissorEnable = false;
	rDesc.SlopeScaledDepthBias = 0.0f;
	ID3D11RasterizerState *rState = NULL;
	HRESULT hr = d3ddev->CreateRasterizerState(&rDesc,&rState);
	if (FAILED(hr)) {
		ASSERT_MSG(false, "Failed to create default rasterizer state");
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}
	d3dcon->RSSetState(rState);
	//rState->Release();
	*/

	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC d;
		d.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		d.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		d.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		d.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		d.Filter = D3D11_FILTER_ANISOTROPIC;
		d.MaxAnisotropy = 16;
		d.MaxLOD = 0;
		d.MinLOD = 0;
		d.MipLODBias = 0;

	HRESULT hr = d3ddev->CreateSamplerState(&d, &sampler);

	if (FAILED(hr)) {
		ASSERT_MSG(false, "Failed to create default sampler state");
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_UNKNOWN;
	}

	d3dcon->PSSetSamplers(0, 1, (ID3D11SamplerState**)&sampler);
	SAFE_RELEASE(sampler);
	return eGapiResult::OK;
}

HRESULT cGraphicsApiD3D11::CompileShaderFromFile(const zsString& fileName, const zsString& entry, const zsString& profile, ID3DBlob** ppBlobOut) {
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	// set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob;
	char ansiEntry[256];
	char ansiProfile[256];
	wcstombs(ansiEntry, entry.c_str(), 256);
	wcstombs(ansiProfile, profile.c_str(), 256);

	hr = D3DX11CompileFromFileW(fileName.c_str(), NULL, NULL, ansiEntry, ansiProfile,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr)) {
		if (pErrorBlob != NULL) {
			char* errorStr = (char*)pErrorBlob->GetBufferPointer();
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, errorStr, strlen(errorStr), NULL, 0);
			zsString errorStrW(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, errorStr, strlen(errorStr), &errorStrW[0], size_needed);

			ASSERT_MSG(false, L"Can't Compile :" + fileName + L"\n\n" + errorStrW);
		}
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


eGapiResult cGraphicsApiD3D11::CompileCgToHLSL(const zsString& cgFilePath, const zsString& hlslFilePath, eProfileCG compileProfile) {
	// Paths
	zsString cgcExePath = L"bin\\cgc.exe";
	zsString entryAndProfile;
	switch (compileProfile)
	{
	case eProfileCG::VS_5_0:
		entryAndProfile = L"-profile vs_5_0 -entry VS_MAIN";
		break;
	case eProfileCG::HS_5_0:
		entryAndProfile = L"-profile hs_5_0 -entry HS_MAIN";
		break;
	case eProfileCG::DS_5_0:
		entryAndProfile = L"-profile ds_5_0 -entry DS_MAIN";
		break;
	case eProfileCG::GS_5_0:
		entryAndProfile = L"-profile gs_5_0 -entry GS_MAIN";
		break;
	case eProfileCG::PS_5_0:
		entryAndProfile = L"-profile ps_5_0 -entry PS_MAIN";
		break;
	case eProfileCG::VS_4_0:
		entryAndProfile = L"-profile vs_4_0 -entry VS_MAIN";
		break;
	case eProfileCG::HS_4_0:
		entryAndProfile = L"-profile hs_4_0 -entry HS_MAIN";
		break;
	case eProfileCG::DS_4_0:
		entryAndProfile = L"-profile ds_4_0 -entry DS_MAIN";
		break;
	case eProfileCG::GS_4_0:
		entryAndProfile = L"-profile gs_4_0 -entry GS_MAIN";
		break;
	case eProfileCG::PS_4_0:
		entryAndProfile = L"-profile ps_4_0 -entry PS_MAIN";
		break;
	case eProfileCG::VS_3_0:
		entryAndProfile = L"-profile vs_3_0 -entry VS_MAIN";
		break;
	case eProfileCG::PS_3_0:
		entryAndProfile = L"-profile ps_3_0 -entry PS_MAIN";
		break;
	case eProfileCG::VS_2_0:
		entryAndProfile = L"-profile vs_2_0 -entry VS_MAIN";
		break;
	case eProfileCG::PS_2_0:
		entryAndProfile = L"-profile ps_2_0 -entry PS_MAIN";
		break;
	}
	//cgc.exe proba.fx -profile vs_5_0 -entry MAIN -o proba.vs
	zsString shellParams = cgcExePath + L" " + cgFilePath + L" " + entryAndProfile + L" -o " + hlslFilePath;

	// Process infos
	STARTUPINFO StartupInfo;
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	PROCESS_INFORMATION ProcessInfo;

	// LPWCSTR to LPWSTR
	wchar_t params[512];
	wcscpy(params, shellParams.c_str());

	// Start cgc.exe and Generate .hlsl from .cg
	BOOL appStarted = CreateProcessW(cgcExePath.c_str(), params, NULL, NULL, false, 0, NULL, NULL, &StartupInfo, &ProcessInfo);
	if (!appStarted) {
		ASSERT_MSG( false, L"Cannot execute cg shader compiler : " + cgcExePath);
		return eGapiResult::ERROR_UNKNOWN;
	}

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	return eGapiResult::OK;
}

eGapiResult	cGraphicsApiD3D11::CreateVertexBuffer(IVertexBuffer** resource, size_t size, eUsage usage, void* data/*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	switch(usage) {
	case eUsage::DEFAULT:		desc.Usage = D3D11_USAGE_DEFAULT;		desc.CPUAccessFlags = 0;												break;
	case eUsage::IMMUTABLE:		desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags = 0;												break;
	case eUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							break;
	case eUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE;		break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	resData.pSysMem = data;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	switch (hr) {
		case S_OK: 
			*resource = new cVertexBufferD3D11(buffer, desc.ByteWidth, usage);
			eGapiResult::OK;
		case E_OUTOFMEMORY:
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		default:
			return eGapiResult::ERROR_UNKNOWN;
	}	
}

eGapiResult	cGraphicsApiD3D11::CreateIndexBuffer(IIndexBuffer** resource, size_t size, eUsage usage, void* data/*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	switch(usage){
	case eUsage::DEFAULT:		desc.Usage = D3D11_USAGE_DEFAULT;		desc.CPUAccessFlags = 0;											break;
	case eUsage::IMMUTABLE:	desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags = 0;											break;
	case eUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;						break;
	case eUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE;	break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	resData.pSysMem = data;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	switch (hr) {
		case S_OK:
			*resource = new cIndexBufferD3D11(buffer, desc.ByteWidth, usage);
			eGapiResult::OK;
		case E_OUTOFMEMORY:
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		default:
			return eGapiResult::ERROR_UNKNOWN;
	}
}

eGapiResult cGraphicsApiD3D11::CreateConstantBuffer(IConstantBuffer** resource, size_t size, eUsage usage, void* data/* = NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size - (size % 16) + 16; // shader constants are multiple of 16 byte
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	switch(usage) {
	case eUsage::DEFAULT:		desc.Usage = D3D11_USAGE_DEFAULT;		desc.CPUAccessFlags = 0;												break;
	case eUsage::IMMUTABLE:	desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags = 0;												break;
	case eUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							break;
	case eUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;	break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	resData.pSysMem = data;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	switch (hr) {
		case S_OK:
			*resource = new cConstantBufferD3D11(buffer, desc.ByteWidth, usage);
			eGapiResult::OK;
		case E_OUTOFMEMORY:
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		default:
			return eGapiResult::ERROR_UNKNOWN;
	}
}

eGapiResult cGraphicsApiD3D11::CreateTexture(ITexture2D** resource, const zsString& filePath) {
	// Shader Resource View of texture
	ID3D11ShaderResourceView* srv;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(d3ddev, filePath.c_str(), 0, 0, &srv, 0);

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
		   *resource = new cTexture2DD3D11(width, height, srv);
		   return eGapiResult::OK;
		}
		default:
			return eGapiResult::ERROR_UNKNOWN;
	}
}

eGapiResult cGraphicsApiD3D11::CreateTexture(ITexture2D** resource, unsigned width, unsigned height, unsigned mipLevels, unsigned arraySize, eFormat format, unsigned bind, eFormat depthStencilFormat /*= eFormat::UNKNOWN*/) {

	ID3D11Texture2D* tex;
	// Outputs
	ID3D11RenderTargetView* rtv = NULL;
	ID3D11ShaderResourceView* srv = NULL;
	ID3D11DepthStencilView* dsv = NULL;

	bool isRenderTarget = (bool)((int)bind & (int)eBind::RENDER_TARGET);
	bool isShaderBindable = (bool)((int)bind & (int)eBind::SHADER_RESOURCE);
	bool hasDepthStencil = (bool)((int)bind & (int)eBind::DEPTH_STENCIL);

	UINT colorBindFlag = 0;
	if (isRenderTarget)   colorBindFlag |= D3D11_BIND_RENDER_TARGET;
	if (isShaderBindable) colorBindFlag |= D3D11_BIND_SHADER_RESOURCE;


	// TODO REMOVE THAT SHIT, MAKE INTERFACE FUNCTION TO FORCE CONVERTING / API FROM GENERAL FORMAT TO HIS FORMAT
	DXGI_FORMAT convertedFormat;
	switch (format) {
		case eFormat::R8G8B8A8_UNORM	: convertedFormat = DXGI_FORMAT_R8G8B8A8_UNORM;		break;
		case eFormat::R16G16_SINT		: convertedFormat = DXGI_FORMAT_R16G16_SINT;		break;
		case eFormat::R16G16B16A16_FLOAT: convertedFormat = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
		case eFormat::R32G32B32A32_FLOAT: convertedFormat = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	default:
		return eGapiResult::ERROR_INVALID_ARG;
	}
	
	D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = arraySize;
		texDesc.BindFlags = colorBindFlag;
		texDesc.CPUAccessFlags = 0; // TODO YOU CAN'T WRITE TEXTURES MUHAHA ( PARAMETERIZE )
		texDesc.Format = convertedFormat;
		texDesc.Height = height;
		texDesc.Width = width;
		texDesc.MipLevels = mipLevels;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT; // TODO ( PARAMETERIZE )
	// Create texture
	HRESULT hr = d3ddev->CreateTexture2D(&texDesc, NULL, &tex);
	if (FAILED(hr)) {
		ASSERT_MSG(false, L"Can't create texture2D");
		if (hr == E_OUTOFMEMORY)
			return eGapiResult::ERROR_OUT_OF_MEMORY;
		else
			return eGapiResult::ERROR_INVALID_ARG;
	}
		
	// Create RenderTarget view
	if (isRenderTarget) {
		hr = d3ddev->CreateRenderTargetView(tex, NULL, &rtv);
		if (FAILED(hr)) {
			SAFE_RELEASE(tex)
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_INVALID_ARG;
		}
	}

	// Create ShaderResource view
	if (isShaderBindable) {
		hr = d3ddev->CreateShaderResourceView(tex, NULL, &srv);
		if (FAILED(hr)) {
			SAFE_RELEASE(tex);
			SAFE_RELEASE(rtv);
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_INVALID_ARG;
		}
			
	}
	SAFE_RELEASE(tex);

	// Create DepthStencil texture
	if (hasDepthStencil) {
		// Convert depthStencil format
		DXGI_FORMAT f;
		switch (depthStencilFormat) {
			case eFormat::D24_UNORM_S8_UINT	: f = DXGI_FORMAT_D24_UNORM_S8_UINT;	break;
			case eFormat::D32_FLOAT			: f = DXGI_FORMAT_D32_FLOAT;			break;
			default:
				SAFE_RELEASE(tex);
				SAFE_RELEASE(rtv);
				SAFE_RELEASE(srv);
				return eGapiResult::ERROR_INVALID_ARG;
			break;
		}

		texDesc.ArraySize = arraySize;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0; // TODO YOU CAN'T WRITE TEXTURES MUHAHA
		texDesc.Format = f;
		texDesc.Height = height;
		texDesc.Width = width;
		texDesc.MipLevels = mipLevels;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		hr = d3ddev->CreateTexture2D(&texDesc, 0, &tex);
		if (FAILED(hr)) {
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_INVALID_ARG;
		}

		hr = d3ddev->CreateDepthStencilView(tex, NULL, &dsv);
		if (FAILED(hr)) {
			SAFE_RELEASE(tex);
			if (hr == E_OUTOFMEMORY)
				return eGapiResult::ERROR_OUT_OF_MEMORY;
			else
				return eGapiResult::ERROR_INVALID_ARG;
		}
	}

	*resource = new cTexture2DD3D11(width, height, srv, rtv, dsv);
	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::CreateShaderProgram(IShaderProgram** resource, const zsString& shaderPath) {


	// asd/asd/myShader cut extension
	zsString pathNoExt = shaderPath.substr(0, shaderPath.size() - 3);

	const size_t nShaders = 5;
	zsString binPaths[nShaders] = { pathNoExt + L"_vs.bin",
									pathNoExt + L"_hs.bin",
									pathNoExt + L"_ds.bin",
									pathNoExt + L"_gs.bin",
									pathNoExt + L"_ps.bin"};

	bool binExistences[nShaders];
	for (size_t i = 0; i < nShaders; i++)
		binExistences[i] = IFile::isFileExits(binPaths[i]);

	zsString entryNames[nShaders] =   { "VS_MAIN", "HS_MAIN", "DS_MAIN", "GS_MAIN", "PS_MAIN" };
	zsString profileNames[nShaders] = { "vs_5_0",  "hs_5_0",  "ds_5_0",  "gs_5_0",  "ps_5_0" };

	// Shader Output data
	ID3D11VertexShader* vs = NULL;
	ID3D11InputLayout* inputLayout = NULL;

	ID3D11PixelShader*		ps = NULL;
	ID3D11GeometryShader*	gs = NULL;
	ID3D11DomainShader*		ds = NULL;
	ID3D11HullShader*		hs = NULL;

	// Shader ByteCodes
	ID3DBlob* blobs[nShaders];
	void* byteCodes[nShaders];
	size_t byteCodeSizes[nShaders];

	// tmp hold shaderByteCode
	static char byteCodeHolder[nShaders][64000];

	IFile* cgFile = NULL;
	for (size_t i = 0; i < nShaders; i++) {
		byteCodeSizes[i] = 0;
		blobs[i] = 0;

		// Found binary ... Read it
		if (binExistences[i]) {
			byteCodeSizes[i] = IFile::GetSize(binPaths[i]);
			IFile::ReadBinary(binPaths[i], byteCodeHolder[i], byteCodeSizes[i]);
			byteCodes[i] = byteCodeHolder[i];
		} else { // There is no binary
			// If cg File not opened open it
			if (cgFile == NULL) cgFile = IFile::Create(shaderPath);

			// Found entry in cg
			if (cgFile->Find(entryNames[i])) {
				// Compile Cg to hlsl
				CompileCgToHLSL(shaderPath, binPaths[i], (eProfileCG)((int)eProfileCG::SM_5_0_BEGIN + i));

				// Compile hlsl to bytecode
				HRESULT hr = CompileShaderFromFile(binPaths[i], L"main", profileNames[i], &blobs[i]);
				if (FAILED(hr)) {
					ASSERT_MSG(false, L"Failed to compile hlsl file, something is wrong with the CG file: " + shaderPath);
					return eGapiResult::ERROR_UNKNOWN;
				}

				byteCodes[i] = blobs[i]->GetBufferPointer();
				byteCodeSizes[i] = blobs[i]->GetBufferSize();

				// Write binaries
				IFile::WriteBinary(binPaths[i], byteCodes[i], byteCodeSizes[i]);
			}
		}
	}

	HRESULT hr = S_OK;
	if (byteCodeSizes[0] != 0) {
		// Create VERTEX_SHADER from byteCode
		hr = d3ddev->CreateVertexShader(byteCodes[0], byteCodeSizes[0], NULL, &vs);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create vertex shader from bytecode: " + binPaths[0]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[1] != 0) {
		// Create HULL_SHADER from byteCode
		hr = d3ddev->CreateHullShader(byteCodes[1], byteCodeSizes[1], NULL, &hs);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create hull shader shader from bytecode: " + binPaths[1]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[2] != 0) {
		// Create DOMAIN_SHADER from byteCode
		hr = d3ddev->CreateDomainShader(byteCodes[2], byteCodeSizes[2], NULL, &ds);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create domain shader from bytecode: " + binPaths[2]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[3] != 0) {
		// Create GEOMETRY_SHADER from byteCode
		hr = d3ddev->CreateGeometryShader(byteCodes[3], byteCodeSizes[3], NULL, &gs);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create geometry shader from bytecode: " + binPaths[3]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	if (byteCodeSizes[4] != 0) {
		// Create PIXEL_SHADER from byteCode
		hr = d3ddev->CreatePixelShader(byteCodes[4], byteCodeSizes[4], NULL, &ps);
		if (FAILED(hr)) {
			ASSERT_MSG(false, L"Failed to create pixel shader from bytecode: " + binPaths[4]);
			return eGapiResult::ERROR_UNKNOWN;
		}
	}
	

	// Parsing cg
	// Parse input Layout... from VERTEX_SHADER
	// 1. search for "VS_MAIN(", get return value, for example VS_OUT
	// 2. search for VS_OUT, get lines under that, while line != "};"
	// 3. extract VERTEX DECLARATION from those lines

	zsString vsInStructName = cgFile->GetWordAfter(L" VS_MAIN(");
	std::list<zsString> vsInStructLines = cgFile->GetLinesUnder(vsInStructName, L"};");

	int nVertexAttributes = 0;

	// Count vertexAttributes
	auto iter = vsInStructLines.begin();
	while(iter != vsInStructLines.end()) {
		// not empty line... Parse Vertex Declaration
		if(iter->size() != 0)
			nVertexAttributes++;

		iter++;
	}
	
	// inputLayout descriptor (vertex Declaration)
	D3D11_INPUT_ELEMENT_DESC *vertexDecl = new D3D11_INPUT_ELEMENT_DESC[nVertexAttributes];
	size_t attribIdx = 0;
	size_t alignedByteOffset = 0;

	iter = vsInStructLines.begin();
	while(iter != vsInStructLines.end()) {
		// not empty line... Parse Vertex Declaration
		if(iter->size() != 0) {
			char semanticNames[10][32]; // Max 10 semantic, each 32 word length
			char semanticIndex[3]; // 999 max

			iter->GetWordBetween(':', ';', semanticNames[attribIdx]);
			iter->GetNumberFromEnd(semanticNames[attribIdx], semanticIndex);
			iter->CutNumberFromEnd(semanticNames[attribIdx]);

			// Gather format and size
			DXGI_FORMAT format;
			size_t byteSize;
			if(iter->find(L"float4") != std::wstring::npos) {
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				byteSize = 16;
			} else if(iter->find(L"float3") != std::wstring::npos) {
				format = DXGI_FORMAT_R32G32B32_FLOAT;
				byteSize = 12;
			} else if(iter->find(L"float2") != std::wstring::npos) {
				format = DXGI_FORMAT_R32G32_FLOAT;
				byteSize = 8;
			} else if(iter->find(L"float") != std::wstring::npos) {
				format = DXGI_FORMAT_R32_FLOAT;
				byteSize = 4;
			} 
			else
				ASSERT_MSG(false, L"Cg file parsing : " + shaderPath + L", can't match Input Vertex FORMAT");


			vertexDecl[attribIdx].SemanticName = semanticNames[attribIdx];
			vertexDecl[attribIdx].Format = format;
			vertexDecl[attribIdx].AlignedByteOffset = alignedByteOffset;
			vertexDecl[attribIdx].InputSlot = 0;
			vertexDecl[attribIdx].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexDecl[attribIdx].InstanceDataStepRate = 0;
			vertexDecl[attribIdx].SemanticIndex = (semanticIndex[0] == '\0') ? 0 : atoi(semanticIndex);

			alignedByteOffset += byteSize;
			attribIdx++;
		}
		iter++;
	}

	// Create input layout
	hr = d3ddev->CreateInputLayout(vertexDecl , nVertexAttributes, byteCodes[0], byteCodeSizes[0], &inputLayout);
	ASSERT_MSG(hr == S_OK, L"cGraphicsApiD3D11::CreateShaderProgram -> Can't create input layout for vertexShader: " + binPaths[0]);

	// FREE UP
	for (size_t i = 0; i < nShaders; i++)
			SAFE_RELEASE(blobs[i]);
	SAFE_RELEASE(cgFile);

	*resource = new cShaderProgramD3D11(alignedByteOffset, inputLayout, vs, hs, ds, gs, ps);
	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::WriteBuffer(IIndexBuffer* buffer , void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes);
	if (hr!=S_OK)
		return eGapiResult::ERROR_INVALID_ARG;

	memcpy((void*)(size_t(mappedRes.pData)+offset), source, size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::WriteBuffer(IVertexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes);
	if (hr!=S_OK)
		return eGapiResult::ERROR_INVALID_ARG;

	memcpy((void*)(size_t(mappedRes.pData)+offset), source, size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr!=S_OK)
		return eGapiResult::ERROR_INVALID_ARG;


	memcpy(dest, (void*)(size_t(mappedRes.pData)+offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return eGapiResult::ERROR_OUT_OF_RANGE;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr!=S_OK) {
		return eGapiResult::ERROR_INVALID_ARG;
	}

	memcpy(dest, (void*)(size_t(mappedRes.pData) + offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return eGapiResult::OK;
}

////////////////////
// draw
////////////////////
void cGraphicsApiD3D11::Clear(bool target /*= true*/, bool depth /*= false*/, bool stencil /*= false*/) {
	static const FLOAT defaultClearColor[4] = { 0.3f, 0.3f, 0.3f, 0.0f };

	// Setup clear flags
	UINT clearFlags = 0;
	if(depth)
		clearFlags |= D3D11_CLEAR_DEPTH;
	if(stencil)
		clearFlags |= D3D11_CLEAR_STENCIL;

	// Clear depth, stencil if needed
	if(depth || stencil)
		d3dcon->ClearDepthStencilView((ID3D11DepthStencilView*)defaultRenderTarget->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Clear BackBuffer
	if(target)
		d3dcon->ClearRenderTargetView((ID3D11RenderTargetView*)defaultRenderTarget->GetRTV(), defaultClearColor);
}

void cGraphicsApiD3D11::Present() {
	ASSERT_MSG(d3dsc != NULL, L"Need to set window for rendering");
	d3dsc->Present(0,0);
}

void cGraphicsApiD3D11::Draw(size_t nVertices, size_t idxStartVertex /*= 0*/) {
	d3dcon->Draw(nVertices, idxStartVertex);
}

void cGraphicsApiD3D11::DrawIndexed(size_t nIndices, size_t idxStartIndex /*= 0*/) {
	d3dcon->DrawIndexed(nIndices, idxStartIndex, 0);
}

void cGraphicsApiD3D11::DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	d3dcon->DrawInstanced(nVerticesPerInstance, nInstances, idxStartVertex, idxStartInstance);
}

void cGraphicsApiD3D11::DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	d3dcon->DrawIndexedInstanced(nIndicesPerInstance, nInstances, idxStartIndex, 0, idxStartInstance);
}

void cGraphicsApiD3D11::SetVertexBuffer(const IVertexBuffer* vertexBuffer, size_t vertexStride) {
	const UINT strides = vertexStride;
	const UINT offset = 0;
	ID3D11Buffer* vertices = ((cVertexBufferD3D11*)vertexBuffer)->GetBufferPointer();
	d3dcon->IASetVertexBuffers(0, 1, &vertices, &strides, &offset);
}

void cGraphicsApiD3D11::SetIndexBuffer(const IIndexBuffer* indexBuffer) {
	d3dcon->IASetIndexBuffer(((cIndexBufferD3D11*)indexBuffer)->GetBufferPointer(), DXGI_FORMAT_R32_UINT, 0);
}

void cGraphicsApiD3D11::SetInstanceData() {
}

void cGraphicsApiD3D11::SetTexture(const ITexture2D* tex, size_t slotIdx) {
	ASSERT(tex != NULL);
	const ID3D11ShaderResourceView* srv = ((cTexture2DD3D11*)tex)->GetSRV();
	d3dcon->PSSetShaderResources(slotIdx, 1, (ID3D11ShaderResourceView**)&srv);
}

void cGraphicsApiD3D11::SetShaderProgram(IShaderProgram* shProg) {
	const cShaderProgramD3D11* shProgD3D11 = (cShaderProgramD3D11*)shProg;
	d3dcon->IASetInputLayout(shProgD3D11->GetInputLayout());
	d3dcon->VSSetShader(shProgD3D11->GetVertexShader(), 0, 0);
	d3dcon->PSSetShader(shProgD3D11->GetPixelShader(), 0, 0);
}

void cGraphicsApiD3D11::SetPrimitiveTopology(ePrimitiveTopology t) {
	switch(t) {
		case ePrimitiveTopology::LINE_LIST:
			d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		case ePrimitiveTopology::TRIANGLE_LIST:
			d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
	}
}

void cGraphicsApiD3D11::SetVSConstantBuffer(IConstantBuffer* buffer, size_t slotIdx) {
	ID3D11Buffer* cBuffer = ((cConstantBufferD3D11*)buffer)->GetBufferPointer();
	d3dcon->VSSetConstantBuffers(slotIdx, 1, &cBuffer);
}

void cGraphicsApiD3D11::SetPSConstantBuffer(IConstantBuffer* buffer, size_t slotIdx) {
	ID3D11Buffer* cBuffer = ((cConstantBufferD3D11*)buffer)->GetBufferPointer();
	d3dcon->PSSetConstantBuffers(slotIdx, 1, &cBuffer);
}

eGapiResult cGraphicsApiD3D11::SetRenderTargets(unsigned nTargets, const ITexture2D* const* renderTargets, ITexture2D* depthStencilTarget) {
	// RTVS
	static ID3D11RenderTargetView* rtvS[16];
	for (unsigned i = 0; i < nTargets; i++)
		rtvS[i] = ((const cTexture2DD3D11*)(renderTargets[i]))->GetRTV();

	// DSV
	ID3D11DepthStencilView* dsv = NULL;
	if (depthStencilTarget) dsv = ((cTexture2DD3D11*)depthStencilTarget)->GetDSV();

	// Set them
	d3dcon->OMSetRenderTargets(nTargets, rtvS, dsv);
	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::SetRenderTargetDefault() {
	d3dcon->RSSetViewports(1, &defaultVP);
	ID3D11RenderTargetView* rtv = defaultRenderTarget->GetRTV();
	d3dcon->OMSetRenderTargets(1, &rtv, (ID3D11DepthStencilView*)defaultRenderTarget->GetDSV());
	return eGapiResult::OK;
}

eGapiResult cGraphicsApiD3D11::SetBackBufferSize(unsigned width, unsigned height) {

	// Release default render target (BackBuffer)
	SAFE_DELETE(defaultRenderTarget);

	HRESULT hr = d3dsc->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_EFFECT_DISCARD | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	if (!FAILED(hr)) {
		defaultVP.Width = width;
		defaultVP.Height = height;
		return CreateRenderTargetViewForBB(swapChainConfig);
	}
	switch (hr) {
		case S_OK: return eGapiResult::OK;   break;
		case E_OUTOFMEMORY: return eGapiResult::ERROR_OUT_OF_MEMORY; break;
		case E_INVALIDARG:	return eGapiResult::ERROR_INVALID_ARG; break;
		default:
			return eGapiResult::ERROR_UNKNOWN;
	}
}

ITexture2D* cGraphicsApiD3D11::GetDefaultRenderTarget() const {
	return defaultRenderTarget;
}