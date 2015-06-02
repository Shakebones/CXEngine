/*
 * File name - GERefractionShader.h
 * Author - CuiXue
 * Updated date - 2013.4.22
 * Desc - 
 *
 */
#ifndef _GE_REFRACTION_H_
#define _GE_REFRACTION_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGERefractionShader
 * Author - CuiXue
 * Updated date - 2013.4.21
 * Desc - 
 *
 */
class CGERefractionShader
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

	typedef struct _clip_buffer_type
	{
#ifdef __cplusplus
		_clip_buffer_type()
		{
		}
#endif

		D3DXVECTOR4 clipPlane;
	}ClipPlaneBufferType;

private:
	// Constructors
public:
	CGERefractionShader();
	CGERefractionShader(const CGERefractionShader&);
	CGERefractionShader& operator=(const CGERefractionShader&);
	~CGERefractionShader();

	// Interfaces
public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, 
				int indexCount,				// Index count.
				D3DXMATRIX,					// world.
				D3DXMATRIX,					// view.
				D3DXMATRIX,					// projection
				ID3D11ShaderResourceView*,	// Shader resources.
				D3DXVECTOR3,				// light direction.
				D3DXVECTOR4,				// ambient color.
				D3DXVECTOR4,				// diffuse color.
				D3DXVECTOR4,				// specular color.
				D3DXVECTOR3,				// camera position.
				float,						// specularPower.
				D3DXVECTOR4);				// Clip Plane.		

	// Kernels
private:
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
							D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float, D3DXVECTOR4);

	void _render_shader(ID3D11DeviceContext*, int);

	// Members
private:
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11SamplerState*		m_pSamplerState;
	ID3D11Buffer*			m_pMatrixBuffer;
	ID3D11Buffer*			m_pLightBuffer;
	ID3D11Buffer*			m_pClipPlaneBuffer;
	ID3D11Buffer*			m_pCameraBuffer;
};
#endif