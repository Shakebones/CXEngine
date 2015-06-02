/*
* File name - Working_text.cpp
* Author - CuiXue
* Updated date - 2013.3.22
* Desc -
* Overload some functions of 'Engine'
* for specific application
*
* 2.26.13（modifying）
* 1）.对初始化进行分类管理
* 2）.对每一帧渲染元素进行分类管理
*/
#include "Demo_water.h"

CWorking::CWorking()
{		
	this->m_pModel					= 0;
	this->m_pCamera					= 0;
	this->m_pCameraMoveMgr			= 0;
	this->m_pFrustum				= 0;
	this->m_pTextureShader			= 0;
	this->m_pLight					= 0;
	this->m_pLightShader			= 0;
	this->m_pBitmap					= 0;
	this->m_pText					= 0;
	this->m_pDXInput				= 0;
	this->m_pAudio					= 0;
	this->m_pCPU_info				= 0;
	this->m_pFadeShader				= 0;
	this->m_pFPS_info				= 0;
	this->m_pTimer					= 0;
	this->m_pWaterShader			= 0;
	this->renderCount				= 0;
	this->m_hConsoleHandle			= 0;
	this->m_pTextureSequence		= 0;
	this->m_pClipPlaneShader		= 0;
	this->m_pTextureTranslation		= 0;
	this->m_pReflectionShader		= 0;
	this->m_pRefractionShader		= 0;
	this->m_pRTT_Reflection			= 0;
	this->m_pRTT_Perlin_Array[0]	= 0;
	this->m_pRTT_Perlin_Array[1]	= 0;
}

CWorking::CWorking(const CWorking& source)
{
}

CWorking& CWorking::operator=(const CWorking& source)
{
	return *this;
}

CWorking::~CWorking()
{
}

bool CWorking::Initialize(bool is_full_screen, bool vsyncEnable, int width, int height)
{
	int tmpSCWidth;
	int tmpSCHeight;
	bool result;

	tmpSCWidth	= width;
	tmpSCHeight = height;

	// Base components init
	result = this->_base_coms_init(is_full_screen, vsyncEnable, width, height);
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Base components initialize failed !", L"Error", MB_OK);
		return false;
	}

	// Graphics components init
	result = this->_graphics_coms_init(width, height);
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Graphics components initialize failed !", L"Error", MB_OK);
		return false;
	}

	//Scene objs init
	result = this->_scene_obj_init();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"One or more scene's obj initialize failed", L"Error", MB_OK);
		return false;
	}

	// Utilities init
	result = this->_utility_init();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Utilities initialize failed !", L"Error", MB_OK);
		return false;
	}

	// Shader for display
	result = this->_shader_ready();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Initialize shaders failed !", L"Error", MB_OK);
		return false;
	}

	return true;
}

void CWorking::Run()
{
	MSG msg = {0};
	bool result;
	bool done = false;

	::ZeroMemory(&msg, sizeof(MSG));
	while( !done )
	{
		if( ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if( WM_QUIT == msg.message )
			done = true;

		else
		{
			result = this->_frame();
			if( !result )
				done = true;
		}

		if( this->m_pDXInput->IsKeyPressed(DIK_ESCAPE) == true )
		{
			done = true;
		}
	}
}

void CWorking::Shutdown()
{
	::Shutdown<CGERefractionShader*>(this->m_pRefractionShader);
	::Shutdown<CGEWaterShader*>(this->m_pWaterShader);
	::Shutdown<CGEFadeShader*>(this->m_pFadeShader);
	::Shutdown<CGERenderToTexture*>(this->m_pRTT_Reflection);
	::Shutdown<CGEReflectionShader*>(this->m_pReflectionShader);
	::Shutdown<CGETextureTranslate*>(this->m_pTextureTranslation);
	::Shutdown<CGEClipPlaneShader*>(this->m_pClipPlaneShader);
	::Shutdown<CGETextureShader*>(this->m_pTextureShader);
	::Shutdown<CGEMultiTextureSequence*>(this->m_pTextureSequence);
	::Delete<CGEFrustum*>(this->m_pFrustum);
	::Delete<CGECameraMovementMgr*>(this->m_pCameraMoveMgr);
	::Delete<CUtility_timer*>(this->m_pTimer);
	::Shutdown<CUtility_cpu*>(this->m_pCPU_info);
	::Delete<CUtility_fps*>(this->m_pFPS_info);
	::Shutdown<CGEText*>(this->m_pText);
	::Shutdown<CGEDXInput*>(this->m_pDXInput);
	::Shutdown<CGESound*>(this->m_pAudio);
	::Shutdown<CGEModel*>(this->m_pModel);
	::Delete<CGECamera*>(this->m_pCamera);
	::Shutdown<CGEGraphics*>(this->m_pGraphics);

	this->_shutdown_windows();

	::CloseHandle(this->m_hConsoleHandle);
}

void CWorking::DeviceInformation()
{
	bool result;

	result = this->_display_mouse_pos();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"No mouse information", L"Error", MB_OK);
	}

	result = this->_display_cpu_utilization();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"No CPU information", L"Error", MB_OK);
	}

	result = this->_display_fps();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"No FPS information", L"Error", MB_OK);
	}

	result = this->_display_scence_info();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"No models' information", L"Error", MB_OK);
	}
}

