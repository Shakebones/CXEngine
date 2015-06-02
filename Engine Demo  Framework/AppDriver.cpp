/*
 * File name - AppDriver.cpp
 * Author - CuiXue
 * Updated date - 2013.7.13
 * Desc - 
 *
 */
#include "AppDriver.h"

CAppDriver::CAppDriver()
{
	// Basic coms init
	this->m_pModel = 0;
	this->m_pFrustum = 0;
	this->m_pCamera = 0;
	this->m_pCameraMoveMgr = 0;
	this->m_pMultiTextureShader = 0;
	this->m_pTextureTranslate = 0;
	this->m_pRTT_Usage = 0;
	this->m_pLight = 0;
	this->m_pBitmap = 0;
	this->m_pText = 0;

	// Utilities init
	this->m_pCPU_info = 0;
	this->m_pFPS_info = 0;
	this->m_pTimer = 0;

	// GPU Interfaces - Shader members init
	this->m_pLightMapShader = 0;
	this->m_pLightShader = 0;
	this->m_pWaterShader = 0;
	this->m_pBumpMapShader = 0;
	this->m_pAlphaMapShader = 0;
	this->m_pTextureShader = 0;
	this->m_pMultiTextureShader = 0;
	this->m_pTransparencyShader = 0;
	this->m_pMultiPointLightShader = 0;
	this->m_p_Usage_Shader = 0;
	this->m_pSpecularMapShader = 0;
	this->m_pFogShader = 0;
	this->m_pClipPlaneShader = 0;
	this->m_pGlassShader = 0;
	this->m_pFireShader = 0;

	// Some other variables
	this->m_frustum_counts = 0;
	this->m_camera_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CAppDriver::CAppDriver(const CAppDriver& source)
{
}

CAppDriver& CAppDriver::operator=(const CAppDriver& source)
{
	return *this;
}

CAppDriver::~CAppDriver()
{
}

bool CAppDriver::Initialize(bool  is_full_screen, bool vsyncEnable, int width, int height)
{
	int tmpSCWidth;
	int tmpSCHeight;
	bool result;

	tmpSCWidth	= width;
	tmpSCHeight = height;

	// Startup debuggin window
	this->Debugging_window();

	// Base components init
	result = this->_base_coms_init(is_full_screen, vsyncEnable, width, height);
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Base components initialize failed !", L"Error", MB_OK);
		return false;
	}
	this->Display_info_on_console_window(L"Fundamental coms initialized success\n");

	// Graphics components init
	result = this->_graphics_coms_init(width, height);
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Graphics components initialize failed !", L"Error", MB_OK);
		return false;
	}
	this->Display_info_on_console_window(L"Graphics coms initialized success\n");

	//Scene objs init
	result = this->_scene_objs_init();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"One or more scene's obj initialize failed", L"Error", MB_OK);
		return false;
	}
	this->Display_info_on_console_window(L"Scene's objs initialized success\n");

	// Utilities init
	result = this->_utilities_init();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Utilities initialize failed !", L"Error", MB_OK);
		return false;
	}
	this->Display_info_on_console_window(L"Utilities initialized success\n");

	// Shader for display
	result = this->_shader_init();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"Initialize shaders failed !", L"Error", MB_OK);
		return false;
	}
	this->Display_info_on_console_window(L"Shaders initialized success\n");

	return true;
}

void CAppDriver::Shutdown()
{
	this->_shutdown();
}

void CAppDriver::Run()
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

void CAppDriver::Display_device_information()
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

void CAppDriver::Display_info_on_console_window(WCHAR* msg)
{
	::WriteConsole(this->m_hConsoleWindow, msg, lstrlenW(msg), NULL, NULL);
}

