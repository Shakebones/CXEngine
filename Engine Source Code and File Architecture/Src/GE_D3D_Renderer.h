/*
 * File name - GE_D3D_Renderer.h
 * Author - CuiXue
 * Updated date - 2013.1.5
 * Desc - 
 *
 */
#ifndef _GE_D3D_RENDERER_H_
#define _GE_D3D_RENDERER_H_

// Lib link
#ifdef _WIN32
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <DXGI.h>
#include <D3Dcommon.h>
#include <D3DX11.h>
#include <D3DX10math.h>
#include "Utility.h"
#endif

/*
 * Class name - CGE_D3D_Renderer
 * Author - CuiXue
 * Updated date - 2013.1.5
 * Desc -
 * 
 */
class CGE_D3D_Renderer
{
public:

	// Constructors & destructor
	CGE_D3D_Renderer();
	CGE_D3D_Renderer(const CGE_D3D_Renderer&);
	CGE_D3D_Renderer& operator=(const CGE_D3D_Renderer&);
	~CGE_D3D_Renderer();

	// Working interfaces
	bool Initialize(bool, int, int, bool, HWND, float, float);
	void Shutdown();

	void BeginScene(D3DXCOLOR&);
	void EndScene();

	// Acceess interfaces
	ID3D11Device* GetDevicePointer();
	ID3D11DeviceContext* GetDeviceContextPointer();
	void GetVideoCardInfo(char*, int&);
	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);
	void ResetViewPort();

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();

	void SetWireFrameMode();
	void SetSolidMode();

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnAlphaBlendingOn();
	void TurnAlphaBlendingOff();

private:

	bool						m_vsync_enabled;
	int							m_videoCardMemory;
	char						m_videoCardDescription[128];
	IDXGISwapChain*				m_pSwapChain;
	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pDeviceContext;
	ID3D11RenderTargetView*		m_pRenderTargetView;
	ID3D11Texture2D*			m_pDepthStencilBuffer;
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilView*		m_pDepthStencilView;
	ID3D11RasterizerState*		m_pRasterState;
	ID3D11DepthStencilState*	m_pDepthDisabledStencilState;

	ID3D11BlendState* m_enable_alpha_blending;
	ID3D11BlendState* m_disable_alpha_blending;

	D3D11_VIEWPORT	viewport;
	D3DXMATRIX m_projMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;
};

#endif