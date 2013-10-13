
// Implementation
#include "GraphicsD3D11.h"

#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"

#include <cassert>
#define ASSERT assert

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
#pragma message("+++++ +++++ Function SetBBRenderTarget not found! +++++ +++++")
	//SetBBRenderTarget();
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

		// Wrap mode... for gpu's not supporting hardware accelerated D3D11
		//HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP , 0, flags, featurelevels, nFeatureLevels, D3D11_SDK_VERSION, &d3ddev, &featurelevel, &d3dcon); // For dx9 machines
		if(!FAILED(hr))
			break;
	}

	ZSASSERT_MSG( i != ARRAYSIZE(featurelevels), L"Can't create DirectX Device");

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

void cGraphicsD3D11::SetRenderTargetDefault() {
	d3dcon->RSSetViewports(1, &backBufferVP);
	d3dcon->OMSetRenderTargets(1, &backBufferRTV, backBufferDSV);
}

IVertexBuffer* cGraphicsD3D11::CreateVertexBuffer(size_t size, eBufferUsage usage, void* data /*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	switch(usage){
	case eBufferUsage::IMMUTABLE:	desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags=0;												break;
	case eBufferUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;							break;
	case eBufferUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE;	break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	memset(&resData, 0, sizeof(resData));
	resData.pSysMem = data;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	if (hr!=S_OK) {
		return NULL;
	}
	else {
		return new cVertexBufferD3D11(buffer);
	}
}

IIndexBuffer* cGraphicsD3D11::CreateIndexBuffer(size_t size  , eBufferUsage usage, void* data /*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	switch(usage){
	case eBufferUsage::IMMUTABLE:	desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags=0;												break;
	case eBufferUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;							break;
	case eBufferUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE;	break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	memset(&resData, 0, sizeof(resData));
	resData.pSysMem = data;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	if (hr!=S_OK) {
		return NULL;
	}
	else {
		return new cIndexBufferD3D11(buffer);
	}
}

bool cGraphicsD3D11::WriteBuffer(IIndexBuffer* buffer , void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return false;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes);
	if (hr!=S_OK) {
		return false;
	}

	memcpy((void*)(size_t(mappedRes.pData)+offset), source, size);

	d3dcon->Unmap(d3dBuffer, 0);

	return true;
}

bool cGraphicsD3D11::WriteBuffer(IVertexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return false;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes);
	if (hr!=S_OK) {
		return false;
	}

	memcpy((void*)(size_t(mappedRes.pData)+offset), source, size);

	d3dcon->Unmap(d3dBuffer, 0);

	return true;
}

bool cGraphicsD3D11::ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return false;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr!=S_OK) {
		return false;
	}

	memcpy(dest, (void*)(size_t(mappedRes.pData)+offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return true;
}

bool cGraphicsD3D11::ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	ASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return false;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr!=S_OK) {
		return false;
	}

	memcpy(dest, (void*)(size_t(mappedRes.pData)+offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return true;
}


void cGraphicsD3D11::Clear(bool target /*= true*/, bool depth /*= false*/, bool stencil /*= false*/) {
	static const FLOAT defaultClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Setup clear flags
	UINT clearFlags = 0;
	if(depth)
		clearFlags |= D3D11_CLEAR_DEPTH;
	if(stencil)
		clearFlags |= D3D11_CLEAR_STENCIL;

	// Clear depth, stencil if needed
	if(depth || stencil)
		d3dcon->ClearDepthStencilView(backBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Clear BackBuffer
	if(target)
		d3dcon->ClearRenderTargetView(backBufferRTV,defaultClearColor);
}

void cGraphicsD3D11::Present() {
	ZSASSERT_MSG(d3dsc != NULL, L"Need to set window for rendering");
	d3dsc->Present(0,0);
}

void cGraphicsD3D11::Draw(size_t nVertices, size_t idxStartVertex /*= 0*/) {
	d3dcon->Draw(nVertices, idxStartVertex);
}

void cGraphicsD3D11::DrawIndexed(size_t nIndices, size_t idxStartIndex /*= 0*/) {
	d3dcon->DrawIndexed(nIndices, idxStartIndex, 0);
}

void cGraphicsD3D11::DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	d3dcon->DrawInstanced(nVerticesPerInstance, nInstances, idxStartVertex, idxStartInstance);
}

void cGraphicsD3D11::DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex /*= 0*/, size_t idxStartInstance /*= 0*/) {
	d3dcon->DrawIndexedInstanced(nIndicesPerInstance, nInstances, idxStartIndex, 0, idxStartInstance);
}

void cGraphicsD3D11::SetVertexData(IVertexBuffer* vertexBuffer) {
	const UINT strides = vertexBuffer->GetStrides();
	const UINT offset = 0;

	d3dcon->IASetVertexBuffers(0, 1, (ID3D11Buffer* const*)vertexBuffer, &strides, &offset);
}

void cGraphicsD3D11::SetIndexData(IIndexBuffer* indexBuffer) {
	d3dcon->IASetIndexBuffer((ID3D11Buffer*)indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void cGraphicsD3D11::SetInstanceData() {

}