/*
* Desc - Kernels implementation
* Updated date - 2013.3.17
*/
bool CWorking::_frame()
{
	bool result;

	this->_pre_run();
	this->_input_process();

	// Reflection (Render to texture)
	this->_rtt_reflections();
	this->_rtt_water_reflection();
	this->_rtt_water_refraction();

	// Little window on left bottom. (Render to texture)
	//this->m_pGraphics->PreProcessSceneToTexture(0.0f, 0.0f, 0.0f, 1.0f);
	//this->_render_objs();
	//this->m_pGraphics->PostProcessSceneToTexture();

	// Graphics run !
	this->m_pGraphics->BeginRender(D3DXCOLOR(0.02f, 0.02f, 0.02f, 1.0f));

	result = this->_render_objs();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"There must at least 1 obj render failed ! please check function 'CWorking::_render_objs()'",
			L"Error", MB_OK);
		return false;
	}

	this->_render_shader_debugger_window();

	// All obj ready to screen.
	this->m_pGraphics->EndRender();

	return true;
}

bool CWorking::_display_mouse_pos()
{
	bool result;
	char mousePos[16];
	char temp[16];
	int mouseX;
	int mouseY;

	this->m_pDXInput->GetMouseLocation(mouseX, mouseY);

	_itoa_s(mouseX, temp, 10);
	strcpy_s(mousePos, "Mouse X = ");
	strcat_s(mousePos, temp);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		MOUSEPOSX, mousePos, 5, 60, 64, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	if( !result )
		return false;

	_itoa_s(mouseY, temp, 10);
	strcpy_s(mousePos, "Mouse Y = ");
	strcat_s(mousePos, temp);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		MOUSEPOSY, mousePos, 5, 80, 64, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	if( !result )
		return false;

	return true;
}

bool CWorking::_display_cpu_utilization()
{
	char tempStr[64];
	char cpu_info[64];
	int  cpu_utilization;
	D3DXVECTOR4 color;
	bool result;

	cpu_utilization = this->m_pCPU_info->GetUtilization();

	_itoa_s(cpu_utilization, tempStr, 10);
	strcpy_s(cpu_info, "CPU Utilization - ");
	strcat_s(cpu_info, tempStr);
	strcat_s(cpu_info, "%");

	if( cpu_utilization >= 100 )
		color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	else
		color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CPUINFO, cpu_info, 5, 120, 64, color);

	if( !result )
		return false;

	return true;
}

bool CWorking::_display_fps()
{
	char tempStr[64];
	char fps_info[64];
	int  fps;
	D3DXVECTOR4 color;
	bool result;

	fps = this->m_pFPS_info->GetFPS();

	if( fps < 30 )
		color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	else
		color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	if( fps > 1000 )
		fps = 1000;

	_itoa_s(fps, tempStr, 10);
	strcpy_s(fps_info, "FPS - ");
	strcat_s(fps_info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		FPSINFO, fps_info, 5, 140, 64, color);
	if( !result )
		return false;

	return true;
}

