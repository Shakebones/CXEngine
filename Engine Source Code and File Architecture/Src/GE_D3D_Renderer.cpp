/*
 * File name GE_D3D_Render.cpp
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc -
 * 
 * Definition
 */
#include "GE_D3D_Renderer.h"

CGE_D3D_Renderer::CGE_D3D_Renderer()
{
	this->m_pSwapChain = 0;
	this->m_pD3DDevice = 0;
	this->m_pDeviceContext = 0;
	this->m_pRenderTargetView = 0;
	this->m_pDepthStencilBuffer = 0;
	this->m_pDepthStencilView = 0;
	this->m_pDepthStencilState = 0;
	this->m_pRasterState = 0;
	this->m_pDepthDisabledStencilState = 0;
	this->m_enable_alpha_blending = 0;
	this->m_disable_alpha_blending = 0;
}

CGE_D3D_Renderer::CGE_D3D_Renderer(const CGE_D3D_Renderer& source)
{
}

CGE_D3D_Renderer& CGE_D3D_Renderer::operator=(const CGE_D3D_Renderer& source)
{
	return *this;
}

CGE_D3D_Renderer::~CGE_D3D_Renderer()
{
}

bool CGE_D3D_Renderer::Initialize(bool is_full_screen, int width, int height,
								  bool is_vsync_enabled, HWND hWnd, float screen_depth, float screen_near)
{
	HRESULT			result;
	IDXGIFactory*	pFactory = NULL;
	IDXGIAdapter*	pAdapter = NULL;
	IDXGIOutput*	pAdapterOutput = NULL;

	int				error;
	unsigned int	num_of_modes;
	unsigned int	numerator;
	unsigned int	denominator;
	unsigned int	string_length;
	
	DXGI_MODE_DESC*					pDisplayModeList;
	DXGI_ADAPTER_DESC				adapterDesc;
	DXGI_SWAP_CHAIN_DESC			swapChainDesc;
	D3D_FEATURE_LEVEL				featureLevel;
	ID3D11Texture2D*				pBackBufferPtr;
	D3D11_TEXTURE2D_DESC			depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC		depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC	depthStencilViewDesc;
	D3D11_DEPTH_STENCIL_DESC		depthDisabledStencilDesc;
	D3D11_RASTERIZER_DESC			rasterDesc;
	D3D11_BLEND_DESC				blendStateDesc;

	float fieldOfView;
	float screenAspect;

	this->m_vsync_enabled = is_vsync_enabled;
	numerator = denominator = 0;

	// About DXGI£º http://blog.csdn.net/pizi0475/article/details/7797472
	result = ::CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if( FAILED(result) )
		return false;

	result = pFactory->EnumAdapters(0, &pAdapter);
	if( FAILED(result) )
		return false;

	result = pAdapter->EnumOutputs(0, &pAdapterOutput);
	if( FAILED(result) )
		return false;

	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &num_of_modes, NULL);
	if( FAILED(result) )
		return false;

	pDisplayModeList = new DXGI_MODE_DESC[num_of_modes];
	if( !pDisplayModeList )
		return false;

	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &num_of_modes, pDisplayModeList);
	if( FAILED(result) )
		return false;

	for( unsigned int i = 0; i < num_of_modes; i++ )
	{
		if(pDisplayModeList[i].Width == (unsigned int)width)
		{
			if(pDisplayModeList[i].Height == (unsigned int)height)
			{
				numerator = pDisplayModeList[i].RefreshRate.Numerator;
				denominator = pDisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get video card description
	result = pAdapter->GetDesc(&adapterDesc);
	if( FAILED(result) )
		return false;

	this->m_videoCardMemory = (int)adapterDesc.DedicatedVideoMemory / 1024 / 1024;
	error = wcstombs_s(&string_length, this->m_videoCardDescription, 128, adapterDesc.Description, 128);

	if( 0 != error )
		return false;

	delete [] pDisplayModeList;
	pDisplayModeList = 0;

	ReleaseWithoutDel<IDXGIOutput*>(pAdapterOutput);
	ReleaseWithoutDel<IDXGIAdapter*>(pAdapter);
	ReleaseWithoutDel<IDXGIFactory*>(pFactory);

	::ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


	// VSYNC to control refresh-rate.
	if( this->m_vsync_enabled )
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}

	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if( is_full_screen )
		swapChainDesc.Windowed = false;

	else
		swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// After filled out swap-chain structure, feature-level, 
	// Now it's time to create swap-chain, device, context.
	result = ::D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel,
		1, D3D11_SDK_VERSION, &swapChainDesc, &this->m_pSwapChain, &this->m_pD3DDevice, NULL, &this->m_pDeviceContext);

	if( FAILED(result) )
		return false;

	result = this->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferPtr);
	if( FAILED(result) )
		return false;

	result = this->m_pD3DDevice->CreateRenderTargetView(pBackBufferPtr, NULL, &this->m_pRenderTargetView);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D11Texture2D*>(pBackBufferPtr);

	// Setup depth buffer
	::ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = this->m_pD3DDevice->CreateTexture2D(&depthBufferDesc, NULL, &this->m_pDepthStencilBuffer);
	if( FAILED(result) )
		return false;

	// Setup stencil buffer
	::ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = this->m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, &this->m_pDepthStencilState);
	if( FAILED(result) )
		return false;

	this->m_pDeviceContext->OMSetDepthStencilState(this->m_pDepthStencilState, 1);
	
	::ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = this->m_pD3DDevice->CreateDepthStencilView(this->m_pDepthStencilBuffer, &depthStencilViewDesc, &this->m_pDepthStencilView);
	if( FAILED(result) )
		return false;

	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetView, this->m_pDepthStencilView);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = this->m_pD3DDevice->CreateRasterizerState(&rasterDesc, &this->m_pRasterState);
	if( FAILED(result) )
		return false;

	this->m_pDeviceContext->RSSetState(this->m_pRasterState);
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	this->m_pDeviceContext->RSSetViewports(1, &viewport);
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)width/(float)height;

	// Create projection matrix for 3D space
	::D3DXMatrixPerspectiveFovLH(&this->m_projMatrix, fieldOfView, screenAspect, screen_near, screen_depth);

	::D3DXMatrixIdentity(&this->m_worldMatrix);

	// Create orthographic projection matrix for 2D space
	::D3DXMatrixOrthoLH(&this->m_orthoMatrix, (float)width, (float)height, screen_near, screen_depth);

	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = this->m_pD3DDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &this->m_pDepthDisabledStencilState);
	if( FAILED(result) )
		return false;

	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));
	
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	result = this->m_pD3DDevice->CreateBlendState(&blendStateDesc, &this->m_enable_alpha_blending);
	if( FAILED(result) )
		return false;

	blendStateDesc.RenderTarget[0].BlendEnable = FALSE;

	result = this->m_pD3DDevice->CreateBlendState(&blendStateDesc, &this->m_disable_alpha_blending);
	if( FAILED(result) )
		return false;

	return true;
}

