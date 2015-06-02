/*
 * File name - GEGlassShader.h
 * Author - CuiXue
 * Updated date - 2013.5.9
 * Desc - 
 *
 */
#ifndef _GE_GLASS_SHADER_H_
#define _GE_GLASS_SHADER_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX11async.h>

#include "Utility.h"
#endif

#include <fstream>

using namespace std;

/*
 * Class name - CGEGlassShader
 * Author - CuiXue
 * Updated date - 2013.5.9
 * Desc - 
 *
 */
class CGEGlassShader
{
private:
	struct GlassFactorBufferType
	{
		float refractionScale;
		D3DXVECTOR3 padding;
	};
	// Constructors
public:
	CGEGlassShader();
	CGEGlassShader(const CGEGlassShader&);
	CGEGlassShader& operator=(const CGEGlassShader&);
	~CGEGlassShader();

	// Interfaces
public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX,	D3DXMATRIX, D3DXMATRIX, 
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float);

	// Kernels
	bool _initialize_shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float);
	void _render_shaders(ID3D11DeviceContext*, int);

	// Memebers
private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pGlassBuffer;
};
#endif