bool CWorking::_display_scence_info()
{
	char tempStr[64];
	char info[64];

	int triangles = 0;
	int vertices = 0;
	int indices = 0;

	float camPosx = 0.0f;
	float camPosy = 0.0f;
	float camPosz = 0.0f;

	camPosx = this->cameraPos.x;
	camPosy = this->cameraPos.y;
	camPosz = this->cameraPos.z;

	D3DXVECTOR4 color(0.0f, 1.0f, 0.0f, 1.0f);

	bool result;

	CGEModel *tmp = NULL;

	for(int i = 0; i != this->m_modelsManager.GetModelCount(); i++)
	{
		tmp = *this->m_modelsManager.GetModelByIndex(i);
		vertices += tmp->GetVertexCount();
		indices	 += tmp->GetIndexCount();
		triangles+= tmp->GetTrianglesCount();
	}

	_itoa_s(vertices, tempStr, 10);
	strcpy_s(info, "Number of vertices : ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		VERTICESTEXT, info, 5, 160, 64, color);
	if( !result )
		return false;

	_itoa_s(indices, tempStr, 10);
	strcpy_s(info, "Number of indices : ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		INDICESTEXT, info, 5, 180, 64, color);
	if( !result )
		return false;

	_itoa_s(triangles, tempStr, 10);
	strcpy_s(info, "Number of triangles : ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		TRIANGLESTEXT, info, 5, 200, 64, color);
	if( !result )
		return false;

	_itoa_s((int)camPosx, tempStr, 10);
	strcpy_s(info, "Camera pos x = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMPOSX, info, 480, 10, 64, color);
	if( !result )
		return false;

	_itoa_s((int)camPosy, tempStr, 10);
	strcpy_s(info, "Camera pos y = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMPOSY, info, 600, 10, 64, color);
	if( !result )
		return false;

	_itoa_s((int)camPosz, tempStr, 10);
	strcpy_s(info, "Camera pos z = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMPOSZ, info, 720, 10, 64, color);
	if( !result )
		return false;

	_itoa_s(this->renderCount, tempStr, 10);
	strcpy_s(info, "Bounding sphere clipping - ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		RENDEREDOBJS, info, 5, 220, 64, color);
	if( !result )
		return false;

	return true;
}

bool CWorking::_base_coms_init(bool fullScreen, bool vsyncEnable, int width, int height)
{
	bool result;

	result = this->_initWindows(fullScreen, width, height);
	if( !result )
		return false;

	// Description -
	// Initialize  compositions
	// The Init-Objs are not classified yet.
	// To make a resource-pool for managing resources.

	/////////////////////////////////// Input ///////////////////////////////////
	this->m_pDXInput = new CGEDXInput;
	if( !this->m_pDXInput )
		return false;

	result = this->m_pDXInput->Initialize(this->m_hInstance, this->m_hWnd, width, height);
	if( !result  )
		return false;
	/////////////////////////////////// Input ///////////////////////////////////

	/////////////////////////////////// Audio ///////////////////////////////////
	this->m_pAudio = new CGESound;
	if( !this->m_pAudio )
		return false;

	result = this->m_pAudio->Initialize(this->m_hWnd);
	if( !result )
		return false;

	result = this->m_pAudio->LoadAudioFile("../Res/Audio/TestSound01.wav", SOUNDTEST);
	if( !result )
		return false;

	result = this->m_pAudio->LoadAudioFile("../Res/Audio/BGM01.wav", BGMTEST);
	if( !result )
		return false;

	result = this->m_pAudio->Play(SOUNDTEST);
	if( !result )
		return false;
	/////////////////////////////////// Audio ///////////////////////////////////

	/////////////////////////////////// Graph ///////////////////////////////////
	this->m_pGraphics = new CGEGraphics;
	if( !this->m_pGraphics )
		return false;

	result = this->m_pGraphics->Initialize(fullScreen, vsyncEnable, width, height, this->m_hWnd);
	if( !result )
		return false;
	/////////////////////////////////// Graph ///////////////////////////////////

	// Console for debugging.
	BOOL result_console_window;
	result_console_window = ::AllocConsole();
	if( !result_console_window )
	{
		::MessageBox(this->m_hWnd, L"Cannot setup console window !", L"Error", MB_OK);
	}

	::SetConsoleTitle(L"Debugging window for Graphics Engine");		

	this->m_hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	WCHAR *pWelcome = L"----------------- Engine Start -----------------";
	::WriteConsole(this->m_hConsoleHandle, pWelcome, lstrlenW(pWelcome), NULL, NULL);

	return true;
}

bool CWorking::_graphics_coms_init(int scWidth, int scHeight)
{
	bool result;
	/////////////////////////////////// Camera //////////////////////////////////
	this->m_pCamera = new CGECamera;
	if( !this->m_pCamera )
		return false;

	this->cameraPos = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	this->m_pCamera->SetPosition(this->cameraPos.x, this->cameraPos.y, this->cameraPos.z);
	this->m_pCamera->Run();
	this->m_pCamera->GetViewMatrix(baseView);
	/////////////////////////////////// Camera //////////////////////////////////

	////////////////////////////////// Text /////////////////////////////////////
	this->m_pText = new CGEText;
	if( !this->m_pText )
		return false;

	result = this->m_pText->Initialize(this->m_pGraphics->GetD3DDevice(),
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		this->m_hWnd, scWidth, scHeight, baseView);
	if( !result )
		return false;

	/*
	* Function desc -
	* New interfaces for user set text.
	*/
	this->m_pText->Text(this->m_pGraphics->GetD3DDevice(),
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		HELLO, "Hello GE - Font Engine",
		5, 10, 64, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));

	this->m_pText->Text(this->m_pGraphics->GetD3DDevice(),
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		GOODBYE, "Goodbye GE - Font Engine", 
		5, 30, 64, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	////////////////////////////////// Text //////////////////////////////////

	////////////////////////////// MultiTexture //////////////////////////////
	this->m_pTextureSequence = new CGEMultiTextureSequence;
	if( !this->m_pTextureSequence )
		return false;

	result = this->m_pTextureSequence->Initialize(this->m_pGraphics->GetD3DDevice(), 
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
	if( !result )
		return false;
	////////////////////////////// MultiTexture //////////////////////////////


	////////////////////////////// Models Manager ////////////////////////////
	result = this->m_modelsManager.Initialize(this->m_pGraphics->GetD3DDevice(), 
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
	if( !result )
		return false;
	////////////////////////////// Models Manager ////////////////////////////

	/////////////////////////////////// Model ////////////////////////////////
	this->m_pModel = new CGEModel;
	if( !this->m_pModel )
		return false;
	/////////////////////////////////// Model ////////////////////////////////

	///////////////////////// Camera Movement Manager ////////////////////////
	this->m_pCameraMoveMgr = new CGECameraMovementMgr;
	if( !this->m_pCameraMoveMgr )
		return false;

	this->m_pCameraMoveMgr->SetStartPoint(0.0f, -8.0f, -20.0f);
	///////////////////////// Camera Movement Manager ////////////////////////

	///////////////////////////////// Frustum ////////////////////////////////
	this->m_pFrustum = new CGEFrustum;
	if( !this->m_pFrustum )
		return false;
	///////////////////////////////// Frustum ///////////////////////////////

	////////////////////////////// Shader Debugger //////////////////////////
	this->m_pGraphics->CreateShaderDebugger(this->m_pGraphics->GetD3DDevice(), this->m_hWnd,
		scWidth, scHeight, 320, 180);
	if( !result )
		return false;
	////////////////////////////// Shader Debugger //////////////////////////

	//////////////////////////// RTT For Reflection /////////////////////////
	this->m_pRTT_Reflection = new CGERenderToTexture;
	if( !this->m_pRTT_Reflection )
		return false;

	result = this->m_pRTT_Reflection->Initialize(this->m_pGraphics->GetD3DDevice(), scWidth, scHeight);
	if( !result )
		return false;
	//////////////////////////// RTT For Reflection /////////////////////////

	//////////////////////////// RTT For Reflection /////////////////////////
	this->m_pReflection_water = new CGERenderToTexture;
	if( !this->m_pReflection_water )
		return false;

	result = this->m_pReflection_water->Initialize(this->m_pGraphics->GetD3DDevice(), scWidth, scHeight);
	if( !result )
		return false;
	//////////////////////////// RTT For Reflection /////////////////////////

	//////////////////////////// RTT For Refraction /////////////////////////
	this->m_pRefraction_water = new CGERenderToTexture;
	if( !this->m_pRefraction_water )
		return false;

	result = this->m_pRefraction_water->Initialize(this->m_pGraphics->GetD3DDevice(), scWidth, scHeight);
	if( !result )
		return false;
	//////////////////////////// RTT For Refraction /////////////////////////

	return true;
}

bool CWorking::_shader_ready()
{
	bool result;

	///////////////////////////////// Lighting ///////////////////////////////
	this->m_pLight = new CGELight;
	if( !this->m_pLight )
		return false;

	this->m_pLightShader = new CGELightShader;
	if( !this->m_pLightShader )
		return false;

	result = this->m_pLightShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;

	this->m_pLight->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	this->m_pLight->SetDiffuseColor(1.0f, 0.7f, 0.3f, 1.0f);
	this->m_pLight->SetSpecularColor(1.0f, 0.7f, 0.3f, 1.0f);
	this->m_pLight->SetSpecularPower(64.0f);
	this->m_pLight->SetLightDirection(0.0f, 0.0f, 1.0f);
	///////////////////////////////// Lighting ///////////////////////////////


	/////////////////////////////// Texture shader ///////////////////////////
	this->m_pTextureShader = new CGETextureShader;
	if( !this->m_pTextureShader )
		return false;

	result = this->m_pTextureShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	/////////////////////////////// Texture shader ///////////////////////////


	/////////////////////////// Clip Plane shader ////////////////////////////
	this->m_pClipPlaneShader = new CGEClipPlaneShader;
	if( !this->m_pClipPlaneShader )
		return false;

	result = this->m_pClipPlaneShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	/////////////////////////// Clip Plane shader ////////////////////////////

	/////////////////////// Texture Translate shader /////////////////////////
	this->m_pTextureTranslation = new CGETextureTranslate;
	if( !this->m_pTextureTranslation )
		return false;

	result = this->m_pTextureTranslation->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	/////////////////////// Texture Translate shader /////////////////////////


	///////////////////////////// Reflection shader //////////////////////////
	this->m_pReflectionShader = new CGEReflectionShader;
	if( !this->m_pReflectionShader )
		return false;
	
	result = this->m_pReflectionShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	///////////////////////////// Reflection shader //////////////////////////

	//////////////////////////////// Fade shader /////////////////////////////
	this->m_pFadeShader = new CGEFadeShader;
	if( !this->m_pFadeShader )
		return false;

	result = this->m_pFadeShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	//////////////////////////////// Fade shader /////////////////////////////

	//////////////////////////////// Water shader ////////////////////////////
	this->m_pWaterShader = new CGEWaterShader;
	if( !this->m_pWaterShader )
		return false;

	result = this->m_pWaterShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	//////////////////////////////// Water shader ////////////////////////////

	///////////////////////////// Refraction shader //////////////////////////
	this->m_pRefractionShader = new CGERefractionShader;
	if( !this->m_pRefractionShader )
		return false;

	result = this->m_pRefractionShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	///////////////////////////// Refraction shader //////////////////////////

	return true;
}

bool CWorking::_utility_init()
{
	bool result;

	///////////////////////////////// CPU Info ///////////////////////////////
	this->m_pCPU_info = new CUtility_cpu;
	if( !this->m_pCPU_info )
		return false;

	result = this->m_pCPU_info->Initialize();
	if( !result )
		return false;

	///////////////////////////////// CPU Info ///////////////////////////////

	///////////////////////////////// Timer Info /////////////////////////////
	this->m_pTimer = new CUtility_timer;
	if( !this->m_pTimer )
		return false;

	result = this->m_pTimer->Initialize();
	if( !result )
		return false;
	///////////////////////////////// Timer Info /////////////////////////////

	///////////////////////////////// FPS Info ///////////////////////////////
	this->m_pFPS_info = new CUtility_fps;
	if( !this->m_pFPS_info )
		return false;

	result = this->m_pFPS_info->Initialize();
	if( !result )
		return false;
	///////////////////////////////// FPS Info ///////////////////////////////

	return true;
}

void CWorking::_pre_run()
{
	// Now working !
	this->m_pDXInput->Run();
	this->m_pCPU_info->Run();
	this->m_pFPS_info->Run();
	this->m_pTimer->Run();

	// Show device information
	this->DeviceInformation();
}

void CWorking::_input_process()
{
	bool isKeyDown = false;
	float rotY = 0.0f;
	float rotX = 0.0f;

	if( this->m_pDXInput->IsKeyPressed(DIK_F1) )
		this->m_pGraphics->SolidMode();

	if( this->m_pDXInput->IsKeyPressed(DIK_F2) )
		this->m_pGraphics->WireFrameMode();

	this->m_pCameraMoveMgr->SetFrameTime(this->m_pTimer->GetTime());

	this->m_pCameraMoveMgr->MoveForward(this->m_pDXInput->IsKeyPressed(DIK_W));
	this->m_pCameraMoveMgr->MoveBack(this->m_pDXInput->IsKeyPressed(DIK_S));
	this->m_pCameraMoveMgr->MoveLeft(this->m_pDXInput->IsKeyPressed(DIK_A));
	this->m_pCameraMoveMgr->MoveRight(this->m_pDXInput->IsKeyPressed(DIK_D));
	this->m_pCameraMoveMgr->Up(this->m_pDXInput->IsKeyPressed(DIK_Q));
	this->m_pCameraMoveMgr->Down(this->m_pDXInput->IsKeyPressed(DIK_E));
	this->m_pCameraMoveMgr->LeftRotationY(this->m_pDXInput->IsKeyPressed(DIK_LEFTARROW));
	this->m_pCameraMoveMgr->RightRotationY(this->m_pDXInput->IsKeyPressed(DIK_RIGHTARROW));
	this->m_pCameraMoveMgr->LookUp(this->m_pDXInput->IsKeyPressed(DIK_UP));
	this->m_pCameraMoveMgr->LookDown(this->m_pDXInput->IsKeyPressed(DIK_DOWN));

	rotY = this->m_pCameraMoveMgr->GetRotationY();
	rotX = this->m_pCameraMoveMgr->GetRotationX();

	this->cameraPos.x = this->m_pCameraMoveMgr->GetLeftRightPos();
	this->cameraPos.y = this->m_pCameraMoveMgr->GetUpDownPos();
	this->cameraPos.z = this->m_pCameraMoveMgr->GetMoveForwardBackPos();

	this->m_pCamera->SetPosition(this->cameraPos.x, this->cameraPos.y, this->cameraPos.z);
	this->m_pCamera->SetRotation(rotX, rotY, 0.0f);
}

bool CWorking::_render_objs()
{
	bool result;

	LightBufferType light_1;

	light_1.ambientColor = this->m_pLight->GetAmbientColor();
	light_1.diffuseColor = this->m_pLight->GetDiffuseColor();
	light_1.specularColor = this->m_pLight->GetSpecularColor();
	light_1.direction = this->m_pLight->GetLightDirection();
	light_1.specularPower = this->m_pLight->GetSpecularPower();

	// Now camera run !
	this->m_pCamera->Run();
	this->m_pCamera->GetViewMatrix(view);
	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->world);
	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->world_wall);
	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->world_water);
	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->worldText);
	this->m_pGraphics->GetD3DObj()->GetProjectionMatrix(this->proj);

	this->m_pGraphics->GetD3DObj()->GetOrthoMatrix(this->ortho);

	this->m_pFrustum->ConstructFrustum(SCREEN_DEPTH, proj, view);

	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * 0.0002f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	this->water_normalmap_translation += 0.0005f;
	if( this->water_normalmap_translation > 1.0f )
		this->water_normalmap_translation -= 1.0f;

	this->m_pLight->SetLightDirection(cos(rotation * 10.0f), -0.5f, sin(rotation * 10.0f));

	this->renderCount = 0;

	for(int i = 0; i != this->m_modelsManager.GetModelCount(); i++)
	{
		this->m_pModel = *this->m_modelsManager.GetModelByIndex(i);

		if( TESTPLANE_GROUND == this->m_modelsManager.GetModelIDByIndex(i) )
		{
			D3DXMatrixTranslation(&this->translation, 0.0f, -10.0f, 0.0f);
			this->resultMat = this->world * this->translation;

			this->m_modelsManager[TESTPLANE_GROUND].Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
			result = this->m_pReflectionShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(), 
				this->m_pModel->GetIndexCount(), this->resultMat, view, proj, 
				this->m_pModel->GetTexture(), this->m_pRTT_Reflection->GetShaderResourceView(),
				this->reflectionView_ground); 

			if( !result )
				return false;

			continue;
		}

		if( TESTMODEL_GODDESS == this->m_modelsManager.GetModelIDByIndex(i) )
		{
			D3DXMatrixTranslation(&translationMat_Goddess, 0.0f, -9.5f, 0.0f);

			resultMat_Goddess = worldGoddess * translationMat_Goddess;

			this->m_modelsManager[TESTMODEL_GODDESS].Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
			result = this->m_pLightShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
				this->m_pModel->GetIndexCount(), this->resultMat_Goddess, view, proj, this->m_pModel->GetTexture(),
				this->m_pLight->GetLightDirection(),
				this->m_pLight->GetAmbientColor(),
				this->m_pLight->GetDiffuseColor(),
				this->m_pLight->GetSpecularColor(),
				this->m_pCamera->GetPosition(),
				this->m_pLight->GetSpecularPower());

			if( !result )
				return false;

			continue;
		}

		if( MODEL_BATH == this->m_modelsManager.GetModelIDByIndex(i) )
		{
			D3DXMatrixTranslation(&translationMat_Goddess, 0.0f, -9.0f, 0.0f);
			D3DXMatrixScaling(&scale, 2.5f, 1.0f, 2.5f);

			resultMat_bath = world_bath * scale * translation_bath;

			this->m_modelsManager[MODEL_BATH].Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
			result = this->m_pLightShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
				this->m_pModel->GetIndexCount(), this->resultMat_bath, view, proj, this->m_pModel->GetTexture(),
				this->m_pLight->GetLightDirection(),
				this->m_pLight->GetAmbientColor(),
				this->m_pLight->GetDiffuseColor(),
				this->m_pLight->GetSpecularColor(),
				this->m_pCamera->GetPosition(),
				this->m_pLight->GetSpecularPower());

			if( !result )
				return false;

			continue;
		}

		if( MODEL_WALL == this->m_modelsManager.GetModelIDByIndex(i) )
		{
			D3DXMatrixTranslation(&translation_wall, 0.0f, -9.0f, 10.0f);
			D3DXMatrixScaling(&this->scale, 2.0f, 1.0f, 1.0f);

			resultMat_wall = world_wall * scale * translation_wall;

			this->m_modelsManager[MODEL_WALL].Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
			result = this->m_pLightShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
				this->m_pModel->GetIndexCount(), this->resultMat_wall, view, proj, this->m_pModel->GetTexture(),
				this->m_pLight->GetLightDirection(),
				this->m_pLight->GetAmbientColor(),
				this->m_pLight->GetDiffuseColor(),
				this->m_pLight->GetSpecularColor(),
				this->m_pCamera->GetPosition(),
				this->m_pLight->GetSpecularPower());

			if( !result )
				return false;

			continue;
		}

		if( MODEL_2D_WATER == this->m_modelsManager.GetModelIDByIndex(i) )
		{
			D3DXMatrixTranslation(&translation_water, 0.0f, this->waterHeight, 0.0f);
			D3DXMatrixScaling(&scale, 2.5, 1.0f, 2.5f);

			this->resultMat_water = this->world_water * scale * translation_water;

			this->m_pGraphics->TurnOnAlphaBlending();
			this->m_modelsManager[MODEL_2D_WATER].Render(this->m_pGraphics->Get_D3D11DeviceContext());
			this->m_pWaterShader->Render(this->m_pGraphics->Get_D3D11DeviceContext(), this->m_modelsManager[MODEL_2D_WATER].GetIndexCount(),
				this->resultMat_water, view, proj, this->reflectionView_water, this->m_pReflection_water->GetShaderResourceView(), this->m_pRefraction_water->GetShaderResourceView(),
				this->m_modelsManager[MODEL_2D_WATER].GetTexture(), this->water_normalmap_translation, 0.03f, &light_1, this->m_pCamera->GetPosition());

			this->m_pGraphics->TurnOffAlphaBlending();

			if( !result )
				return false;

			continue;
		}
	}

	//******************* Text *******************
	this->m_pGraphics->TurnOffZBuffer();
	this->m_pGraphics->TurnOnAlphaBlending();

	// Text engine run !
	result = this->m_pText->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		worldText, ortho);
	if( !result )
		return false;

	this->m_pGraphics->TurnOffAlphaBlending();
	this->m_pGraphics->TurnOnZBuffer();
	//******************* Text *******************

	return true;
}

