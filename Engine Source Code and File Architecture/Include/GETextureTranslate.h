/*
 * File name - GETextureTranslate.h
 * Author - CuiXue
 * Updated date - 2013.4.7
 * Desc - 
 *
 */
#ifndef _GE_TEXTURE_TRANSLATE_H_
#define _GE_TEXTURE_TRANSLATE_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGETextureTranslate
 * Author - CuiXue
 * Updated date - 2013.4.7
 * Desc -
 */
class CGETextureTranslate
{
	// Constructors
public:
	CGETextureTranslate();
	CGETextureTranslate(const CGETextureTranslate&);
	CGETextureTranslate& operator=(const CGETextureTranslate&);
	~CGETextureTranslate();

	// Interfaces.
public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);

	// Kernels.
private:
	bool _initialize_shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	bool _set_shader_params(ID3D11DeviceContext*,
							D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
							ID3D11ShaderResourceView*, float);
	void _render_shader(ID3D11DeviceContext*, int);

private:
	typedef struct _texture_translation_buffer
	{
#ifdef __cpluspluse
		_texture_translation_buffer()
		{
			texTranslation = 0.0f;
		}

		_texture_translation_buffer(float texTrans)
		{
			texTranslation = texTrans;
		}
#endif
		float texTranslation;
		D3DXVECTOR3 padding;
	}TranslationBufferType;

	// Members.
private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11SamplerState* m_pSamplerState;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pMatrixBuffer;
	ID3D11Buffer* m_pTranslationBuffer;
};
#endif