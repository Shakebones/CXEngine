/*
 * File name - FPS.h
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
#ifndef _FPS_H_
#define _FPS_H_

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
#endif

/*
 * Class name - CUtility_fps
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
class CUtility_fps
{
public:
	// Interfaces
	CUtility_fps();
	CUtility_fps(const CUtility_fps&);
	CUtility_fps& operator=(const CUtility_fps&);
	~CUtility_fps();

public:
	bool Initialize();
	void Run();
	int GetFPS();

private:
	int m_fps;
	int m_count;
	unsigned long m_startTime;
};

#endif