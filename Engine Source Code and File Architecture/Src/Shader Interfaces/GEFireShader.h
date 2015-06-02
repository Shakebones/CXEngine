/*
 * File name - GEFireShader.h
 * Author - CuiXue
 * Updated date - 2013.7.6
 * Desc - 
 *
 */
#ifndef _GE_FIRE_SHADER_H_
#define _GE_FIRE_SHADER_H_

#ifdef _WIN32
#include <D3DX11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - GEFireShader
 * Author - CuiXue
 * Updated date - 2013.7.6
 * Desc - 
 */
class CGEFireShader
{
private:
	struct NoiseBufferType
	{
		float frameTime;
		D3DXVECTOR3 scrollSpeeds;
		D3DXVECTOR3 scales;
		float padding;
	};

	struct DistortionBufferType
	{
		D3DXVECTOR2 distortion1;
		D3DXVECTOR2 distortion2;
		D3DXVECTOR2 distortion3;
		float distortionScale;
		float distortionBias;
	};

public:
	CGEFireShader();
	CGEFireShader(const CGEFireShader&);
	CGEFireShader& operator=(const CGEFireShader&);
	~CGEFireShader();

public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, 
		float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);

private:
	bool _initialize_shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView**,	float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);
	void _render_shaders(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11SamplerState* m_pClampSamplerState;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pNoiseBuffer;
	ID3D11Buffer* m_pDistortionBuffer;
};
#endif