#include "GraphicsApiD3D11.h"
#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "ConstantBufferD3D11.h"
#include "ShaderProgramD3D11.h"
#include "Texture2DColorD3D11.h"

#include "../../Core/src/IFile.h"

#ifdef WIN32
#pragma warning(disable: 4244)
#pragma warning(disable: 4005)
#endif

cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::tDxConfig::DEFAULT = cGraphicsApiD3D11::tDxConfig();
cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::tDxConfig::MEDIUM = cGraphicsApiD3D11::tDxConfig();
cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::tDxConfig::HIGH = cGraphicsApiD3D11::tDxConfig();
cGraphicsApiD3D11::tDxConfig cGraphicsApiD3D11::swapChainConfig = cGraphicsApiD3D11::tDxConfig::DEFAULT;

cGraphicsApiD3D11::tDxConfig::tDxConfig() 
	:multiSampleQuality(0), multiSampleCount(1), createDeviceAtMaxResolution(false), createDeviceFullScreen(false) {
}

cGraphicsApiD3D11::cGraphicsApiD3D11()
:d3ddev(NULL), d3dcon(NULL), d3dsc(NULL), backBufferRTV(NULL),backBufferDSV(NULL) {
	// Create d3ddevice, d3dcontext
	CreateDevice();

	// Create default states
	CreateDefaultStates(D3D11_CULL_MODE::D3D11_CULL_FRONT,D3D11_FILL_MODE::D3D11_FILL_SOLID);

	// If you want WIREFRAME MODE
	//CreateDefaultStates(D3D11_CULL_MODE::D3D11_CULL_NONE,D3D11_FILL_MODE::D3D11_FILL_WIREFRAME);
}

void cGraphicsApiD3D11::SetWindow(IWindow *renderWindow) {
	uint32 clientWidth = renderWindow->GetClientWidth();
	uint32 clientHeight = renderWindow->GetClientHeight();
	// Same window size : don't need new swap chain
	if(clientWidth == bbWidth && clientHeight == bbHeight)
		return;

	// Save new swap chain config
	//memcpy(&cGraphicsApiD3D11::swapChainConfig, &config, sizeof(tDxConfig));

	// Create swap chain for device
	CreateMostAcceptableSwapChain(clientWidth, clientHeight, (HWND)(renderWindow->GetHandle()), cGraphicsApiD3D11::swapChainConfig);

	// Create main render target (BackBuffer)
	CreateRenderTargetViewForBB(swapChainConfig);

	// Create viewport for swapChain rendering
	memset(&backBufferVP,0,sizeof(D3D11_VIEWPORT));
	backBufferVP.Width = clientWidth;
	backBufferVP.Height = clientHeight;
	backBufferVP.MaxDepth = 1.0f;
	backBufferVP.MinDepth = 0.0f;

	// BackBuffer will be the render target in default
	SetRenderTargetDefault();
}

void cGraphicsApiD3D11::Release() {
	ID3D11RenderTargetView *nulltarget[] = {0};
	d3dcon->OMSetRenderTargets(1, nulltarget, 0);

	if(d3dcon)d3dcon->ClearState();
	if(d3dcon)d3dcon->Flush();

	SAFE_RELEASE(backBufferRTV);
	SAFE_RELEASE(backBufferDSV);
	SAFE_RELEASE(d3ddev);
	SAFE_RELEASE(d3dcon);
	SAFE_RELEASE(d3dsc);
}

