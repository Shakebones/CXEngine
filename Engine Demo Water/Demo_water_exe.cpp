/*
 * File name - Main_test.cpp
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc -
 *
 * Win32 Application for testing Micro 3D Engine
 */
#include "Demo_water.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdLine, int iCmdShow)
{
	CGEAppEngine *Engine = new CWorking;
	if( !Engine )
		return 0;

	if( !Engine->Initialize(false, true, 1440, 720) )
		return 0;

	Engine->Run();
	Shutdown<CGEAppEngine*>(Engine);

	return 0;
}