bool CWorking::_rtt_reflections()
{
	bool result;

	this->m_pRTT_Reflection->SetRenderTarget(this->m_pGraphics->Get_D3D11DeviceContext(), 
											 this->m_pGraphics->GetD3DObj()->GetDepthStencilView());

	this->m_pRTT_Reflection->ClearRenderTarget(this->m_pGraphics->Get_D3D11DeviceContext(),
											   this->m_pGraphics->GetD3DObj()->GetDepthStencilView(), 
											   0.0f, 0.0f, 0.0f, 1.0);

	D3DXMatrixTranslation(&translationMat_Goddess, 0.0f, -9.5f, 0.0);
	D3DXMatrixTranslation(&translation_bath, 0.0f, -9.0f, 0.0);

	this->m_pCamera->CalculateReflectionMatrix(-10.0f);

	this->reflectionView_ground = this->m_pCamera->GetReflectionViewMatrix();

	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->worldGoddess);
	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->world_bath);
	this->m_pGraphics->GetD3DObj()->GetProjectionMatrix(this->proj);

	D3DXMatrixScaling(&this->scale, 2.5f, 1.0f, 2.5f);
	this->resultMat_Goddess = worldGoddess * translationMat_Goddess;
	this->resultMat_bath = world_bath * scale * translation_bath;

	this->m_modelsManager[MODEL_BATH].Render(this->m_pGraphics->Get_D3D11DeviceContext());
	result = this->m_pLightShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
										  this->m_modelsManager[MODEL_BATH].GetIndexCount(), 
										  this->resultMat_bath, this->reflectionView_ground, this->proj, 
										  this->m_modelsManager[MODEL_BATH].GetTexture(),
										  this->m_pLight->GetLightDirection(),
										  this->m_pLight->GetAmbientColor(),
										  this->m_pLight->GetDiffuseColor(),
										  this->m_pLight->GetSpecularColor(),
										  this->m_pCamera->GetPosition(),
										  this->m_pLight->GetSpecularPower());

	if( !result )
		return false;

	this->m_pGraphics->GetD3DObj()->SetBackBufferRenderTarget();

	return true;
}