void CGE_D3D_Renderer::Shutdown()
{
	if( this->m_pSwapChain )
		this->m_pSwapChain->SetFullscreenState(false, NULL);

	ReleaseWithoutDel<ID3D11BlendState*>(this->m_enable_alpha_blending);
	ReleaseWithoutDel<ID3D11BlendState*>(this->m_disable_alpha_blending);
	ReleaseWithoutDel<ID3D11DepthStencilState*>(this->m_pDepthDisabledStencilState);
	ReleaseWithoutDel<ID3D11RasterizerState*>(this->m_pRasterState);
	ReleaseWithoutDel<ID3D11DepthStencilView*>(this->m_pDepthStencilView);
	ReleaseWithoutDel<ID3D11DepthStencilState*>(this->m_pDepthStencilState);
	ReleaseWithoutDel<ID3D11Texture2D*>(this->m_pDepthStencilBuffer);
	ReleaseWithoutDel<ID3D11RenderTargetView*>(this->m_pRenderTargetView);
	ReleaseWithoutDel<ID3D11DeviceContext*>(this->m_pDeviceContext);
	ReleaseWithoutDel<ID3D11Device*>(this->m_pD3DDevice);
	ReleaseWithoutDel<IDXGISwapChain*>(this->m_pSwapChain);
}

