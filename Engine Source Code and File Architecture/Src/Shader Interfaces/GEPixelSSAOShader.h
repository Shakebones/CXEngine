/*
 * File name - GEPixelSSAOShader.h
 * Author - CuiXue
 * Updated date - 2013.4.2
 * Desc - 
 *
 */
#ifndef _GE_PIXEL_SSAO_SHADER_H_
#define _GE_PIXEL_SSAO_SHADER_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include "Utility.h"
#endif

#include <fstream>
using namespace std;

/*
 * Class name - CGEPixelSSAOShader
 * Author - CuiXue
 * Updated date - 2013.4.2
 * Desc - 
 */
class CGEPixelSSAOShader
{
public:
	CGEPixelSSAOShader();
	CGEPixelSSAOShader(const CGEPixelSSAOShader&);
	CGEPixelSSAOShader& operator=(const CGEPixelSSAOShader&);
	~CGEPixelSSAOShader();

public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	//...

private:
	bool _initialize_shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void _shutdown();
	void _output_error_message(ID3D10Blob*, HWND, WCHAR*);
	//...
	bool _render_shaders(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	ID3D11ShaderResourceView* m_pPositionRTT;
	ID3D11ShaderResourceView* m_pNormalsRTT;
	ID3D11ShaderResourceView* m_pRandomTexture;
};
#endif