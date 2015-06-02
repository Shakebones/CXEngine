/*
 * File name - GEFadeShader.h
 * Author - CuiXue
 * Updatd date - 2013.4.15
 * Desc - 
 *
 */
#ifndef _GE_FADE_SHADER_H_
#define _GE_FADE_SHADER_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGEFadeShader
 * Author - CuiXue
 * Updated date - 2013.4.15
 * Desc - 
 *
 */
class CGEFadeShader
{
	// Constructor
public:
	CGEFadeShader();
	CGEFadeShader(const CGEFadeShader&);
	CGEFadeShader& operator=(const CGEFadeShader&);
	~CGEFadeShader();

	// Interfaces
public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, 
		D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
		float);

	// Kernels
private:
	bool _initialize_shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
							ID3D11ShaderResourceView*,
							float);
	void _render_shader(ID3D11DeviceContext*, int);

	// Members
private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*	m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pFadeFactorBuffer;

	typedef struct _fade_buffer_type
	{
#ifdef __cplusplus
		_fade_buffer_type()
		{
			fadeFactor = 0.0f;
		}
#endif
		float fadeFactor;
		D3DXVECTOR3 padding;
	}FadeBufferType;
};

#endif