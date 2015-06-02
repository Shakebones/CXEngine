/*
 * File name - TIMER.cpp
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
#include "TIMER.h"

CUtility_timer::CUtility_timer()
{
}

CUtility_timer::CUtility_timer(const CUtility_timer& source)
{
}

CUtility_timer& CUtility_timer::operator=(const CUtility_timer& source)
{
	return *this;
}

CUtility_timer::~CUtility_timer()
{
}

bool CUtility_timer::Initialize()
{
	::QueryPerformanceFrequency((LARGE_INTEGER*)&this->m_frequency);
	if( 0 == this->m_frequency )
		return false;

	this->m_ticksPer_ms = (float)(this->m_frequency / 1000);
	
	::QueryPerformanceCounter((LARGE_INTEGER*)&this->m_startTime);

	return true;
}

void CUtility_timer::Run()
{
	__int64 curTime;
	float timeDelta;

	::QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
	timeDelta = (float)(curTime - this->m_startTime);

	this->m_frameTime = timeDelta / this->m_ticksPer_ms;
	
	this->m_startTime = curTime;
}

float CUtility_timer::GetTime()
{
	return this->m_frameTime;
}