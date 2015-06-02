/*
 * File name - GELightMapShader.h
 * Author - CuiXue
 * Updated date - 2013.3.7
 * Desc - 
 *
 */
#ifndef _GE_LIGHT_MAP_SHADER_H_
#define _GE_LIGHT_MAP_SHADER_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#include "Utility.h"
#endif

#include <fstream>

/*
 * Class name - CGELightMapShader
 * Author - CuiXue
 * Updated date - 2013.3.7
 * Desc - 
 *
 */
class CGELightMapShader
{
public:
	CGELightMapShader();
	CGELightMapShader(const CGELightMapShader&);
	CGELightMapShader& operator=(const CGELightMapShader&);
	~CGELightMapShader();

public:
	// Interfaces
	// Functions
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

private:
	// Kernels
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	bool _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*	m_pPixelShader;
	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pMatrixBuffer;
	ID3D11SamplerState* m_pSamplerState;
};

#endif