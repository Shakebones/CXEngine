/*
 * File name - GEFontShader.h
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc -
 *
 */
#ifndef _GE_FONT_SHADER_H_
#define _GE_FONT_SHADER_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGEFontShader
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc -
 *
 */
class CGEFontShader
{
public:
	// Interfaces
	CGEFontShader();
	CGEFontShader(const CGEFontShader&);
	CGEFontShader& operator=(const CGEFontShader&);
	~CGEFontShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int , D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

private:
	// Kernels
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*	m_pPixelShader;
	ID3D11InputLayout*	m_pLayout;
	ID3D11Buffer*		m_pConstantBuffer;
	ID3D11SamplerState* m_pSamplerState;

	ID3D11Buffer*		m_pFontPixelBuffer;
};

#endif