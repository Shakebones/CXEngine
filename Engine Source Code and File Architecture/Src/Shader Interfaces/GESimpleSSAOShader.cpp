/*
 * File name - GESimpleSSAOShader.cpp
 * Author - CuiXue
 * Updated date - 2013.3.12
 * Desc - 
 *
 */
#include "GESimpleSSAOSahder.h"

CGESimpleSSAOShader::CGESimpleSSAOShader()
{
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pInputLayout = 0;
	this->m_pParamsBuffer = 0;
	this->m_pPositionSamplerState = 0;
	this->m_pNormSamplerState = 0;
	this->m_pRandomSamplerState = 0;
}

CGESimpleSSAOShader::CGESimpleSSAOShader(const CGESimpleSSAOShader& source)
{
}

CGESimpleSSAOShader& CGESimpleSSAOShader::operator=(const CGESimpleSSAOShader& soruce)
{
	return *this;
}

bool CGESimpleSSAOShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shader(pDevice, hWnd, L"../Include/Shader/SimpleSSAO.vs",
													 L"../Include/Shader/SimpleSSAO.ps");
	if( !result )
		return false;

	return true;
}

void CGESimpleSSAOShader::Shutdown()
{
	this->_shutdown();
}

bool CGESimpleSSAOShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, GE_SSAO_Params* pParams,
								 D3DXMATRIX world,
								 D3DXMATRIX view,
								 D3DXMATRIX proj)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, indexCount, world, view, proj,
		pParams);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}