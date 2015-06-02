/*
 * File name - GEGraphics.cpp
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc
 *
 * Definition
 */
#include "GEGraphics.h"

CGEGraphics::CGEGraphics()
{
	this->m_GE_D3D_Renderer = 0;
	this->m_pRenderToTexture = 0;
	this->m_pShaderDebugger = 0;
}

CGEGraphics::CGEGraphics(const CGEGraphics& source)
{
}

CGEGraphics& CGEGraphics::operator=(const CGEGraphics& source)
{
	return *this;
}

CGEGraphics::~CGEGraphics()
{
}

bool CGEGraphics::Initialize(bool is_full_screen, bool vsyncEnable, int width, int height, HWND hWnd)
{
	bool result;
	this->m_GE_D3D_Renderer = new CGE_D3D_Renderer;
	if( !this->m_GE_D3D_Renderer )
		return false;

	result = this->m_GE_D3D_Renderer->Initialize(is_full_screen, width, height, vsyncEnable, hWnd,
		SCREEN_DEPTH, SCREEN_NEAR);

	if( !result )
	{
		::MessageBox(hWnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	/*
	 * Initialize some components.
	 * Camera.
	 * ColorShader.
	 * ...
	 */

	this->m_pRenderToTexture = new CGERenderToTexture;
	if( !this->m_pRenderToTexture )
		return false;

	result = this->m_pRenderToTexture->Initialize(this->m_GE_D3D_Renderer->GetDevicePointer(), width, height);
	if( !result )		
	{
		MessageBox(hWnd, L"Could not initialize 'RenderToTexture coms'", L"Error", MB_OK);
		return false;
	}

	return true;
}

void CGEGraphics::Shutdown()
{
	::Shutdown<CGERenderToTexture*>(this->m_pRenderToTexture);
	::Shutdown<CGEShaderDebugger*>(this->m_pShaderDebugger);
	::Shutdown<CGE_D3D_Renderer*>(this->m_GE_D3D_Renderer);
}

bool CGEGraphics::BeginRender(D3DXCOLOR _clear_color)
{
	bool result;
	result = this->_render(_clear_color);
	if( !result )
		return false;

	return true;
}

bool CGEGraphics::_render(D3DXCOLOR& _clear_backbuffer_color)
{
	// Clear back buffer ready to render
	this->m_GE_D3D_Renderer->BeginScene(_clear_backbuffer_color);
	return true;
}

bool CGEGraphics::_end_render()
{
	this->m_GE_D3D_Renderer->EndScene();
	return true;
}

bool CGEGraphics::EndRender()
{
	bool result;
	result = this->_end_render();
	if( !result )
		return false;

	return true;
}

ID3D11Device* CGEGraphics::GetD3DDevice()
{
	return this->m_GE_D3D_Renderer->GetDevicePointer();
}

CGE_D3D_Renderer* CGEGraphics::GetD3DObj()
{
	return this->m_GE_D3D_Renderer;
}

void CGEGraphics::WireFrameMode()
{
	this->m_GE_D3D_Renderer->SetWireFrameMode();
}

void CGEGraphics::SolidMode()
{
	this->m_GE_D3D_Renderer->SetSolidMode();
}

bool CGEGraphics::_pre_render_to_texture(float r, float g, float b, float a)
{
	this->m_pRenderToTexture->SetRenderTarget(this->m_GE_D3D_Renderer->GetDeviceContextPointer(), 
											  this->m_GE_D3D_Renderer->GetDepthStencilView());

	this->m_pRenderToTexture->ClearRenderTarget(this->m_GE_D3D_Renderer->GetDeviceContextPointer(),
												this->m_GE_D3D_Renderer->GetDepthStencilView(), r, g, b, a);

	return true;
}

bool CGEGraphics::_post_render_to_texture()
{
	this->m_GE_D3D_Renderer->SetBackBufferRenderTarget();

	return true;
}

bool CGEGraphics::PreProcessSceneToTexture(float r, float g, float b, float a)
{
	bool result;

	result = this->_pre_render_to_texture(r, g, b, a);
	if( !result )
		return false;

	return true;
}

bool CGEGraphics::PostProcessSceneToTexture()
{
	bool result;

	result = this->_post_render_to_texture();
	if( !result )
		return false;

	return true;
}

bool CGEGraphics::CreateShaderDebugger(ID3D11Device* pDevice, HWND hWnd, int screenWidth, int screenHeight, int width, int height)
{
	bool result;

	this->m_pShaderDebugger = new CGEShaderDebugger;
	if( !this->m_pShaderDebugger )
	{
		MessageBox(hWnd, L"Could not initialize shader debugger", L"Error", MB_OK);
		return false;
	}

	result = this->m_pShaderDebugger->Initialize(pDevice, screenWidth, screenHeight, width, height);
	if( !result )
	{
		MessageBox(hWnd, L"Could not initialize shader debugger", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool CGEGraphics::RenderShaderDebuggerWindow(int posX, int posY)
{
	bool result;

	result = this->m_pShaderDebugger->Render(this->m_GE_D3D_Renderer->GetDeviceContextPointer(),
		posX, posY);
	if( !result )
		return false;

	return true;
}

void CGEGraphics::TurnOnZBuffer()
{
	this->m_GE_D3D_Renderer->TurnZBufferOn();
}

void CGEGraphics::TurnOffZBuffer()
{
	this->m_GE_D3D_Renderer->TurnZBufferOff();
}

void CGEGraphics::TurnOnAlphaBlending()
{
	this->m_GE_D3D_Renderer->TurnAlphaBlendingOn();
}

void CGEGraphics::TurnOffAlphaBlending()
{
	this->m_GE_D3D_Renderer->TurnAlphaBlendingOff();
}

CGEShaderDebugger* CGEGraphics::Get_ShaderDebugger_Module()
{
	return this->m_pShaderDebugger;
}

CGERenderToTexture* CGEGraphics::Get_RenderToTexture_Module()
{
	return this->m_pRenderToTexture;
}

ID3D11DeviceContext* CGEGraphics::Get_D3D11DeviceContext()
{
	return this->m_GE_D3D_Renderer->GetDeviceContextPointer();
}

