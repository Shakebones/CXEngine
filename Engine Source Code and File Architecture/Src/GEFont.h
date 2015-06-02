/*
 * File name - GEFont.h
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc - 
 * 
 */
#ifndef _GE_FONT_H_
#define _GE_FONT_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX10math.h>
#include "Utility.h"
#include "GETexture.h"
#endif

#include <fstream>
using namespace std;


/*
 * Class name - CGEFont
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc -
 *
 */
class CGEFont
{
public:
	// Interfaces
	CGEFont();
	CGEFont(const CGEFont&);
	CGEFont& operator=(const CGEFont&);
	~CGEFont();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);

private:
	// Kernels
	bool _load_font(char*);
	void _release_font_data();
	bool _load_texture(ID3D11Device*, WCHAR*);
	void _release_texture();

private:
	GEFontType*	m_pFont;
	CGETexture*	m_pTexture;
};
#endif