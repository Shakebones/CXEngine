/*
 * File name - AppEngine.cpp
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc
 *
 * Definition
 */
#include "AppEngine.h"

CGEAppEngine::CGEAppEngine()
{
	this->m_pDXInput = NULL;
	this->m_pInput = NULL;
	this->m_pAudio = NULL;
	this->m_pGraphics = NULL;
	this->m_hInstance = NULL;
	this->m_hWnd = NULL;
}

CGEAppEngine::CGEAppEngine(const CGEAppEngine& source)
{
}

CGEAppEngine& CGEAppEngine::operator=(const CGEAppEngine& source)
{
	return *this;
}

CGEAppEngine::~CGEAppEngine()
{
}

bool CGEAppEngine::Initialize(bool is_full_screen, bool vsyncEnable, int width, int height)
{
	int tmpSCWidth;
	int tmpSCHeight;
	bool result;

	tmpSCWidth	= width;
	tmpSCHeight = height;

#ifdef _WIN32
	result = this->_initWindows(is_full_screen, tmpSCWidth, tmpSCHeight);
	if( !result )
		return false;
#endif

	// Initialize  COMs
	this->m_pInput = new CGEInput;
	if( !this->m_pInput )
		return false;

	result = this->m_pInput->Initialize();
	if( !result )
		return false;

	this->m_pGraphics = new CGEGraphics;
	if( !this->m_pGraphics )
		return false;

	result = this->m_pGraphics->Initialize(is_full_screen, vsyncEnable, tmpSCWidth, tmpSCHeight, this->m_hWnd);
	if( !result )
		return false;

	return true;
}

void CGEAppEngine::Shutdown()
{
	::Shutdown<CGEGraphics*>(this->m_pGraphics);
	::Shutdown<CGEInput*>(this->m_pInput);

	this->_shutdown_windows();
}

void CGEAppEngine::Run()
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
	}
}

bool CGEAppEngine::_frame()
{
	bool result;

	if( this->m_pInput->IsKeyDown(VK_ESCAPE) )
		return false;

	result = this->m_pGraphics->BeginRender(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	if( !result )
		return false;

	result = this->m_pGraphics->EndRender();
	if( !result )
		return false;

	return true;
}

LRESULT CALLBACK CGEAppEngine::MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CGEAppEngine::_initWindows(bool is_full_screen, int& width, int& height)
{
	if( 0 == width || 0 == height )
		return false;

	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX;
	int posY;

	::ApplicationHandle = this;
	this->m_hInstance = ::GetModuleHandle(NULL);
	this->m_pAppName = L"GE Engine";

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = ::LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hInstance = this->m_hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = this->m_pAppName;
	wc.lpszMenuName = this->m_pAppName;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if( !::RegisterClassEx(&wc) )
		return false;

	if( is_full_screen )
	{
		//Default value of full screen mode.
		//width = ::GetSystemMetrics(SM_CXSCREEN);
		//height = ::GetSystemMetrics(SM_CYSCREEN);

		::memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned int)width;
		dmScreenSettings.dmPelsHeight = (unsigned int)height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		::ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = posY = 0;

		this->m_hWnd = ::CreateWindowEx(WS_EX_APPWINDOW, this->m_pAppName, this->m_pAppName,
			WS_POPUP, posX, posY, width, height, NULL, NULL, this->m_hInstance, NULL);

		if( !this->m_hWnd )
			return false;
	}

	else
	{
		posX = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		posY = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		// 注意 - width 和 height 已经加上了因为win32窗体引起的像素挤压造成的模糊现象的误差值
		this->m_hWnd = ::CreateWindowEx(WS_EX_APPWINDOW, this->m_pAppName, this->m_pAppName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU, posX, posY, width + 16, height + 39, NULL, NULL, this->m_hInstance, NULL);

		if( !this->m_hWnd )
			return false;
	}

	::ShowWindow(this->m_hWnd, SW_SHOW);
	::SetForegroundWindow(this->m_hWnd);
	::SetFocus(this->m_hWnd);
	::ShowCursor(true);

	return true;
}

void CGEAppEngine::_shutdown_windows()
{
	::ShowCursor(true);
	if( FULL_SCREEN )
		::ChangeDisplaySettings(NULL, 0);

	::DestroyWindow(this->m_hWnd);
	this->m_hWnd = NULL;
	::ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_CLOSE:
		::PostQuitMessage(0);
		break;

	default:
		return ::ApplicationHandle->MsgHandler(hWnd, msg, wParam, lParam);
	}

	return 0;
}