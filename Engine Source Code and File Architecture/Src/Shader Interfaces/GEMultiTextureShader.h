/* 
 * File name - GEMultiTexture.h
 * Author - CuiXue
 * Updated date - 2013.3.1
 * Desc -
 *
 */
#ifndef _GE_MULTITEXTURE_H_
#define _GE_MULTITEXTURE_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#endif

#include <fstream>
#include "Utility.h"
using namespace std;

/*
 * Class name - CGEMultiTextureShader
 * Author - CuiXue
 * Updated date - 2013.3.1
 * Desc -
 *
 */
class CGEMultiTextureShader
{
public:
	// Interfaces
	CGEMultiTextureShader();
	CGEMultiTextureShader(const CGEMultiTextureShader&);
	CGEMultiTextureShader& operator=(const CGEMultiTextureShader&);
	~CGEMultiTextureShader();

public:
	// Functions
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

private:
	// Kernels
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*	m_pPixelShader;
	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pMatrixBuffer;
	ID3D11Buffer*		m_pPSConstantBuffer;
	ID3D11SamplerState* m_pSamplerState;

private:
	struct _constant_buffer_ps
	{
#ifdef __cplusplus
		_constant_buffer_ps()
		{
			num_of_textures = 0;
		};
#endif
		int num_of_textures;
	};
};

#endif