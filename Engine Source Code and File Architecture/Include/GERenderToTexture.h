/*
 * File name - GERenderToTexture.h
 * Author - CuiXue
 * Updated date - 2013.3.18
 * Desc - 
 *
 */
#ifndef _GE_RENDER_TO_TEXTURE_H_
#define _GE_RENDER_TO_TEXTURE_H_

#ifdef _WIN32
#include <D3D11.h>

#include "Utility.h"
#endif

/*
 * Class name - CGERenderToTexture
 * Author - CuiXue
 * Updated date - 2013.3.18
 * Desc - 
 *
 */
class CGERenderToTexture
{
public:
	CGERenderToTexture();
	CGERenderToTexture(const CGERenderToTexture&);
	CGERenderToTexture& operator=(const CGERenderToTexture&);
	~CGERenderToTexture();

public:
	bool Initialize(ID3D11Device*, int, int);
	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void SetRenderTarget(ID3D11DeviceContext*);
	void SetRenderTarget_viewport(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();
	void ClearRenderTarget_viewport(ID3D11DeviceContext*, float, float, float, float);
	void GetOrthoMatrix(D3DXMATRIX&);
	float GetWidth();
	float GetHeight();

private:
	float m_textureWidth;
	float m_textureHeight;
	ID3D11Texture2D* m_pRenderTargetTexture;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11ShaderResourceView* m_pShaderResourceView;

	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
	D3D11_VIEWPORT	m_viewPort;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;
};
#endif