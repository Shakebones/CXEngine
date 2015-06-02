/*
 * File name - AppExe.cpp
 * Author - CuiXue
 * Updated date - 2013.7.13
 * Desc - 
 *
 */
#include "AppDriver.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdLine, int iCmdShow)
{
	CGEAppEngine* pDriver = new CAppDriver;
	if( !pDriver )
	{
		MessageBox(NULL, L"Cannot create a new instantce !", L"Error", MB_OK);
		return 0;
	}

	if( !pDriver->Initialize(false, true, 1280, 720) )
	{
		MessageBox(NULL, L"Cannot initialize Engine !", L"Error", MB_OK);
		return 0;
	}

	// Application loop
	pDriver->Run();

	Shutdown<CGEAppEngine*>(pDriver);

	return 0;
}