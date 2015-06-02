/*
 * File name - GEColorShader.h
 * Author - CuiXue
 * Updated date - 2012.12.12
 * Desc - 
 *
 */
#ifndef _GE_COLOR_SHADER_H_
#define _GE_COLOR_SHADER_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGEColorShader
 * Author - CuiXue
 * Updated date - 2012.12.12
 * Desc -
 *
 */
class CGEColorShader
{
public:
	CGEColorShader();
	CGEColorShader(const CGEColorShader&);
	CGEColorShader& operator=(const CGEColorShader&);
	~CGEColorShader();

public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pLayout;
	ID3D11Buffer*			m_pMatrixBuffer;
};

#endif