bool CWorking::_rtt_water_reflection()
{
	bool result;

	D3DXVECTOR4 clipPlane = D3DXVECTOR4(0.0f, 1.0f, 0.0f, -this->waterHeight + 0.1f);

	this->m_pReflection_water->SetRenderTarget(this->m_pGraphics->Get_D3D11DeviceContext(), 
											   this->m_pGraphics->GetD3DObj()->GetDepthStencilView());
	this->m_pReflection_water->ClearRenderTarget(this->m_pGraphics->Get_D3D11DeviceContext(),
		this->m_pGraphics->GetD3DObj()->GetDepthStencilView(),
		0.0f, 0.0f, 0.0f, 1.0);

	this->m_pCamera->CalculateReflectionMatrix(this->waterHeight);
	this->reflectionView_water = this->m_pCamera->GetReflectionViewMatrix();

	this->m_modelsManager[TESTMODEL_GODDESS].Render(this->m_pGraphics->Get_D3D11DeviceContext());
	result = this->m_pRefractionShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
										  this->m_modelsManager[TESTMODEL_GODDESS].GetIndexCount(), 
										  this->resultMat_Goddess, this->reflectionView_water, this->proj, 
										  this->m_modelsManager[TESTMODEL_GODDESS].GetTexture(),
										  this->m_pLight->GetLightDirection(),
										  this->m_pLight->GetAmbientColor(),
										  this->m_pLight->GetDiffuseColor(),
										  this->m_pLight->GetSpecularColor(),
										  this->m_pCamera->GetPosition(),
										  this->m_pLight->GetSpecularPower(),
										  clipPlane);
	if( !result )
		return false;

	D3DXMatrixTranslation(&translation_wall, 0.0f, -9.0f, 10.0f);
	D3DXMatrixScaling(&this->scale, 2.0f, 1.0f, 1.0f);

	resultMat_wall = world_wall * this->scale * translation_wall;

	this->m_modelsManager[MODEL_WALL].Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
	result = this->m_pRefractionShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
										  this->m_modelsManager[MODEL_WALL].GetIndexCount(), this->resultMat_wall, reflectionView_water, proj, 
										  this->m_modelsManager[MODEL_WALL].GetTexture(),
										  this->m_pLight->GetLightDirection(),
										  this->m_pLight->GetAmbientColor(),
										  this->m_pLight->GetDiffuseColor(),
										  this->m_pLight->GetSpecularColor(),
										  this->m_pCamera->GetPosition(),
										  this->m_pLight->GetSpecularPower(),
										  clipPlane);

	if( !result )
		return false;

	this->m_pGraphics->GetD3DObj()->SetBackBufferRenderTarget();

	return true;
}

