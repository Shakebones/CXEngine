/*
 * File name - GETexture.h
 * Author - CuiXue
 * Updated date - 2012.12.10
 * Desc - 
 *
 */
#ifndef _GE_TEXTURE_H_
#define _GE_TEXTURE_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX11tex.h>
#include "Utility.h"
#endif


/*
 * Class name - CGETexture
 * Author - CuiXue
 * Updated date - 2012.12.10
 * Desc - 
 *
 */
class CGETexture
{
public:
	CGETexture();
	CGETexture(const CGETexture& source);
	CGETexture& operator=(const CGETexture& source);
	~CGETexture();

public:
	// Interfaces
	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_pTexture;
};

#endif