void cGraphicsApiD3D11::CreateDevice() {
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

void cGraphicsApiD3D11::CreateMostAcceptableSwapChain(uint16 width, uint16 height, HWND windowHandle, const tDxConfig& config) {
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
		ILog::GetInstance()->MsgBox(L"Using non standard resolution, this may slow down the DirectX application");
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
	if(FAILED(hr)) ILog::GetInstance()->MsgBox(L"Can't create DirectX's Swap Chain");

	// free up everything
	SAFE_RELEASE(fact);
	SAFE_RELEASE(mainAdapter);
	SAFE_RELEASE(mainOutput);
	delete[] filteredVideoModes;
	delete[] modeDesc;
}

void cGraphicsApiD3D11::CreateRenderTargetViewForBB(const tDxConfig& config) {
	SAFE_RELEASE(backBufferRTV);
	SAFE_RELEASE(backBufferDSV);

	ID3D11Texture2D *backBuffer = NULL;
	d3dsc->GetBuffer(NULL,__uuidof(ID3D11Resource*),(void**)&backBuffer);
	d3ddev->CreateRenderTargetView(backBuffer, 0, &backBufferRTV);

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


void cGraphicsApiD3D11::CreateDefaultStates(const D3D11_CULL_MODE& cullMode, const D3D11_FILL_MODE& fillMode) {
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

void cGraphicsApiD3D11::SetRenderTargetDefault() {
	d3dcon->RSSetViewports(1, &backBufferVP);
	d3dcon->OMSetRenderTargets(1, &backBufferRTV, backBufferDSV);
}

IVertexBuffer* cGraphicsApiD3D11::CreateBufferVertex(size_t nVertices, size_t vertexStride, eBufferUsage usage, void* data /*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = nVertices * vertexStride;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = vertexStride;
	switch(usage){
	case eBufferUsage::DEFAULT:		desc.Usage = D3D11_USAGE_DEFAULT;		desc.CPUAccessFlags = 0;												break;
	case eBufferUsage::IMMUTABLE:	desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags = 0;												break;
	case eBufferUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							break;
	case eBufferUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE;		break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	resData.pSysMem = data;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	if (hr!=S_OK) {
		return NULL;
	}
	else {
		return new cVertexBufferD3D11(buffer, desc.ByteWidth, usage);
	}
}

IIndexBuffer* cGraphicsApiD3D11::CreateBufferIndex(size_t size , eBufferUsage usage, void* data /*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(unsigned);
	switch(usage){
	case eBufferUsage::DEFAULT:		desc.Usage = D3D11_USAGE_DEFAULT;		desc.CPUAccessFlags = 0;											break;
	case eBufferUsage::IMMUTABLE:	desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags = 0;											break;
	case eBufferUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;						break;
	case eBufferUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE;	break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	memset(&resData, 0, sizeof(resData));
	resData.pSysMem = data;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	if (hr != S_OK) {
		return NULL;
	}
	else {
		return new cIndexBufferD3D11(buffer, desc.ByteWidth, usage);
	}
}

IConstantBuffer* cGraphicsApiD3D11::CreateBufferConstant(size_t size , eBufferUsage usage, void* data /*= NULL*/) {
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	switch(usage){
	case eBufferUsage::DEFAULT:		desc.Usage = D3D11_USAGE_DEFAULT;		desc.CPUAccessFlags = 0;												break;
	case eBufferUsage::IMMUTABLE:	desc.Usage = D3D11_USAGE_IMMUTABLE;		desc.CPUAccessFlags = 0;												break;
	case eBufferUsage::DYNAMIC:		desc.Usage = D3D11_USAGE_DYNAMIC;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							break;
	case eBufferUsage::STAGING:		desc.Usage = D3D11_USAGE_STAGING;		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;	break;
	}

	D3D11_SUBRESOURCE_DATA resData;
	memset(&resData, 0, sizeof(resData));
	resData.pSysMem = data;

	HRESULT hr = d3ddev->CreateBuffer(&desc, &resData, &buffer);
	if (hr != S_OK) {
		return NULL;
	}
	else {
		return new cConstantBufferD3D11(buffer, desc.ByteWidth, usage);
	}
}

ITexture2D*	cGraphicsApiD3D11::CreateTexture(const zsString& filePath) {
	// Shader Resource View of texture
	ID3D11ShaderResourceView* srv;

	size_t width;
	size_t height;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(d3ddev, filePath.c_str(), 0, 0, &srv, 0);
	if(FAILED(hr)) {
		ILog::GetInstance()->MsgBox(L"cGraphicsApiD3D11::CreateTexture Failed to load :" + filePath);
	} else {
		ILog::GetInstance()->Log(L"Texture Created: " + filePath);

		// Get Width, Height
		ID3D11Texture2D* tex2D;
		srv->GetResource((ID3D11Resource**)&tex2D);
		D3D11_TEXTURE2D_DESC texDesc;
		tex2D->GetDesc(&texDesc);
		width = texDesc.Width;
		height = texDesc.Height;
		tex2D->Release();
	}

	return new cTexture2DColorD3D11(srv, width, height);
}

IShaderProgram* cGraphicsApiD3D11::CreateShaderProgram(const zsString& shaderPath) {

	// For example, test.cg  ->  test
	zsString shaderWithoutExtension = shaderPath;
	shaderWithoutExtension = shaderWithoutExtension.substr(0, shaderWithoutExtension.size() - 3);
	

	// detect that the .cg have VS_MAIN, PS_MAIN, ..... etc
	zsString cgFullPath = shaderPath;
	zsString hlslVsFullPath = shaderWithoutExtension + L"_vs.hlsl";
	zsString hlslPsFullPath = shaderWithoutExtension + L"_ps.hlsl";
	bool cgHaveVS = false;
	bool cgHavePS = false;

	IFile* cgFile = IFile::Create(cgFullPath);
	if(cgFile->Find(L"VS_MAIN")) {
		cgHaveVS = true;
		CompileCgToHLSL(cgFullPath, hlslVsFullPath, eProfileCG::VS_5_0);
	}
	if(cgFile->Find(L"PS_MAIN")) {
		cgHavePS = true;
		CompileCgToHLSL(cgFullPath, hlslPsFullPath, eProfileCG::PS_5_0);
	}

	// Shader Compiling creating.. and input layout creation
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;
	ID3D11InputLayout* inputLayout;
	ID3DBlob *vsBlob;
	ID3DBlob *psBlob;
	
	// Compile, Create VERTEX_SHADER
	while(FAILED(CompileShaderFromFile(hlslVsFullPath, L"main", L"vs_5_0", &vsBlob)))
		ILog::GetInstance()->MsgBox(L".cg VERTEX SHADER parts are wrong, repair it i wait you: " + cgFullPath);

	HRESULT hr = d3ddev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vs);
	if(FAILED(hr))
		ILog::GetInstance()->MsgBox(L"Failed to create vertex shader from bytecode: " + hlslVsFullPath);

	// Compile, Create PIXEL_SHADER
	while(FAILED(CompileShaderFromFile(hlslPsFullPath, L"main", L"ps_5_0", &psBlob)))
		ILog::GetInstance()->MsgBox(L".cg PIXEL SHADER parts are wrong, repair it i wait you: " + cgFullPath);

	hr = d3ddev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &ps);
	if(FAILED(hr))
		ILog::GetInstance()->MsgBox(L"Failed to create vertex shader from bytecode: " + hlslPsFullPath);


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
		if(iter->size() != 0) {
			nVertexAttributes++;
		}
		iter++;
	}
	
	// inputLayout descriptor (vertex Declaration)
	D3D11_INPUT_ELEMENT_DESC *vertexDecl = new D3D11_INPUT_ELEMENT_DESC[nVertexAttributes];
	size_t attribIdx = 0;
	size_t alignedByteOffset = 0;
	cVertexFormat vertexFormat;

	iter = vsInStructLines.begin();
	while(iter != vsInStructLines.end()) {
		// not empty line... Parse Vertex Declaration
		if(iter->size() != 0) {
			const char* semanticName;
			eVertexAttribute attribType;

			// Gather Semantic name
			if(iter->find(L"POSITION") != std::wstring::npos) {
				semanticName = "POSITION";
				attribType = eVertexAttribute::POSITION;
			}
			else if(iter->find(L"NORMAL") != std::wstring::npos) {
				semanticName = "NORMAL";
				attribType = eVertexAttribute::NORMAL;
			}
			else if(iter->find(L"TEXCOORD") != std::wstring::npos) {
				semanticName = "TEXCOORD";
				attribType = eVertexAttribute::TEXCOORD;
			}
			else if(iter->find(L"COLOR") != std::wstring::npos) {
				semanticName = "COLOR";
				attribType = eVertexAttribute::COLOR;
			}
			else if(iter->find(L"TANGENT") != std::wstring::npos) {
				semanticName = "TANGENT";
				attribType = eVertexAttribute::TANGENT;
			}
			else
				ILog::GetInstance()->MsgBox(L"Cg compiling, can't math SEMANTIC NAME");

			// Add new Attribute
			vertexFormat.AddAttribute(attribType);

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
				ILog::GetInstance()->MsgBox(L"Cg compiling, can't match Input Vertex FORMAT");

			vertexDecl[attribIdx].SemanticName = semanticName;
			vertexDecl[attribIdx].Format = format;
			vertexDecl[attribIdx].AlignedByteOffset = alignedByteOffset;
			vertexDecl[attribIdx].InputSlot = 0;
			vertexDecl[attribIdx].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexDecl[attribIdx].InstanceDataStepRate = 0;
			vertexDecl[attribIdx].SemanticIndex = 0;

			alignedByteOffset += byteSize;
			attribIdx++;
		}
		iter++;
	}

	// Create input layout
	hr = d3ddev->CreateInputLayout(vertexDecl , nVertexAttributes, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
	if(FAILED(hr))
		ILog::GetInstance()->MsgBox(L"cGraphicsApiD3D11::CreateShaderProgram -> Can't create input layout for vertexShader: " + hlslVsFullPath);

	vsBlob->Release();
	psBlob->Release();

	return new cShaderProgramD3D11(vertexFormat, alignedByteOffset, inputLayout, vs, ps);
}

bool cGraphicsApiD3D11::WriteBuffer(IIndexBuffer* buffer , void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ZSASSERT(buffer!=NULL);

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

bool cGraphicsApiD3D11::WriteBuffer(IVertexBuffer* buffer, void* source, size_t size /*= ZS_NUMLIMITMAX(size_t)*/, size_t offset /*= 0*/) {
	ZSASSERT(buffer!=NULL);

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

bool cGraphicsApiD3D11::ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset /*= 0*/) {
	ZSASSERT(buffer!=NULL);

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

bool cGraphicsApiD3D11::ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset /*= 0*/) {
	ZSASSERT(buffer!=NULL);

	if (buffer->GetSize()<size+offset)
		return false;

	HRESULT hr = S_OK;
	ID3D11Buffer* d3dBuffer = (ID3D11Buffer*)buffer;
	D3D11_MAPPED_SUBRESOURCE mappedRes;

	hr = d3dcon->Map(d3dBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
	if (hr!=S_OK) {
		return false;
	}

	memcpy(dest, (void*)(size_t(mappedRes.pData) + offset), size);

	d3dcon->Unmap(d3dBuffer, 0);

	return true;
}

void cGraphicsApiD3D11::SetConstantBuffer(IConstantBuffer* buffer, size_t slotIdx) {
	ID3D11Buffer* cBuffer = ((cConstantBufferD3D11*)buffer)->GetBufferPointer();
	d3dcon->VSSetConstantBuffers(slotIdx, 1, &cBuffer);
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
		d3dcon->ClearDepthStencilView(backBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Clear BackBuffer
	if(target)
		d3dcon->ClearRenderTargetView(backBufferRTV, defaultClearColor);
}

void cGraphicsApiD3D11::Present() {
	ZSASSERT_MSG(d3dsc != NULL, L"Need to set window for rendering");
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
	ID3D11ShaderResourceView *srv = ((cTexture2DColorD3D11*)tex)->GetSRV();
	d3dcon->PSSetShaderResources(slotIdx, 1, &srv);
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
			d3dcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
	}
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
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
	if(FAILED(hr)) { 
		if(pErrorBlob != NULL) {
			char* errorStr = (char*)pErrorBlob->GetBufferPointer();
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, errorStr, strlen(errorStr), NULL, 0);
			zsString errorStrW(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, errorStr, strlen(errorStr), &errorStrW[0], size_needed);

			ILog::GetInstance()->MsgBox(L"Can't Compile :" + fileName + L"\n\n" + errorStrW);
		}
		if( pErrorBlob ) pErrorBlob->Release();
		return hr;
	}
	if( pErrorBlob ) pErrorBlob->Release();

	return S_OK;
}


void cGraphicsApiD3D11::CompileCgToHLSL(const zsString& cgFilePath, const zsString& hlslFilePath, eProfileCG compileProfile) {
	// Paths
	zsString cgcExePath = L"bin\\cgc.exe";
	zsString entryAndProfile;
	switch(compileProfile)
	{
		case eProfileCG::VS_5_0 :
			entryAndProfile =  L"-profile vs_5_0 -entry VS_MAIN";
			break;
		case eProfileCG::PS_5_0 :
			entryAndProfile =  L"-profile ps_5_0 -entry PS_MAIN";
			break;
		case eProfileCG::VS_4_0 :
			entryAndProfile =  L"-profile vs_4_0 -entry VS_MAIN";
			break;
		case eProfileCG::PS_4_0 :
			entryAndProfile =  L"-profile ps_4_0 -entry PS_MAIN";
			break;
	}
	//cgc.exe proba.fx -profile vs_3_0 -entry MAIN -o proba.vs
	zsString shellParams = cgcExePath + L" " + cgFilePath + L" " + entryAndProfile + L" -o " + hlslFilePath;

	// Process infos
	STARTUPINFO StartupInfo;
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo;
	PROCESS_INFORMATION ProcessInfo;

	// LPWCSTR to LPWSTR
	wchar_t params[512];
	wcscpy(params, shellParams.c_str());

	// Start cgc.exe and Generate .hlsl from .cg
	bool appStarted = CreateProcessW(cgcExePath.c_str(), params, NULL, NULL, false, 0, NULL, NULL, &StartupInfo, &ProcessInfo);
	if(appStarted) {
		WaitForSingleObject( ProcessInfo.hProcess, INFINITE );
	} else {
		ILog::GetInstance()->MsgBox(L"Cannot execute cg shader compiler : " + cgcExePath);
	}
}