/* 
 * File name - Working_test.h
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc -
 *
 * Test 'GESystem'
 *
 * WARNING !!
 * Update "include file" as same time
 */
#ifndef _WORKING_TEST_H_
#define _WORKING_TEST_H_

// Including files
// Lib linking.
#pragma comment(lib, "..\\lib\\GESystem.lib")

#include "..\\Include\\AppEngine.h"
#include "Demo_water_res.h"

/*
 * Class name CWorking
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc -
 *
 */
class CWorking : public CGEAppEngine
{
public:
	CWorking();
	CWorking(const CWorking&);
	CWorking& operator=(const CWorking&);
	~CWorking();

public:
	bool Initialize(bool is_full_screen, bool vsyncEnable, int width, int height);
	void Run();
	void Shutdown();

public:
	void DeviceInformation();

private:
	// Information display
	bool _display_mouse_pos();
	bool _display_cpu_utilization();
	bool _display_fps();
	bool _display_scence_info();
	// Information display end.

private:
	// Initialize classification
	bool _base_coms_init(bool fullScreen, bool vsyncEnable, int width, int height);
	bool _graphics_coms_init(int, int);
	bool _shader_ready();
	bool _utility_init();
	// Initialize classification end.

private: // Coms of "_graphics_coms_init()"
	bool _scene_obj_init();
	
private:
	// Frame classification
	void _pre_run();
	void _input_process();
	bool _render_objs();
	bool _rtt_reflections();
	bool _rtt_water_reflection();
	bool _rtt_water_refraction();
	void _render_shader_debugger_window();

	// Frame classification end.
	bool _frame();	

private:
	// Matrix for scene
	D3DXMATRIX worldText;
	D3DXMATRIX worldShaderDebugger;
	D3DXMATRIX baseView, view;
	D3DXMATRIX proj, ortho;
	D3DXMATRIX world;
	D3DXMATRIX translation, kMat;
	D3DXMATRIX resultMat;

	// Goddess
	D3DXMATRIX worldGoddess, translationMat_Goddess, kMat_Goddess, resultMat_Goddess;

	// Bath
	D3DXMATRIX world_bath, translation_bath, kMat_bath, resultMat_bath;

	// Wall
	D3DXMATRIX world_wall, translation_wall, kMat_wall, resultMat_wall;

	// Water
	D3DXMATRIX world_water, translation_water, kMat_water, resultMat_water;

	// Scale
	D3DXMATRIX scale;

	//
	D3DXMATRIX reflectionView_water;
	D3DXMATRIX reflectionView_ground;

	ID3D11ShaderResourceView* m_pRTT_Perlin_Array[2];

private:
	CGEModel*					m_pModel;
	CGEModelsManager			m_modelsManager;
	CGEFrustum*					m_pFrustum;
	CGECamera*					m_pCamera;
	CGECameraMovementMgr*		m_pCameraMoveMgr;
	CGEClipPlaneShader*			m_pClipPlaneShader;
	CGEFadeShader*				m_pFadeShader;
	CGEMultiTextureSequence*	m_pTextureSequence;
	CGETextureTranslate*		m_pTextureTranslation;
	CGERefractionShader*		m_pRefractionShader;
	CGEReflectionShader*		m_pReflectionShader;
	CGERenderToTexture*			m_pRTT_Reflection;
	CGEWaterShader*				m_pWaterShader;
	CGETextureShader*			m_pTextureShader;
	CGELight*					m_pLight;
	CGELightShader*				m_pLightShader;
	CGEBitmap*					m_pBitmap;
	CGEText*					m_pText;

	CGERenderToTexture*			m_pReflection_water;
	CGERenderToTexture*			m_pRefraction_water;

	// Utilities
	CUtility_cpu*				m_pCPU_info;
	CUtility_fps*				m_pFPS_info;
	CUtility_timer*				m_pTimer;

	D3DXVECTOR3					cameraPos;
	int							renderCount;
	float						waterHeight;
	float						water_normalmap_translation;

	// For debugging -
	// Console debugging window handle
	HANDLE						m_hConsoleHandle;
};

#endif