bool CWorking::_rtt_water_refraction()
{
	bool result;

	D3DXVECTOR4 clipPlane;

	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, this->waterHeight + 0.1f);

	this->m_pRefraction_water->SetRenderTarget(this->m_pGraphics->Get_D3D11DeviceContext(),
		this->m_pGraphics->GetD3DObj()->GetDepthStencilView());
	
	this->m_pRefraction_water->ClearRenderTarget(this->m_pGraphics->Get_D3D11DeviceContext(),
		this->m_pGraphics->GetD3DObj()->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	this->m_pCamera->Run();
	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->world_bath);
	this->m_pCamera->GetViewMatrix(this->view);
	this->m_pGraphics->GetD3DObj()->GetProjectionMatrix(this->proj);

	D3DXMatrixTranslation(&translation_bath, 0.0f, -9.0f, 0.0);
	D3DXMatrixTranslation(&translationMat_Goddess, 0.0f, -9.5f, 0.0);
	D3DXMatrixScaling(&this->scale, 2.5f, 1.0f, 2.5f);
	this->resultMat_bath = this->world_bath * scale * translation_bath;
	this->resultMat_Goddess = this->worldGoddess * this->translationMat_Goddess;

	this->m_modelsManager[MODEL_BATH].Render(this->m_pGraphics->Get_D3D11DeviceContext());
	result = this->m_pRefractionShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
										  this->m_modelsManager[MODEL_BATH].GetIndexCount(), 
										  this->resultMat_bath, view, this->proj, 
										  this->m_modelsManager[MODEL_BATH].GetTexture(),
										  this->m_pLight->GetLightDirection(),
										  this->m_pLight->GetAmbientColor(),
										  this->m_pLight->GetDiffuseColor(),
										  this->m_pLight->GetSpecularColor(),
										  this->m_pCamera->GetPosition(),
										  this->m_pLight->GetSpecularPower(),
										  clipPlane);

	this->m_modelsManager[TESTMODEL_GODDESS].Render(this->m_pGraphics->Get_D3D11DeviceContext());
	result = this->m_pRefractionShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
										  this->m_modelsManager[TESTMODEL_GODDESS].GetIndexCount(), 
										  this->resultMat_Goddess, view, this->proj, 
										  this->m_modelsManager[TESTMODEL_GODDESS].GetTexture(),
										  this->m_pLight->GetLightDirection(),
										  this->m_pLight->GetAmbientColor(),
										  this->m_pLight->GetDiffuseColor(),
										  this->m_pLight->GetSpecularColor(),
										  this->m_pCamera->GetPosition(),
										  this->m_pLight->GetSpecularPower(),
										  clipPlane);

	if( !result )
		return false;

	this->m_pGraphics->GetD3DObj()->SetBackBufferRenderTarget();

	return true;
}

