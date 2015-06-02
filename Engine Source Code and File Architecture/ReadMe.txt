The Engine is organized as the regular library file. Compiling the engine will give the user static library.
And include the header file in a specific application, and then connect to the library by typing:

#pragma comment(lib, "LibraryPath.lib")

A simple example for start up:

#include "AppDriver.h"

#pragma comment(lib, "Engine.lib")

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

I had been responsiblly modifying the engine until last year December since the studio closed at that time.
And then I keep modifying by myself when I was free. The last time modifying is about the tech
called Screen Space Tessellation, it was an idea extracted from Screen Space Ambient Oculussion.
The references could be the examples from DirectX SDK