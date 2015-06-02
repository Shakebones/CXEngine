/*
 * File name - GERenderToTexture.cpp
 * Author - CuiXue
 * Updated date - 2013.3.18
 * Desc - 
 *
 */
#include "GERenderToTexture.h"

CGERenderToTexture::CGERenderToTexture()
{
	this->m_pRenderTargetTexture = 0;
	this->m_pRenderTargetView = 0;
	this->m_pShaderResourceView = 0;
}

CGERenderToTexture::CGERenderToTexture(const CGERenderToTexture& source)
{
}

CGERenderToTexture& CGERenderToTexture::operator=(const CGERenderToTexture& source)
{
	return *this;
}

CGERenderToTexture::~CGERenderToTexture()
{
}

ID3D11ShaderResourceView* CGERenderToTexture::GetShaderResourceView()
{
	return this->m_pShaderResourceView;
}

void CGERenderToTexture::GetOrthoMatrix(D3DXMATRIX& _out_ortho_matrix)
{
	_out_ortho_matrix = this->m_orthoMatrix;
}

float CGERenderToTexture::GetWidth()
{
	return this->m_textureWidth;
}

float CGERenderToTexture::GetHeight()
{
	return this->m_textureHeight;
}

bool CGERenderToTexture::Initialize(ID3D11Device* pDevice, int textureWidth, int textureHeight)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	result = pDevice->CreateTexture2D(&textureDesc, NULL, &this->m_pRenderTargetTexture);
	if( FAILED(result) )
		return false;

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = pDevice->CreateRenderTargetView(this->m_pRenderTargetTexture, &renderTargetViewDesc, &this->m_pRenderTargetView);
	if( FAILED(result) )
		return false;

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(this->m_pRenderTargetTexture, &shaderResourceViewDesc, &this->m_pShaderResourceView);
	if( FAILED(result) )
		return false;

	return true;
}

bool CGERenderToTexture::Initialize(ID3D11Device* pDevice, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{	
	HRESULT result;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	this->m_textureWidth = (float)textureWidth;
	this->m_textureHeight = (float)textureHeight;

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	result = pDevice->CreateTexture2D(&textureDesc, NULL, &this->m_pRenderTargetTexture);
	if( FAILED(result) )
		return false;

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = pDevice->CreateRenderTargetView(this->m_pRenderTargetTexture, &renderTargetViewDesc, &this->m_pRenderTargetView);
	if( FAILED(result) )
		return false;

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(this->m_pRenderTargetTexture, &shaderResourceViewDesc, &this->m_pShaderResourceView);
	if( FAILED(result) )
		return false;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = pDevice->CreateTexture2D(&depthBufferDesc, NULL, &this->m_pDepthStencilBuffer);
	if( FAILED(result) )
		return false;

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = pDevice->CreateDepthStencilView(this->m_pDepthStencilBuffer, &depthStencilViewDesc, &this->m_pDepthStencilView);
	if( FAILED(result) )
		return false;

	this->m_viewPort.Width = (float)textureWidth;
	this->m_viewPort.Height = (float)textureHeight;
	this->m_viewPort.MinDepth = 0.0f;
	this->m_viewPort.MaxDepth = 1.0f;
	this->m_viewPort.TopLeftX = 0.0f;
	this->m_viewPort.TopLeftY = 0.0f;

	D3DXMatrixPerspectiveFovLH(&this->m_projectionMatrix, ((float)D3DX_PI / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);
	D3DXMatrixOrthoLH(&this->m_orthoMatrix, (float)textureWidth, (float)textureHeight, screenNear, screenDepth);

	return true;
}

void CGERenderToTexture::Shutdown()
{
	::ReleaseWithoutDel<ID3D11ShaderResourceView*>(this->m_pShaderResourceView);
	::ReleaseWithoutDel<ID3D11RenderTargetView*>(this->m_pRenderTargetView);
	::ReleaseWithoutDel<ID3D11Texture2D*>(this->m_pRenderTargetTexture);
}

void CGERenderToTexture::SetRenderTarget(ID3D11DeviceContext* pDeviceContext, ID3D11DepthStencilView* pDepthStencilView)
{
	pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetView, pDepthStencilView);
}

void CGERenderToTexture::SetRenderTarget(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetView, this->m_pDepthStencilView);
	pDeviceContext->RSSetViewports(1, &this->m_viewPort);
}

void CGERenderToTexture::SetRenderTarget_viewport(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->OMSetRenderTargets(1, &this->m_pRenderTargetView, this->m_pDepthStencilView);
	pDeviceContext->RSSetViewports(1, &this->m_viewPort);
}

void CGERenderToTexture::ClearRenderTarget(ID3D11DeviceContext* pDeviceContext, ID3D11DepthStencilView* pDepthStencilView,
										   float r, float g, float b, float a)
{
	float textureColor[4];

	textureColor[0] = r;
	textureColor[1] = g;
	textureColor[2] = b;
	textureColor[3] = a;

	pDeviceContext->ClearRenderTargetView(this->m_pRenderTargetView, textureColor);
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CGERenderToTexture::ClearRenderTarget(ID3D11DeviceContext* pDeviceContext, float r, float g, float b, float a)
{
	float textureColor[4];

	textureColor[0] = r;
	textureColor[1] = g;
	textureColor[2] = b;
	textureColor[3] = a;

	pDeviceContext->ClearRenderTargetView(this->m_pRenderTargetView, textureColor);
	pDeviceContext->ClearDepthStencilView(this->m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CGERenderToTexture::ClearRenderTarget_viewport(ID3D11DeviceContext* pDeviceContext, float r, float g, float b, float a)
{
	float textureColor[4];

	textureColor[0] = r;
	textureColor[1] = g;
	textureColor[2] = b;
	textureColor[3] = a;

	pDeviceContext->ClearRenderTargetView(this->m_pRenderTargetView, textureColor);
	pDeviceContext->ClearDepthStencilView(this->m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}