void CGE_D3D_Renderer::BeginScene(D3DXCOLOR& _clear_backbuffer_color)
{
	float clear_color[4];
	clear_color[0] = _clear_backbuffer_color.r;
	clear_color[1] = _clear_backbuffer_color.g;
	clear_color[2] = _clear_backbuffer_color.b;
	clear_color[3] = _clear_backbuffer_color.a;

	// Clear back buffer
	this->m_pDeviceContext->ClearRenderTargetView(this->m_pRenderTargetView, clear_color);

	// Clear depth buffer
	this->m_pDeviceContext->ClearDepthStencilView(this->m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CGE_D3D_Renderer::EndScene()
{
	if( this->m_vsync_enabled )
		this->m_pSwapChain->Present(1, 0);

	else
		this->m_pSwapChain->Present(0, 0);
}

ID3D11Device* CGE_D3D_Renderer::GetDevicePointer()
{
	return this->m_pD3DDevice;
}

ID3D11DeviceContext* CGE_D3D_Renderer::GetDeviceContextPointer()
{
	return this->m_pDeviceContext;
}

void CGE_D3D_Renderer::GetProjectionMatrix(D3DXMATRIX& _out_proj_matrix)
{
	_out_proj_matrix = this->m_projMatrix;
}

void CGE_D3D_Renderer::GetOrthoMatrix(D3DXMATRIX& _out_ortho_matrix)
{
	_out_ortho_matrix = this->m_orthoMatrix;
}

void CGE_D3D_Renderer::GetWorldMatrix(D3DXMATRIX& _out_world_matrix)
{
	_out_world_matrix = this->m_worldMatrix;
}

void CGE_D3D_Renderer::GetVideoCardInfo(char* _out_cardName, int& _out_memory)
{
	::strcpy_s(_out_cardName, 128, this->m_videoCardDescription);
	_out_memory = this->m_videoCardMemory;
}

void CGE_D3D_Renderer::ResetViewPort()
{
	this->m_pDeviceContext->RSSetViewports(1, &this->viewport);
	return;
}

void CGE_D3D_Renderer::SetSolidMode()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	this->m_pD3DDevice->CreateRasterizerState(&rasterDesc, &this->m_pRasterState);
	this->m_pDeviceContext->RSSetState(this->m_pRasterState);
}

void CGE_D3D_Renderer::SetWireFrameMode()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	this->m_pD3DDevice->CreateRasterizerState(&rasterDesc, &this->m_pRasterState);
	this->m_pDeviceContext->RSSetState(this->m_pRasterState);
}

void CGE_D3D_Renderer::TurnZBufferOn()
{
	this->m_pDeviceContext->OMSetDepthStencilState(this->m_pDepthStencilState, 1);
}

void CGE_D3D_Renderer::TurnZBufferOff()
{
	this->m_pDeviceContext->OMSetDepthStencilState(this->m_pDepthDisabledStencilState, 1);
}

void CGE_D3D_Renderer::TurnAlphaBlendingOn()
{
	float blendFactor[4];

	for(int i = 0; i < 4; i++)
		blendFactor[i] = 0.0f;

	this->m_pDeviceContext->OMSetBlendState(this->m_enable_alpha_blending, blendFactor, 0xffffffff);
}

void CGE_D3D_Renderer::TurnAlphaBlendingOff()
{
	float blendFactor[4];

	for(int i = 0; i < 4; i++)
		blendFactor[i] = 0.0f;

	this->m_pDeviceContext->OMSetBlendState(this->m_disable_alpha_blending, blendFactor, 0xffffffff);
}

ID3D11DepthStencilView* CGE_D3D_Renderer::GetDepthStencilView()
{
	return this->m_pDepthStencilView;
}

void CGE_D3D_Renderer::SetBackBufferRenderTarget()
{
	this->m_pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetView, this->m_pDepthStencilView);
}