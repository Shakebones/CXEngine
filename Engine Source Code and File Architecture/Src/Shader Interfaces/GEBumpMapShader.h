/*
 * File name - GEBumpMapShader.h
 * Author - CuiXue
 * Updated date - 2013.3.11
 * Desc - 
 *
 */
#ifndef _GE_BUMPMAP_SHADER_H_
#define _GE_BUMPMAP_SHADER_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;
/* 
 * Class name - CGEBumpMapShader
 * Author - CuiXue
 * Updated date - 2013.3.11
 * Desc - 
 *
 */
class CGEBumpMapShader
{
public:
	CGEBumpMapShader();
	CGEBumpMapShader(const CGEBumpMapShader&);
	CGEBumpMapShader& operator=(const CGEBumpMapShader&);
	~CGEBumpMapShader();

public:
	// Interfaces
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4,
		D3DXVECTOR4, float);
	
private:
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR4, float);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader*	m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pMatrixBuffer;
	ID3D11Buffer*		m_pLightBuffer;
	ID3D11Buffer*		m_pCameraBuffer;
	ID3D11SamplerState* m_pSamplerState;
};

#endif