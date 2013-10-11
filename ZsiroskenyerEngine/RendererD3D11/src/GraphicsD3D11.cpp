// Implementation
#include "GraphicsD3D11.h"

#ifdef WIN32
#pragma warning(disable: 4244)
#pragma warning(disable: 4005)
#endif

cGraphicsD3D11::tDxConfig cGraphicsD3D11::tDxConfig::DEFAULT = cGraphicsD3D11::tDxConfig();
cGraphicsD3D11::tDxConfig cGraphicsD3D11::tDxConfig::MEDIUM = cGraphicsD3D11::tDxConfig();
cGraphicsD3D11::tDxConfig cGraphicsD3D11::tDxConfig::HIGH = cGraphicsD3D11::tDxConfig();
cGraphicsD3D11::tDxConfig cGraphicsD3D11::swapChainConfig = cGraphicsD3D11::tDxConfig::DEFAULT;

cGraphicsD3D11::tDxConfig::tDxConfig() 
	:multiSampleQuality(0), multiSampleCount(1), createDeviceAtMaxResolution(false), createDeviceFullScreen(false) {
}

cGraphicsD3D11::cGraphicsD3D11()
:d3ddev(NULL), d3dcon(NULL), d3dsc(NULL), backBufferRTV(NULL),backBufferDSV(NULL) {
	// Create d3ddevice, d3dcontext
	CreateDevice();

	// Create default states
	CreateDefaultStates(D3D11_CULL_MODE::D3D11_CULL_BACK,D3D11_FILL_MODE::D3D11_FILL_SOLID);
}

void cGraphicsD3D11::SetWindow(IWindow *renderWindow) {
	uint32 clientWidth = renderWindow->GetClientWidth();
	uint32 clientHeight = renderWindow->GetClientHeight();
	// Same window size : don't need new swap chain
	if(clientWidth == bbWidth && clientHeight == bbHeight)
		return;

	// Save new swap chain config
	//memcpy(&cGraphicsD3D11::swapChainConfig, &config, sizeof(tDxConfig));

	// Create swap chain for device
	CreateMostAcceptableSwapChain(clientWidth, clientHeight, (HWND)(renderWindow->GetHandle()), cGraphicsD3D11::swapChainConfig);

	// Create main render target (BackBuffer)
	CreateRenderTargetViewForBB(swapChainConfig);

	// Create viewport for swapChain rendering
	memset(&backBufferVP,0,sizeof(D3D11_VIEWPORT));
	backBufferVP.Width = clientWidth;
	backBufferVP.Height = clientHeight;
	backBufferVP.MaxDepth = 1.0f;
	backBufferVP.MinDepth = 0.0f;

	// BackBuffer will be the render target in default
	SetBBRenderTarget();
}

cGraphicsD3D11::~cGraphicsD3D11() {

	ID3D11RenderTargetView *nulltarget[] = {0};
	d3dcon->OMSetRenderTargets(1,nulltarget,0);

	if(d3dcon)d3dcon->ClearState();
	if(d3dcon)d3dcon->Flush();

	SAFE_RELEASE(backBufferRTV);
	SAFE_RELEASE(backBufferDSV);
	SAFE_RELEASE(d3ddev);
	SAFE_RELEASE(d3dcon);
	SAFE_RELEASE(d3dsc);
}

void cGraphicsD3D11::CreateDevice() {
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

	// Dx debug Mode if needed
	UINT flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// go from high feature level to lower, try to create device with each
	D3D_FEATURE_LEVEL featurelevel;
	int i = 0;
	const int nFeatureLevels = ARRAYSIZE(featurelevels);
	for(; i < nFeatureLevels; i++, featurelevel = featurelevels[i]) {
		HRESULT hr = D3D11CreateDevice(mainAdapter, D3D_DRIVER_TYPE_UNKNOWN , 0, flags, featurelevels, nFeatureLevels, D3D11_SDK_VERSION, &d3ddev, &featurelevel, &d3dcon);

		// Wrap mode...
		//HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP , 0, flags, featurelevels, nFeatureLevels, D3D11_SDK_VERSION, &d3ddev, &featurelevel, &d3dcon); // For dx9 machines
		if(!FAILED(hr))
			break;
	}

	ZSASSERT_MSG( i != ARRAYSIZE(featurelevels), "Can't create DirectX Device");

	SAFE_RELEASE(fact);
	SAFE_RELEASE(mainAdapter);
}

void cGraphicsD3D11::CreateMostAcceptableSwapChain(uint16 width, uint16 height, HWND windowHandle, const tDxConfig& config) {
	if(d3dsc != NULL)
		SAFE_RELEASE(d3dsc);

	// create Graphic Infrastructure factory
	IDXGIFactory* fact = NULL;
	CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&fact);

	// enumerate adapters
	IDXGIAdapter* mainAdapter = NULL;
	UINT adapterIndex = 0;
	fact->EnumAdapters(adapterIndex,&mainAdapter);

	// enumerate outputs
	IDXGIOutput* mainOutput = NULL;
	DXGI_MODE_DESC* modeDesc = NULL;
	UINT outputIndex = 0;
	mainAdapter->EnumOutputs(outputIndex,&mainOutput);

	// how many displayModes ?
	UINT numModes = 0;
	mainOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,0,&numModes,0);

	// get those displayModes
	modeDesc = new DXGI_MODE_DESC[numModes];
	mainOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modeDesc);

	// select displayModes that matches our renderWindow params and DirectX Config
	// worst case sized array
	DXGI_MODE_DESC** filteredVideoModes = new DXGI_MODE_DESC*[numModes];

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
	memset(&sdesc,0,sizeof(sdesc));
	sdesc.BufferCount = 1;
	if(selectedVideoMode != NULL) {
		sdesc.BufferDesc = *selectedVideoMode; // Copy DisplayMode Data
	} else {
		zsPrintError("Using non standard resolution, this may slow down the DirectX application");
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

	HRESULT hr = fact->CreateSwapChain(d3ddev,&sdesc,&d3dsc);
	if(FAILED(hr)) zsPrintError("Can't create DirectX's Swap Chain");

	// free up everything
	SAFE_RELEASE(fact);
	SAFE_RELEASE(mainAdapter);
	SAFE_RELEASE(mainOutput);
	delete[] filteredVideoModes;
	delete[] modeDesc;
}

