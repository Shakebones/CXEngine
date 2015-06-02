/*
 * File name - GETransparencyShader.h
 * Author - CuiXue
 * Updated date - 2013.4.8
 * Desc - 
 *
 */
#ifndef _GE_TRANSPARENCY_SHADER_H_
#define _GE_TRANSPARENCY_SHADER_H_

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
 * Class name - CGETransparencyShader
 * Author - CuiXue
 * Updated date - 2013.4.8
 * Desc - 
 *
 */
class CGETransparencyShader
{
public:
	CGETransparencyShader();
	CGETransparencyShader(const CGETransparencyShader&);
	CGETransparencyShader& operator=(const CGETransparencyShader&);
	~CGETransparencyShader();

public:
	// Interfaces
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, float);
	
private:
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, float);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	typedef struct _transparencyBufferType
	{
		float transparency;
		D3DXVECTOR3 padding;
	}TransparencyBufferType;

private:
	ID3D11VertexShader*	m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pMatrixBuffer;
	ID3D11Buffer*		m_pTransparency;
	ID3D11SamplerState* m_pSamplerState;
};

#endif