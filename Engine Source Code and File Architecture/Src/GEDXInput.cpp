/*
 * File name - GEDXInput.cpp
 * Author - CuiXue
 * Updated date - 2013.1.20
 * Desc - 
 *
 */
#include "GEDXInput.h"

CGEDXInput::CGEDXInput()
{
	this->m_pDirectInput = 0;
	this->m_pKeyboard = 0;
	this->m_pMouse = 0;
}

CGEDXInput::CGEDXInput(const CGEDXInput& source)
{
}

CGEDXInput& CGEDXInput::operator=(const CGEDXInput& source)
{
	return *this;
}

CGEDXInput::~CGEDXInput()
{
}

bool CGEDXInput::Initialize(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
	HRESULT result;

	this->m_screenHeight = height;
	this->m_screenWidth = width;

	this->m_mouseX = 0;
	this->m_mouseY = 0;

	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&this->m_pDirectInput, NULL);
	if( FAILED(result) )
		return false;

	/////////////////////// Keyboard ///////////////////////
	result = this->m_pDirectInput->CreateDevice(GUID_SysKeyboard, &this->m_pKeyboard, NULL);
	if( FAILED(result) )
		return false;

	result = this->m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if( FAILED(result) )
		return false;

	result = this->m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if( FAILED(result) )
		return false;

	result = this->m_pKeyboard->Acquire();
	if( FAILED(result) )
		return false;
	/////////////////////// Keyboard ///////////////////////

	/////////////////////// Mouse ///////////////////////
	result = this->m_pDirectInput->CreateDevice(GUID_SysMouse, &this->m_pMouse, NULL);
	if( FAILED(result) )
		return false;

	result = this->m_pMouse->SetDataFormat(&c_dfDIMouse);
	if( FAILED(result) )
		return false;

	result = this->m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	result = this->m_pMouse->Acquire();
	if( FAILED(result) )
		return false;
	/////////////////////// Mouse ///////////////////////

	return true;
}

void CGEDXInput::Shutdown()
{
	::ReleaseDXInput<IDirectInputDevice8*>(this->m_pMouse);
	::ReleaseDXInput<IDirectInputDevice8*>(this->m_pKeyboard);
	::ReleaseWithoutDel<IDirectInput8*>(this->m_pDirectInput);
}

bool CGEDXInput::Run()
{
	bool result;

	result = this->_read_keyboard();
	if( !result )
		return false;

	result = this->_read_mouse();
	if( !result )
		return false;

	this->_proccess_input();

	return true;
}

bool CGEDXInput::_read_keyboard()
{
	HRESULT result;

	result = this->m_pKeyboard->GetDeviceState(sizeof(this->m_keyboardState), (LPVOID)&this->m_keyboardState);
	if( FAILED(result) )
	{
		if( result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED )
			this->m_pKeyboard->Acquire();

		else
			return false;
	}

	return true;
}

bool CGEDXInput::_read_mouse()
{
	HRESULT result;

	result = this->m_pMouse->GetDeviceState(sizeof(this->m_mouseState), (LPVOID)&this->m_mouseState);
	if( FAILED(result) )
	{
		if( result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED )
			this->m_pMouse->Acquire();

		else
			return false;
	}

	return true;
}

/*
 * 函数记录鼠标坐标存在问题如下：
 * 1.无法正确显示鼠标相对于当前应用程序窗口的位置
 * 2.原始位置不正确
 */
void CGEDXInput::_proccess_input()
{
	this->m_mouseX += this->m_mouseState.lX;
	this->m_mouseY += this->m_mouseState.lY;

	if( this->m_mouseX < 0 )
		this->m_mouseX = 0;

	if( this->m_mouseY < 0 )
		this->m_mouseY = 0;

	if( this->m_mouseX > this->m_screenWidth )
		this->m_mouseX = this->m_screenWidth;

	if( this->m_mouseY > this->m_screenHeight )
		this->m_mouseY = this->m_screenHeight;
}

bool CGEDXInput::IsKeyPressed(int key)
{
	if( this->m_keyboardState[key] & 0x80 )
		return true;

	return false;
}

void CGEDXInput::GetMouseLocation(int& mx, int& my)
{
	mx = this->m_mouseX;
	my = this->m_mouseY;
}