void cGraphicsD3D11::CreateRenderTargetViewForBB(const tDxConfig& config) {
	SAFE_RELEASE(backBufferRTV);
	SAFE_RELEASE(backBufferDSV);

	ID3D11Texture2D *backBuffer = NULL;
	d3dsc->GetBuffer(NULL,__uuidof(ID3D11Resource*),(void**)&backBuffer);
	d3ddev->CreateRenderTargetView(backBuffer,0,&backBufferRTV);

	D3D11_TEXTURE2D_DESC bbDesc;
	memset(&bbDesc,0,sizeof(bbDesc));
	backBuffer->GetDesc(&bbDesc);
	bbWidth = bbDesc.Width;
	bbHeight = bbDesc.Height;

	// create Depth texture
	D3D11_TEXTURE2D_DESC tD;
	memset(&tD,0,sizeof(tD));
	tD.ArraySize = 1;
	tD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tD.CPUAccessFlags = 0;
	tD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tD.Width = bbWidth;
	tD.Height = bbHeight;
	tD.MipLevels = 1;
	tD.MiscFlags = 0;
	tD.SampleDesc.Count = config.multiSampleCount;
	tD.SampleDesc.Quality = config.multiSampleQuality;
	tD.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D *depthTexture = NULL;
	d3ddev->CreateTexture2D(&tD,0,&depthTexture);

	// create DepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	memset(&dsvDesc,0,sizeof(dsvDesc));
	dsvDesc.Format = tD.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3ddev->CreateDepthStencilView(depthTexture,&dsvDesc,&backBufferDSV);

	depthTexture->Release();
	backBuffer->Release();
}


void cGraphicsD3D11::CreateDefaultStates(const D3D11_CULL_MODE& cullMode, const D3D11_FILL_MODE& fillMode) {
	// default geometry topology!!
	d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// default rasterizer state
	D3D11_RASTERIZER_DESC rDesc;
	memset(&rDesc,0,sizeof(rDesc));
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
	d3ddev->CreateRasterizerState(&rDesc,&rState);
	d3dcon->RSSetState(rState);
	rState->Release();
}

void cGraphicsD3D11::CreateVertexBuffer(uint32 vertexStride, uint32 nVertex, void *vertices, ID3D11Buffer **vb) {
	D3D11_BUFFER_DESC vbDesc;
	memset(&vbDesc, 0, sizeof(vbDesc));
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.ByteWidth = vertexStride * nVertex;
	vbDesc.StructureByteStride = vertexStride;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices;
	HRESULT hr = d3ddev->CreateBuffer(&vbDesc, &initData, vb);
	if(FAILED(hr)) {
		zsPrintError("Can't create vertex buffer");
	}
}

void cGraphicsD3D11::CreateIndexBuffer(uint32 indexStride, uint32 nIndex, void *indices, ID3D11Buffer **ib) {
	D3D11_BUFFER_DESC ibDesc;
	memset(&ibDesc, 0, sizeof(ibDesc));
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.ByteWidth = indexStride * nIndex;
	ibDesc.StructureByteStride = indexStride;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;
	HRESULT hr = d3ddev->CreateBuffer(&ibDesc, &initData, ib);
	if(FAILED(hr)) {
		zsPrintError("Can't create index buffer");
	}
}

void cGraphicsD3D11::BBClear(bool clearOnlyDepth) {
	static const FLOAT defaultClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	d3dcon->ClearDepthStencilView(backBufferDSV,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0,0);
	if(!clearOnlyDepth) {
		d3dcon->ClearRenderTargetView(backBufferRTV,defaultClearColor);
	}
}

void cGraphicsD3D11::Draw(uint32 vertexCount) {
	d3dcon->Draw(vertexCount,0);
}

void cGraphicsD3D11::BBPresent() {
	ZSASSERT_MSG(d3dsc != NULL, "Need to set window for rendering");
	d3dsc->Present(0,0); 
}

void cGraphicsD3D11::DrawIndexed(uint32 indexCount) {
	d3dcon->DrawIndexed(indexCount,0,0);
}

void cGraphicsD3D11::DrawIndexedInstanced(uint32 indexCount, uint32 instanceCount) {
	d3dcon->DrawIndexedInstanced(indexCount,instanceCount,0,0,0);
}

void cGraphicsD3D11::SetBBRenderTarget() {
	d3dcon->RSSetViewports(1, &backBufferVP);
	d3dcon->OMSetRenderTargets(1, &backBufferRTV, backBufferDSV);
}