bool CWorking::_scene_obj_init()
{
	bool result;

	// Ground obj
	result = this->m_pModel->InitializePlane(this->m_pGraphics->GetD3DDevice(), 1, 1, 64.0f, 64.0f);
	if( !result )
		return false;

	result = this->m_pModel->SetTexture(this->m_pGraphics->GetD3DDevice(), L"../Res/Tex/wall01.dds");
	if( !result )
		return false;

	result = this->m_modelsManager.AddModel(TESTPLANE_GROUND, &this->m_pModel);
	if( !result )
		return false;

	this->m_pModel->Shutdown();

	if( !result )
		return false;

	// Goddess obj
	result = this->m_pModel->InitializeVTNVertexBuffer(this->m_pGraphics->GetD3DDevice(), "../Res/Model/Goddess.txt");

	result = this->m_pModel->SetTexture(this->m_pGraphics->GetD3DDevice(), L"../Res/Tex/GoddessMaterial.jpg");
	if( !result )
		return false;

	result = this->m_modelsManager.AddModel(TESTMODEL_GODDESS, &this->m_pModel);
	if( !result )
		return false;

	this->m_pModel->Shutdown();

	// Bath obj
	result = this->m_pModel->InitializeVTNVertexBuffer(this->m_pGraphics->GetD3DDevice(), "../Res/Model/bath.txt");
	if( !result )
		return false;

	result = this->m_pModel->SetTexture(this->m_pGraphics->GetD3DDevice(), L"../Res/Tex/marble01.dds");
	if( !result )
		return false;

	result = this->m_modelsManager.AddModel(MODEL_BATH, &this->m_pModel);
	if( !result )
		return false;

	this->m_pModel->Shutdown();

	// Wall obj
	result = this->m_pModel->InitializeVTNVertexBuffer(this->m_pGraphics->GetD3DDevice(), "../Res/Model/wall.txt");
	if( !result )
		return false;

	result = this->m_pModel->SetTexture(this->m_pGraphics->GetD3DDevice(), L"../Res/Tex/wall01.dds");
	if( !result )
		return false;

	result = this->m_modelsManager.AddModel(MODEL_WALL, &this->m_pModel);
	if( !result )
		return false;

	this->m_pModel->Shutdown();

	// Water plane obj
	result = this->m_pModel->InitializeBumpMapVertexBuffer(this->m_pGraphics->GetD3DDevice(), "../Res/Model/water.txt");
	if( !result )
		return false;

	result = this->m_pModel->SetTexture(this->m_pGraphics->GetD3DDevice(), L"../Res/Tex/normalmap01.jpg");
	if( !result )
		return false;

	result = this->m_modelsManager.AddModel(MODEL_2D_WATER, &this->m_pModel);
	if( !result )
		return false;

	this->waterHeight = -8.75f;
	this->water_normalmap_translation = 0.0f;

	return true;
}

