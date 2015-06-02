/*
 * File name - GEReflectionShader.h
 * Author - CuiXue
 * Updated date - 2013.4.9
 * Desc - 
 *
 */
#ifndef _GE_REFLECTION_SHADER_H_
#define _GE_REFLECTION_SHADER_H_

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
 * Class name - CGEReflectionShader
 * Author - CuiXue
 * Updated date - 2013.4.9
 * Desc - 
 *
 */
class CGEReflectionShader
{
public:
	CGEReflectionShader();
	CGEReflectionShader(const CGEReflectionShader&);
	CGEReflectionShader& operator=(const CGEReflectionShader&);
	~CGEReflectionShader();

public:
	// Interfaces
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXMATRIX);
	
private:
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXMATRIX);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	typedef struct _reflection_matrix
	{
		D3DXMATRIX reflectionMat;
	}ReflectionMatrixBufferType;

private:
	ID3D11VertexShader*	m_pVertexShader;
	ID3D11PixelShader*  m_pPixelShader;
	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pMatrixBuffer;
	ID3D11Buffer*		m_pReflectionMatrixBuffer;
	ID3D11SamplerState* m_pSamplerState;
};

#endif