/*
 * File name GEPixelSSAOShader.cpp
 * Author - CuiXue
 * Updated date - 2013.4.3
 * Desc - 
 *
 */

#include "GEPixelSSAOShader.h"

CGEPixelSSAOShader::CGEPixelSSAOShader()
{
	this->m_pPixelShader = 0;
	this->m_pVertexShader = 0;
}

CGEPixelSSAOShader::CGEPixelSSAOShader(const CGEPixelSSAOShader& source)
{
}

CGEPixelSSAOShader& CGEPixelSSAOShader::operator=(const CGEPixelSSAOShader& source)
{
	return *this;
}

CGEPixelSSAOShader::~CGEPixelSSAOShader()
{
}

bool CGEPixelSSAOShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shaders(pDevice, hWnd, L"../Include/Shaders/SSAO.vs",
													  L"../Include/Shaders/SSAO.ps");
	if( !result) 
		return false;

	return true;
}

void CGEPixelSSAOShader::Shutdown()
{
	this->_shutdown();
}