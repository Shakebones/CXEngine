/*
 * File name - AppEngine.h
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc - 
 *
 */
#ifndef _APP_ENGINE_H_
#define _APP_ENGINE_H_

#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
#include <Windows.h>

// Graphics
#include "GEGraphics.h"

// Input
#include "GEInput.h"
#include "GEDXInput.h"

// Sound
#include "GESound.h"

// Tools
#include "Utility.h"
#endif

/*
 * Class name - CGEAppEngine
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc -
 * 
 * Fully drives Win32 App
 */
class CGEAppEngine
{
public:
	// Interfaces

	// Constructors & destructor
	CGEAppEngine();
	CGEAppEngine(const CGEAppEngine&);
	CGEAppEngine& operator=(const CGEAppEngine&);
	~CGEAppEngine();

	// Working Interfaces
	virtual bool	Initialize(bool is_full_screen, bool vsyncEnable, int width, int height);
	virtual void	Shutdown();
	virtual void	Run();

	virtual LRESULT CALLBACK MsgHandler(HWND, UINT, WPARAM, LPARAM);

protected:
	virtual bool	_frame();
	virtual bool	_initWindows(bool, int&, int&);
	virtual void	_shutdown_windows();

protected:

	// COM Members
	CGEGraphics*	m_pGraphics;
	CGEDXInput*		m_pDXInput;

	CGEInput*		m_pInput;
	CGESound*		m_pAudio;
	CUtility_cpu*	m_pCPU_info;
	CUtility_fps*	m_pFPS_info;
	CUtility_timer*	m_pTimer;

	LPCWSTR		m_pAppName;
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
};

// Message function declaration
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global static pointer
static CGEAppEngine* ApplicationHandle = 0;
#endif