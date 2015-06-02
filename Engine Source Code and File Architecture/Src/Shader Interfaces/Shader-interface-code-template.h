/*--------------------------------------------------------------------------
 * File name - Shader-interface-code-template.h
 * Author - CuiXue
 * Updated date - 2013.7.11
 * Desc - 
 *
 *--------------------------------------------------------------------------*/
#pragma once

#ifndef _GE_SHADER_INTERFACE_CODE_TEMPLATE_H_
#define _GE_SHADER_INTERFACE_CODE_TEMPLATE_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*--------------------------------------
 * Class name - Shader_interface
 * Author - CuiXue
 * Updated date - 2013.7.11
 * Desc - 
 *
 *--------------------------------------*/
class Shader_interface
{
private:
	// Private structures

public:
	// Constructors
	Shader_interface();
	Shader_interface(const Shader_interface&);
	Shader_interface& operator=(const Shader_interface&);
	~Shader_interface();

public:
	// Interfaces
	bool Initialize(ID3D11Device* pDevice, HWND hWnd);
	void Shutdown();
	bool SetParams(/* Modify params */);
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount);

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

	// Buffers

	// Shader interfaces need some buffers for shader file veriable-communication.
	// Like matrix buffer for transform vertices.
	// Ex1 : ID3D11Buffer* m_pMatrixBuffer;
	// Ex2 : ID3D11Buffer* m_pPerturbBuffer;
};

#endif