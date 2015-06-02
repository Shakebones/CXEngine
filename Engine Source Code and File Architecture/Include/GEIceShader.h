/*
 * File name - 
 * Author - 
 * Updated date - 
 * Desc - 
 *
 */
#ifndef _XXX_H_
#define _XXX_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;
/*
 * Class name - XXXX
 * Author - 
 * Updated date - 
 * Desc -
 *
 */J
class XXXX
{
	// Structures
private:

	// Constructors
public:
	XXXX();
	XXXX(const XXXX&);
	XXXX& operator=(const XXXX&);
	~XXXX();

public:
	// Interfaces
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, /* params */);

private:
	// Kernels
	bool _initialize_shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	bool _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, /* params */);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader*	m_pPixelShader;
	ID3D11InputLayout*	m_pInputLayout;
	ID3D11Buffer*		m_pMatrixBuffer;
	ID3D11SamplerState* m_pSamplerState;
};

#endif