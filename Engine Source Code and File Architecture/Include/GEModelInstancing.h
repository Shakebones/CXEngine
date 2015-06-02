/*--------------------------------------------------------------------------
 * File name - GEModelInstancing.h
 * Author - CuiXue
 * Updated date - 2015.2.1
 * Desc - 
 *
 *--------------------------------------------------------------------------*/
#pragma once

#ifndef _GE_MODEL_INSTANCING_H_
#define _GE_MODEL_INSTANCING_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*--------------------------------------
 * Class name - CGEModelInstancing
 * Author - CuiXue
 * Updated date - 2015.2.1
 * Desc - 
 *
 *--------------------------------------*/
class CGEModelInstancing
{
private:
	// Private structures
	struct _vertex_type
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 texture;
	}VertexType;

	// Adapt to specific change
	struct _directx_11_instancing_type
	{
		D3DXVECTOR3 position;
	};

public:
	// Constructors
	CGEModelInstancing();
	CGEModelInstancing(const CGEModelInstancing&);
	CGEModelInstancing& operator=(const CGEModelInstancing&);
	~CGEModelInstancing();

public:
	// Access functions
	int GetVertexCount();
	int GetInstanceCount();

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
	ID3D11Buffer* m_instanceBuffer;
	int m_vertexCount;
	// Shader interfaces need some buffers for shader file veriable-communication.
	// Like matrix buffer for transform vertices.
	// Ex1 : ID3D11Buffer* m_pMatrixBuffer;
	// Ex2 : ID3D11Buffer* m_pPerturbBuffer;
};

#endif