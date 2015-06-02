/*--------------------------------------------------------------------------
 * File name - GEFFTOceanSurface.h
 * Author - CuiXue
 * Updated date - 2015.2.12
 * Desc - 
 *
 *--------------------------------------------------------------------------*/
#pragma once

#ifndef _GE_CGEFFTOceanSurface_CODE_TEMPLATE_H_
#define _GE_CGEFFTOceanSurface_CODE_TEMPLATE_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*--------------------------------------
 * Class name - CGEFFTOceanSurface
 * Author - CuiXue
 * Updated date - 2013.7.11
 * Desc - 
 *
 *--------------------------------------*/
class CGEFFTOceanSurface
{
private:
	// Private structures

public:
	// Constructors
	CGEFFTOceanSurface();
	CGEFFTOceanSurface(const CGEFFTOceanSurface&);
	CGEFFTOceanSurface& operator=(const CGEFFTOceanSurface&);
	~CGEFFTOceanSurface();

public:
	// Interfaces
	bool Initialize(ID3D11Device* pDevice, HWND hWnd);
	void Shutdown();
	bool SetParams(/* Modify params */);
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount);

private:
	// 

private:
	// Kernels
	bool _initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename /* May need more other shaders like hull shader*/);
	void _shutdown();
	void _output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename);
	bool _set_shader_params(/* Modify params */);
	bool _render(ID3D11DeviceContext* pDeviceContext, int indexCount);

private:
	// Members
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputlayout;
	ID3D11SamplerState* m_pBasicSampler;

private:
	// Ocean params
	float m_wind_velocity;
	float m_height_field_size;
	float m_suppresed_height_field_size;
	float m_gravity;
	D3DXVECTOR3 m_wind_direction;
	D3DXVECTOR3 m_fft_resolution;


	// Buffers

	// Shader interfaces need some buffers for shader file veriable-communication.
	// Like matrix buffer for transform vertices.
	// Ex1 : ID3D11Buffer* m_pMatrixBuffer;
	// Ex2 : ID3D11Buffer* m_pPerturbBuffer;
};

#endif