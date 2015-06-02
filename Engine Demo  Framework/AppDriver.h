/*
 * File name - AppDriver.h
 * Author - CuiXue
 * Updated date - 2013.7.13
 * Desc - 
 *
 */
#ifndef _APP_DRIVER_H_
#define _APP_DRIVER_H_

// Lib link
#pragma comment(lib, "../lib/GESystem.lib")

// Including files
#include "../Include/AppEngine.h"
#include "AppResourcesID.h"

// Including for application

/*
 * Class name - CAppDriver
 * Author - CuiXue
 * Updated date - 2013.7.13
 * Desc - 
 */
class CAppDriver : public CGEAppEngine
{
public:
	CAppDriver();
	CAppDriver(const CAppDriver&);
	CAppDriver& operator=(const CAppDriver&);
	~CAppDriver();

public:
	// Basic Interfaces
	bool Initialize(bool is_full_screen, bool vsyncEnable, int width, int height);
	void Run();
	void Shutdown();

public:
	// Display Interfaces
	// Kernel of function - _pre_run()
	void Display_device_information();
	void Display_info_on_console_window(WCHAR* msg);
	void Debugging_window();

private:
	// Kernel of Display Interfaces
	bool _display_mouse_pos();
	bool _display_cpu_utilization();
	bool _display_fps();
	bool _display_scence_info();

private:
	// Kernel of function - Initialize(..)
	bool _base_coms_init(bool fullScreen, bool vsyncEnable, int width, int height);
	bool _graphics_coms_init(int width, int height);
	bool _shader_init();
	bool _utilities_init();
	void _shutdown();

private:
	// Kernel of function - _graphics_coms_init(..)
	bool _scene_objs_init();
	void _matrix_process();

private:
	// Kernels of function - Run()
	bool _frame();

private:
	// Kernel of function - _frame()
	void _pre_run();
	void _input_process();
	bool _render_objs();

private:
	// Basic coms
	CGEModel*								m_pModel;
	CGEModelsManager						m_modelsManager;
	CGEFrustum*								m_pFrustum;
	CGECamera*								m_pCamera;
	CGECameraMovementMgr*					m_pCameraMoveMgr;
	CGEMultiTextureSequence*				m_pTextureSequence;
	CGETextureTranslate*					m_pTextureTranslate;
	CGERenderToTexture*						m_pRTT_Usage;
	CGELight*								m_pLight;
	CGEBitmap*								m_pBitmap;
	CGEText*								m_pText;

	CUtility_cpu*							m_pCPU_info;
	CUtility_fps*							m_pFPS_info;
	CUtility_timer*							m_pTimer;

private:
	// GPU Interfaces - Shaders
	CGELightMapShader*			m_pLightMapShader;
	CGELightShader*				m_pLightShader;
	CGEWaterShader*				m_pWaterShader;
	CGEBumpMapShader*			m_pBumpMapShader;
	CGEAlphaMapShader*			m_pAlphaMapShader;
	CGETextureShader*			m_pTextureShader;
	CGETextureTranslate*		m_pTextureTranslation;
	CGEMultiTextureShader*		m_pMultiTextureShader;
	CGETransparencyShader*		m_pTransparencyShader;
	CGEMultiPointLightShader*	m_pMultiPointLightShader;
	CGEReflectionShader*		m_p_Usage_Shader;
	CGESpecularMappingShader*	m_pSpecularMapShader;
	CGEFogShader*				m_pFogShader;
	CGEClipPlaneShader*			m_pClipPlaneShader;
	CGEGlassShader*				m_pGlassShader;
	CGEFireShader*				m_pFireShader;
	CGEFadeShader*				m_pFadeShader;

private:
	// Some other variables
	int							m_frustum_counts;
	D3DXVECTOR3					m_camera_position;
	HANDLE						m_hConsoleWindow;

#ifdef _WIN32
	D3DXMATRIX					m_text_WorldMat;
	D3DXMATRIX					m_text_orthoMat;
	D3DXMATRIX					m_baseView;
	D3DXMATRIX					m_baseProj;
#endif
};

#endif