/*
* Function name - CWorking::_render_shader_debugger_window
* Params - /
* Author - CuiXue
* Updated date - 2013.3.22
* Desc - 
*
*     该函数当前有严重性能问题，主要由于该函数在每帧中被调用时，
* 对多纹理序列作出大量 内存拷贝 工作，严重占用CPU时间，当前
* 正在对序列中资源释放的部分进行检测，并同时考虑考虑此处合理
* 的程序结构，以保证尽可能低的CPU消耗（因大量CPU消耗的工作在
* 初始化期间进行）
*/
void CWorking::_render_shader_debugger_window()
{
	CGEShaderDebugger* tmpShaderDebugger = this->m_pGraphics->Get_ShaderDebugger_Module();
	CGERenderToTexture* tmpRenderToTexture = this->m_pGraphics->Get_RenderToTexture_Module();

	this->m_pRTT_Perlin_Array[1] = tmpRenderToTexture->GetShaderResourceView();

	this->m_pGraphics->TurnOffZBuffer();

	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(worldShaderDebugger);

	this->m_pGraphics->RenderShaderDebuggerWindow(50, 500);

	this->m_pTextureShader->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		tmpShaderDebugger->GetIndexCount(), this->worldShaderDebugger, this->baseView, this->ortho,
		this->m_pRefraction_water->GetShaderResourceView());

	this->m_pGraphics->TurnOnZBuffer();
}