void CAppDriver::Debugging_window()
{
	// Console for debugging.
	BOOL result_console_window;
	result_console_window = ::AllocConsole();
	if( !result_console_window )
	{
		::MessageBox(this->m_hWnd, L"Cannot setup console window !", L"Error", MB_OK);
	}

	::SetConsoleTitle(L"Debugging window for Graphics Engine");

	this->m_hConsoleWindow = GetStdHandle(STD_OUTPUT_HANDLE);

	::SetConsoleTextAttribute(this->m_hConsoleWindow, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	WCHAR *pWelcome = L"----------------- Engine Start -----------------\n";	
	::WriteConsole(this->m_hConsoleWindow, pWelcome, lstrlenW(pWelcome), NULL, NULL);
}

bool CAppDriver::_display_mouse_pos()
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

bool CAppDriver::_display_cpu_utilization()
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

bool CAppDriver::_display_fps()
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

bool CAppDriver::_display_scence_info()
{
	char tempStr[64];
	char info[64];

	int triangles = 0;
	int vertices = 0;
	int indices = 0;

	float camPosx = 0.0f;
	float camPosy = 0.0f;
	float camPosz = 0.0f;

	float camera_direction_x;
	float camera_direction_y;
	float camera_direction_z;

	camera_direction_x = this->m_pCamera->GetDirection().x;
	camera_direction_y = this->m_pCamera->GetDirection().y;
	camera_direction_z = this->m_pCamera->GetDirection().z;

	camPosx = this->m_camera_position.x;
	camPosy = this->m_camera_position.y;
	camPosz = this->m_camera_position.z;

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

	//_itoa_s((int)camPosx, tempStr, 10);
	sprintf_s(tempStr, "%.2f", camPosx);
	strcpy_s(info, "Camera pos x = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMPOSX, info, 5, 660, 64, color);
	if( !result )
		return false;

	//_itoa_s((int)camPosy, tempStr, 10);
	sprintf_s(tempStr, "%.2f", camPosy);
	strcpy_s(info, "Camera pos y = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMPOSY, info, 5, 680, 64, color);
	if( !result )
		return false;

	//_itoa_s((int)camPosz, tempStr, 10);
	sprintf_s(tempStr, "%.2f", camPosz);
	strcpy_s(info, "Camera pos z = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMPOSZ, info, 5, 700, 64, color);
	if( !result )
		return false;

	//_itoa_s((int)camPosx, tempStr, 10);
	sprintf_s(tempStr, "%.2f", camera_direction_x);
	strcpy_s(info, "Camera direction x = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMERA_DIRECTION_X, info, 5, 600, 64, color);
	if( !result )
		return false;

	//_itoa_s((int)camPosy, tempStr, 10);
	sprintf_s(tempStr, "%.2f", camera_direction_y);
	strcpy_s(info, "Camera direction y = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMERA_DIRECTION_Y, info, 5, 620, 64, color);
	if( !result )
		return false;

	//_itoa_s((int)camPosz, tempStr, 10);
	sprintf_s(tempStr, "%.2f", camera_direction_z);
	strcpy_s(info, "Camera direction z = ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		CAMERA_DIRECTION_Z, info, 5, 640, 64, color);
	if( !result )
		return false;

	_itoa_s(this->m_frustum_counts, tempStr, 10);
	strcpy_s(info, "Bounding sphere clipping - ");
	strcat_s(info, tempStr);

	result = this->m_pText->Text(this->m_pGraphics->GetD3DDevice(), this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		RENDEREDOBJS, info, 5, 220, 64, color);
	if( !result )
		return false;

	return true;
}

bool CAppDriver::_base_coms_init(bool fullScreen, bool vsyncEnable, int width, int height)
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
	/////////////////////////////////// Audio ///////////////////////////////////

	/////////////////////////////////// Graph ///////////////////////////////////
	this->m_pGraphics = new CGEGraphics;
	if( !this->m_pGraphics )
		return false;

	result = this->m_pGraphics->Initialize(fullScreen, vsyncEnable, width, height, this->m_hWnd);
	if( !result )
		return false;
	/////////////////////////////////// Graph ///////////////////////////////////

	return true;
}

bool CAppDriver::_graphics_coms_init(int width, int height)
{
	bool result;
	/////////////////////////////////// Camera //////////////////////////////////
	this->m_pCamera = new CGECamera;
	if( !this->m_pCamera )
		return false;
	this->m_camera_position = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	this->m_pCamera->SetPosition(this->m_camera_position.x, this->m_camera_position.y, this->m_camera_position.z);
	this->m_pCamera->Run();
	this->m_pCamera->GetViewMatrix(this->m_baseView);
	this->Display_info_on_console_window(L"Camera startup\n");
	/////////////////////////////////// Camera //////////////////////////////////

	////////////////////////////////// Text /////////////////////////////////////
	this->m_pText = new CGEText;
	if( !this->m_pText )
		return false;

	result = this->m_pText->Initialize(this->m_pGraphics->GetD3DDevice(),
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		this->m_hWnd, width, height, this->m_baseView);
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
	this->Display_info_on_console_window(L"Font Engine startup\n");
	////////////////////////////////// Text //////////////////////////////////

	////////////////////////////// MultiTexture //////////////////////////////
	this->m_pTextureSequence = new CGEMultiTextureSequence;
	if( !this->m_pTextureSequence )
		return false;

	result = this->m_pTextureSequence->Initialize(this->m_pGraphics->GetD3DDevice(), 
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
	if( !result )
		return false;
	this->Display_info_on_console_window(L"MultiTexture module startup\n");
	////////////////////////////// MultiTexture //////////////////////////////


	////////////////////////////// Models Manager ////////////////////////////
	result = this->m_modelsManager.Initialize(this->m_pGraphics->GetD3DDevice(), 
		this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer());
	if( !result )
		return false;
	this->Display_info_on_console_window(L"Models manager startup\n");
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
	this->Display_info_on_console_window(L"Camera movement manager startup\n");
	///////////////////////// Camera Movement Manager ////////////////////////

	///////////////////////////////// Frustum ////////////////////////////////
	this->m_pFrustum = new CGEFrustum;
	if( !this->m_pFrustum )
		return false;
	this->Display_info_on_console_window(L"Frustum startup\n");
	///////////////////////////////// Frustum ///////////////////////////////

	////////////////////////////// Shader Debugger //////////////////////////
	this->m_pGraphics->CreateShaderDebugger(this->m_pGraphics->GetD3DDevice(), this->m_hWnd,
		width, height, 320, 180);
	if( !result )
		return false;
	this->Display_info_on_console_window(L"Render target startup, at position 320, 180\n");
	////////////////////////////// Shader Debugger //////////////////////////

	//////////////////////////// RTT For Reflection /////////////////////////
	this->m_pRTT_Usage = new CGERenderToTexture;
	if( !this->m_pRTT_Usage )
		return false;

	result = this->m_pRTT_Usage->Initialize(this->m_pGraphics->GetD3DDevice(), width, height);
	if( !result )
		return false;
	this->Display_info_on_console_window(L"One render target startup\n");
	//////////////////////////// RTT For Reflection /////////////////////////

	return true;
}

bool CAppDriver::_shader_init()
{
	bool result;

	////////////////////////////// Light Shader ///////////////////////////
	this->m_pLightShader = new CGELightShader;
	if( !this->m_pLightShader )
		return false;

	result = this->m_pLightShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	////////////////////////////// Light Shader ///////////////////////////

	////////////////////////////// MultiTexture //////////////////////////////
	this->m_pMultiTextureShader = new CGEMultiTextureShader;
	if( !this->m_pMultiTextureShader )
		return false;

	result = this->m_pMultiTextureShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;

	////////////////////////////// MultiTexture //////////////////////////////

	////////////////////////////// Light Map /////////////////////////////////
	this->m_pLightMapShader = new CGELightMapShader;
	if( !this->m_pLightMapShader )
		return false;

	result = this->m_pLightMapShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	////////////////////////////// Light Map /////////////////////////////////

	////////////////////////////// Alpha Map /////////////////////////////////
	this->m_pAlphaMapShader = new CGEAlphaMapShader;
	if( !this->m_pAlphaMapShader )
		return false;

	result = this->m_pAlphaMapShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	////////////////////////////// Alpha Map /////////////////////////////////

	/////////////////////////////// Bump Map /////////////////////////////////
	this->m_pBumpMapShader = new CGEBumpMapShader;
	if( !this->m_pBumpMapShader )
		return false;

	result = this->m_pBumpMapShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	/////////////////////////////// Bump Map /////////////////////////////////

	/////////////////////////////// Specular Map /////////////////////////////
	this->m_pSpecularMapShader = new CGESpecularMappingShader;
	if( !this->m_pSpecularMapShader )
		return false;

	result = this->m_pSpecularMapShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	/////////////////////////////// Specular Map /////////////////////////////

	/////////////////////////////// Texture shader ///////////////////////////
	this->m_pTextureShader = new CGETextureShader;
	if( !this->m_pTextureShader )
		return false;

	result = this->m_pTextureShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	/////////////////////////////// Texture shader ///////////////////////////

	////////////////////////////////// Fog shader ////////////////////////////
	this->m_pFogShader = new CGEFogShader;
	if( !this->m_pFogShader )
		return false;

	result = this->m_pFogShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	////////////////////////////////// Fog shader ////////////////////////////

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

	/////////////////////// Texture Transparency shader //////////////////////
	this->m_pTransparencyShader = new CGETransparencyShader;
	if( !this->m_pTransparencyShader )
		return false;

	result = this->m_pTransparencyShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	/////////////////////// Texture Transparency shader //////////////////////

	///////////////////////////// Reflection shader //////////////////////////
	this->m_p_Usage_Shader = new CGEReflectionShader;
	if( !this->m_p_Usage_Shader )
		return false;
	
	result = this->m_p_Usage_Shader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
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

	////////////////////////// Multi Point Light Info ////////////////////////
	this->m_pMultiPointLightShader = new CGEMultiPointLightShader;
	if( !this->m_pMultiPointLightShader )
		return false;

	result = this->m_pMultiPointLightShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	////////////////////////// Multi Point Light Info ////////////////////////

	//////////////////////////////// Glass ///////////////////////////////////
	this->m_pGlassShader = new CGEGlassShader;
	if( !this->m_pGlassShader )
		return false;

	result = this->m_pGlassShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	//////////////////////////////// Glass ///////////////////////////////////

	///////////////////////////////// Fire ///////////////////////////////////
	this->m_pFireShader = new CGEFireShader;
	if( !this->m_pFireShader )
		return false;

	result = this->m_pFireShader->Initialize(this->m_pGraphics->GetD3DDevice(), this->m_hWnd);
	if( !result )
		return false;
	///////////////////////////////// Fire ///////////////////////////////////

	return true;
}

bool CAppDriver::_utilities_init()
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

void CAppDriver::_shutdown()
{
	::Shutdown<CGEFireShader*>(this->m_pFireShader);
	::Shutdown<CGEGlassShader*>(this->m_pGlassShader);
	::Shutdown<CGEMultiPointLightShader*>(this->m_pMultiPointLightShader);
	::Shutdown<CGEWaterShader*>(this->m_pWaterShader);
	::Shutdown<CGEFadeShader*>(this->m_pFadeShader);
	::Shutdown<CGERenderToTexture*>(this->m_pRTT_Usage);
	::Shutdown<CGEReflectionShader*>(this->m_p_Usage_Shader);
	::Shutdown<CGETransparencyShader*>(this->m_pTransparencyShader);
	::Shutdown<CGETextureTranslate*>(this->m_pTextureTranslate);
	::Shutdown<CGEClipPlaneShader*>(this->m_pClipPlaneShader);
	::Shutdown<CGEFogShader*>(this->m_pFogShader);
	::Shutdown<CGESpecularMappingShader*>(this->m_pSpecularMapShader);
	::Shutdown<CGETextureShader*>(this->m_pTextureShader);
	::Shutdown<CGEBumpMapShader*>(this->m_pBumpMapShader);
	::Shutdown<CGEAlphaMapShader*>(this->m_pAlphaMapShader);
	::Shutdown<CGELightMapShader*>(this->m_pLightMapShader);
	::Shutdown<CGEMultiTextureShader*>(this->m_pMultiTextureShader);
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

	::CloseHandle(this->m_hConsoleWindow);
}

bool CAppDriver::_scene_objs_init()
{
	return true;
}

void CAppDriver::_matrix_process()
{
	this->m_pCamera->Run();
	this->m_pCamera->GetViewMatrix(this->m_baseView);
	this->m_pGraphics->GetD3DObj()->GetWorldMatrix(this->m_text_WorldMat);
	this->m_pGraphics->GetD3DObj()->GetOrthoMatrix(this->m_text_orthoMat);
}

bool CAppDriver::_frame()
{
	bool result;

	this->_pre_run();
	this->_input_process();

	D3DXVECTOR3 positionCam = this->m_pCamera->GetPosition();

	this->m_pAudio->SetListenerPosition(positionCam.x, positionCam.y, positionCam.z);

	this->m_pGraphics->BeginRender(D3DXCOLOR(0.02f, 0.02f, 0.02f, 1.0f));

	result = this->_render_objs();
	if( !result )
	{
		MessageBox(this->m_hWnd, L"There must at least 1 obj render failed ! please check function 'CWorking::_render_objs()'",
			L"Error", MB_OK);
		return false;
	}

	this->m_pGraphics->EndRender();

	return true;
}

void CAppDriver::_pre_run()
{
	// Now working !
	this->m_pDXInput->Run();
	this->m_pCPU_info->Run();
	this->m_pFPS_info->Run();
	this->m_pTimer->Run();

	// Show device information
	this->Display_device_information();
}

void CAppDriver::_input_process()
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

	this->m_camera_position.x = this->m_pCameraMoveMgr->GetLeftRightPos();
	this->m_camera_position.y = this->m_pCameraMoveMgr->GetUpDownPos();
	this->m_camera_position.z = this->m_pCameraMoveMgr->GetMoveForwardBackPos();
	
	this->m_pCamera->SetPosition(this->m_camera_position.x, this->m_camera_position.y, this->m_camera_position.z);
	this->m_pCamera->SetRotation(rotX, rotY, 0.0f);
}

bool CAppDriver::_render_objs()
{
	this->_matrix_process();

	bool result;
	//******************* Text *******************
	this->m_pGraphics->TurnOffZBuffer();
	this->m_pGraphics->TurnOnAlphaBlending();

	// Text engine run !
	result = this->m_pText->Render(this->m_pGraphics->GetD3DObj()->GetDeviceContextPointer(),
		this->m_text_WorldMat, this->m_text_orthoMat);
	if( !result )
		return false;

	this->m_pGraphics->TurnOffAlphaBlending();
	this->m_pGraphics->TurnOnZBuffer();
	//******************* Text *******************
	return true;
}