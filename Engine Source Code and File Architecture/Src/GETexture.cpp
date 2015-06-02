/*
 * File name GETexture.cpp
 * Author - CuiXue
 * Updated date - 2012.12.10
 * Desc -
 *
 */
#include "GETexture.h"

CGETexture::CGETexture()
{
	this->m_pTexture = 0;
}

CGETexture::CGETexture(const CGETexture& source)
{
}

CGETexture& CGETexture::operator=(const CGETexture& source)
{
	return *this;
}

CGETexture::~CGETexture()
{
}

bool CGETexture::Initialize(ID3D11Device* pDevice, WCHAR* filename)
{
	HRESULT result;

	if( NULL == filename )
		return false;

	result = ::D3DX11CreateShaderResourceViewFromFile(pDevice, filename, NULL,
		NULL, &this->m_pTexture, NULL);
	if( FAILED(result) )
		return false;

	return true;
}

void CGETexture::Shutdown()
{
	::ReleaseWithoutDel<ID3D11ShaderResourceView*>(this->m_pTexture);
}

ID3D11ShaderResourceView* CGETexture::GetTexture()
{
	return this->m_pTexture;
}

