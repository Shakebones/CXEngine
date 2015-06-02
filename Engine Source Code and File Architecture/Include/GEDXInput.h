/*
 * File name - GEDXInput.h
 * Author - CuiXue
 * Updated date - 2013.1.20
 * Desc - 
 *
 */
#ifndef _GE_DXINPUT_H_
#define _GE_DXINPUT_H_

#define DIRECTINPUT_VERSION 0x0800

#ifdef _WIN32
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include "Utility.h"
#endif

/*
 * Class name - CGEDXInput
 * Author - CuiXue
 * Updated date - 2013.1.20
 * Desc - 
 *
 */
class CGEDXInput
{
public:
	// Interfaces
	CGEDXInput();
	CGEDXInput(const CGEDXInput&);
	CGEDXInput& operator=(const CGEDXInput&);
	~CGEDXInput();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Run();
	
	bool IsKeyPressed(int);
	void GetMouseLocation(int&, int&);

private:
	// Kernels
	bool _read_keyboard();
	bool _read_mouse();
	void _proccess_input();

private:
	IDirectInput8* m_pDirectInput;
	IDirectInputDevice8* m_pKeyboard;
	IDirectInputDevice8* m_pMouse;
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth;
	int m_screenHeight;
	int m_mouseX;
	int m_mouseY;

	D3DXVECTOR2 m_wnd_min_pos;
	D3DXVECTOR2 m_wnd_max_pos;
};

#endif