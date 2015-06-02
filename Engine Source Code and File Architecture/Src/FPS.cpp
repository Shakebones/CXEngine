/*
 * File name - FPS.cpp
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
#include "FPS.h"

CUtility_fps::CUtility_fps()
{
}

CUtility_fps::CUtility_fps(const CUtility_fps& source)
{
}

CUtility_fps& CUtility_fps::operator=(const CUtility_fps& source)
{
	return *this;
}

CUtility_fps::~CUtility_fps()
{
}

bool CUtility_fps::Initialize()
{
	this->m_count = 0;
	this->m_startTime = ::timeGetTime();
	this->m_fps = 0;

	return true;
}

void CUtility_fps::Run()
{
	this->m_count++;
	if( timeGetTime() >= (this->m_startTime + 1000) )
	{
		this->m_fps = this->m_count;
		this->m_count = 0;
		this->m_startTime = timeGetTime();
	}
}

int CUtility_fps::GetFPS()
{
	return this->m_fps;
}