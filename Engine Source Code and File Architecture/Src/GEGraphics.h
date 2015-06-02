/*
 * File name - GEGraphics.h
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc - 
 *
 */
#ifndef _GE_GRAPHICS_H_
#define _GE_GRAPHICS_H_

#ifdef _WIN32
#include <Windows.h>
#include "Utility.h"
#include "GE_D3D_Renderer.h"
#include "GEModel.h"
#include "GECamera.h"
#include "GEColorShader.h"
#include "GETexture.h"
#include "GETextureShader.h"
#include "GELight.h"
#include "GELightShader.h"
#include "GELightMapShader.h"
#include "GEBitmap.h"
#include "GEText.h"
#include "GEModelsManager.h"
#include "GECameraMovementMgr.h"
#include "GEMultiTextureSequence.h"
#include "GEMultiTextureShader.h"
#include "GEAlphaMapShader.h"
#include "GESpecularMappingShader.h"
#include "GEMultiPointLightShader.h"
#include "GEBumpMapShader.h"
#include "GEShaderDebugger.h"
#include "GERenderToTexture.h"
#include "GEClipPlaneShader.h"
#include "GEFadeShader.h"
#include "GEWaterShader.h"
#include "GEGlassShader.h"
#include "GEFireShader.h"
#include "GEFireShader.h"
#include "GETextureTranslate.h"
#include "GETransparencyShader.h"
#include "GEReflectionShader.h"
#include "GERefractionShader.h"
#include "GEFogShader.h"
#include "GEFrustum.h"
#include "GEDepthBufferShader.h"
#include "GEHorizontalBlurShader.h"
#include "GEVerticalBlurShader.h"
#include "GEOrthoWindow.h"
#endif

// Default var
const bool	FULL_SCREEN		= false;
const bool	VSYNC_ENABLE	= false;
const float	SCREEN_DEPTH	= 1000.0f;
const float SCREEN_NEAR		= 0.1f;

/*
 * Class name - CGEGraphics
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc
 * 
 */
class CGEGraphics
{
public:

	// Constructors & destructors
	CGEGraphics();
	CGEGraphics(const CGEGraphics&);
	CGEGraphics& operator=(const CGEGraphics&);
	~CGEGraphics();

	virtual bool	Initialize(bool, bool, int, int, HWND);
	virtual void	Shutdown();
	virtual bool	BeginRender(D3DXCOLOR);
	virtual bool	EndRender();
	virtual ID3D11Device* GetD3DDevice();
	virtual CGE_D3D_Renderer* GetD3DObj();

	// Access functions
	void WireFrameMode();
	void SolidMode();

	// Z-Buffer processing
	void TurnOnZBuffer();
	void TurnOffZBuffer();
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	// Down Sample Texture & Up Sample Texture
	

	/*
	 * Desc - 
	 * 1.Render to texture
	 * 2.Shader debugger window
	 *
	 * PS - Order !
	 * 1.CreateShaderDebugger();
	 * 2.Pre processing.
	 * 3.Post processing.
	 * 4.Turn off z-buffer.
	 * 5.RenderShaderDebuggerWindow();
	 * 6.Turn on z-buffer.
	 */
	bool CreateShaderDebugger(ID3D11Device*, HWND, int, int, int, int);
	bool PreProcessSceneToTexture(float, float, float, float);
	bool PostProcessSceneToTexture();
	bool RenderShaderDebuggerWindow(int, int);

	CGEShaderDebugger* Get_ShaderDebugger_Module();
	CGERenderToTexture* Get_RenderToTexture_Module();
	ID3D11DeviceContext* Get_D3D11DeviceContext();

private:
	bool _pre_render_to_texture(float, float, float, float);
	bool _post_render_to_texture();

protected:
	virtual bool	_render(D3DXCOLOR&);
	virtual bool	_end_render();

protected:
	CGE_D3D_Renderer*	m_GE_D3D_Renderer;
	CGERenderToTexture*	m_pRenderToTexture;
	CGEShaderDebugger*	m_pShaderDebugger;
};

#endif