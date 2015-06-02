/*
 * File name - GELightShader.h
 * Author - CuiXue
 * Updated date - 2012.12.26
 * Desc -
 *
 */
#ifndef _GE_LIGHT_SHADER_H_
#define _GE_LIGHT_SHADER_H_

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
 * Class name - CGELightShader
 * Author - CuiXue
 * Updated date - 2012.12.26
 * Desc -
 *
 */
class CGELightShader
{
public:
	CGELightShader();
	CGELightShader(const CGELightShader&);
	CGELightShader& operator=(const CGELightShader&);
	~CGELightShader();

public:
	// Interfaces
	bool Initialize(ID3D11Device*, HWND);
	bool Initialize_DX11Instance(ID3D11Device*, HWND);
	bool Initialize_Tessellation(ID3D11Device*, HWND);
	void Shutdown();

	// Mother fucking params.
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
		float);						// specularPower.

	bool Render(ID3D11DeviceContext*, 
		int vertexCount,				// Index count.
		int,						// Instance count.
		D3DXMATRIX,					// world.
		D3DXMATRIX,					// view.
		D3DXMATRIX,					// projection
		ID3D11ShaderResourceView*,	// Shader resources.
		D3DXVECTOR3,				// light direction.
		D3DXVECTOR4,				// ambient color.
		D3DXVECTOR4,				// diffuse color.
		D3DXVECTOR4,				// specular color.
		D3DXVECTOR3,				// camera position.
		float);						// specularPower.

	bool RenderTessellation(ID3D11DeviceContext*,
		int indexCount,				// Index count.
		ID3D11ShaderResourceView**,	// Shader resources.
		GE_UpdateConstantBuffer*,
		GE_Material_ConstantBufferType*);
private:
	// Kernels;
	bool _initialize_shader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool _initialize_shader_dx11_instance(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool _initialize_shader_tessellation(ID3D11Device*, HWND, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
							D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);
	bool _set_shader_params(ID3D11DeviceContext*, ID3D11ShaderResourceView**, GE_UpdateConstantBuffer*, GE_Material_ConstantBufferType*);
	void _render_shader(ID3D11DeviceContext*, int);
	void _render_shader_tessellation(ID3D11DeviceContext*, int);
	void _render_shader(ID3D11DeviceContext*, int, int);

private:
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11HullShader*		m_pHullShader;
	ID3D11DomainShader*		m_pDomainShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11SamplerState*		m_pSampleState;
	ID3D11Buffer*			m_pMatrixBuffer;
	ID3D11Buffer*			m_pLightBuffer;
	ID3D11Buffer*			m_pCameraBuffer;
	ID3D11Buffer*			m_pTessellationBuffer;
	ID3D11Buffer*			m_pUpdateConstantBuffer;
	ID3D11Buffer*			m_pMaterialConstantBuffer;
};

#endif