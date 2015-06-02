/*
 * File name - TIMER.h
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef _WIN32
#include <Windows.h>
#endif

/*
 * Class name - CUtility_timer
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
class CUtility_timer
{
public:
	// Interfaces
	CUtility_timer();
	CUtility_timer(const CUtility_timer&);
	CUtility_timer& operator=(const CUtility_timer&);
	~CUtility_timer();

public:
	bool Initialize();
	void Run();
	float GetTime();

private:
	__int64 m_frequency;
	float	m_ticksPer_ms;
	__int64 m_startTime;
	float	m_frameTime;
};
#endif