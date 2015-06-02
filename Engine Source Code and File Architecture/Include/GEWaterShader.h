/*
 * File name - GEWater.h
 * Author - CuiXue
 * Updated date - 2013.4.21
 * Desc - 
 *
 */
#ifndef _GE_WATER_H_
#define _GE_WATER_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGEWaterShader
 * Author - CuiXue
 * Updated date - 2013.4.21
 * Desc - 
 *
 */
class CGEWaterShader
{
	// Structure
	typedef struct _water_type
	{
#ifdef __cplusplus
		_water_type()
		{
			reflection_refraction_scale = 0.0f;
			translation = 0.0f;
		}
#endif

		float reflection_refraction_scale;
		float translation;
		D3DXVECTOR2 padding;
	}WaterType, *pWaterType;

	typedef struct _reflection_buffer_type
	{
#ifdef __cplusplus
		_reflection_buffer_type()
		{
		}
#endif

		D3DXMATRIX reflectionMatrix;
	}ReflectionBufferType;

private:
	// Constructors
public:
	CGEWaterShader();
	CGEWaterShader(const CGEWaterShader&);
	CGEWaterShader& operator=(const CGEWaterShader&);
	~CGEWaterShader();

	// Interfaces
public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, 
				D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
				ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
				float, float, LightBufferType*, D3DXVECTOR3);

	// Kernels
private:
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		float, float, LightBufferType*, D3DXVECTOR3);

	void _render_shader(ID3D11DeviceContext*, int);

	// Members
private:
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11SamplerState*		m_pSamplerState;
	ID3D11Buffer*			m_pMatrixBuffer;
	ID3D11Buffer*			m_pWaterBuffer;
	ID3D11Buffer*			m_pLightBuffer;
	ID3D11Buffer*			m_pCameraBuffer;
	ID3D11Buffer*			m_pReflectionBuffer;
};
#endif