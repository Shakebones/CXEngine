/*
 * File name - GEPointLightShader.h
 * Author - CuiXue
 * Updated date - 2013.4.28
 * Desc - 
 *
 */
#ifndef _GE_POINT_LIGHT_SHADER_H_
#define _GE_POINT_LIGHT_SHADER_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

#define NUM_LIGHTS 4

/*
 * Class name - CGEMultiPointLightShader
 * Author - CuiXue
 * Updated date - 2013.4.28
 * Desc - 
 *
 */
class CGEMultiPointLightShader
{
	// Structures
private:

	// Light position
	typedef struct _light_position
	{
#ifdef __cplusplus
		_light_position()
		{
			for(int i = 0; i != NUM_LIGHTS; ++i)
			{
				lightPosition[i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
#endif
		D3DXVECTOR4 lightPosition[NUM_LIGHTS];
	}LightPositionBufferType;

	// Light color
	typedef struct _light_color
	{
#ifdef __cplusplus
		_light_color()
		{
			for(int i = 0; i != NUM_LIGHTS; ++i)
			{
				lightColor[i] = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
#endif
		D3DXVECTOR4 lightColor[NUM_LIGHTS];
	}LightColorBufferType;

	// Constructors
public:
	CGEMultiPointLightShader();
	CGEMultiPointLightShader(const CGEMultiPointLightShader&);
	CGEMultiPointLightShader& operator=(const CGEMultiPointLightShader&);
	~CGEMultiPointLightShader();

	// Interfaces
public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int,
		D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, int,
		D3DXVECTOR4[], D3DXVECTOR4[]);

	// Kernels
private:
	bool _initialize_shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, int, D3DXVECTOR4[], D3DXVECTOR4[]);
	void _render_shaders(ID3D11DeviceContext*, int);

	// Members
private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11Buffer*		m_pLightColor;
	ID3D11Buffer*		m_pLightPosition;
	ID3D11Buffer*		m_pMatrixBuffer;
};
#endif