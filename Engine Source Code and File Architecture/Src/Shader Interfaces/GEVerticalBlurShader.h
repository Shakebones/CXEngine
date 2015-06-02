/*--------------------------------------------------------------------------
 * File name - GEVerticalBlurShader.h
 * Author - CuiXue
 * Updated date - 2013.7.18
 * Desc - 
 *
 *--------------------------------------------------------------------------*/
#pragma once

#ifndef _GE_VERTICAL_BLURE_SHADER_H_
#define _GE_VERTICAL_BLURE_SHADER_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*--------------------------------------
 * Class name - CGEVerticalBlurShader
 * Author - CuiXue
 * Updated date - 2013.7.11
 * Desc - 
 *
 *--------------------------------------*/
class CGEVerticalBlurShader
{
private:
	struct ScreenHeightBufferType
	{
		float screenHeight;
		D3DXVECTOR3 padding;
	};

public:
	// Constructors
	CGEVerticalBlurShader();
	CGEVerticalBlurShader(const CGEVerticalBlurShader&);
	CGEVerticalBlurShader& operator=(const CGEVerticalBlurShader&);
	~CGEVerticalBlurShader();

public:
	// Interfaces
	bool Initialize(ID3D11Device* pDevice, HWND hWnd);
	void Shutdown();
	bool SetParams(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);

private:
	// Kernels
	bool _initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename /* May need more other shaders like hull shader*/);
	void _shutdown();
	void _output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);
	bool _render(ID3D11DeviceContext* pDeviceContext, int indexCount);

private:
	// Members
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputlayout;
	ID3D11SamplerState* m_pBasicSampler;

	// Buffers
	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pScreenHeightBuffer;

	// Shader interfaces need some buffers for shader file veriable-communication.
	// Like matrix buffer for transform vertices.
	// Ex1 : ID3D11Buffer* m_pMatrixBuffer;
	// Ex2 : ID3D11Buffer* m_pPerturbBuffer;
};

#endif