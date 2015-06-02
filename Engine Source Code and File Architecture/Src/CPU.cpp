/*
 * File name - CPU.cpp
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
#include "CPU.h"

CUtility_cpu::CUtility_cpu()
{
}

CUtility_cpu::CUtility_cpu(const CUtility_cpu& source)
{
}

CUtility_cpu& CUtility_cpu::operator=(const CUtility_cpu& source)
{
	return *this;
}

CUtility_cpu::~CUtility_cpu()
{
}

bool CUtility_cpu::Initialize()
{
	PDH_STATUS status;

	this->m_access_of_cpu = true;

	status = PdhOpenQuery(NULL, 0, &this->m_queryHandle);
	if( status != ERROR_SUCCESS )
		this->m_access_of_cpu = false;

	status = PdhAddCounter(this->m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &this->m_counterHandle);
	if( status != ERROR_SUCCESS )
		this->m_access_of_cpu = false;

	this->m_lastSampleTime = ::GetTickCount();
	this->m_cpu_utilization = 0;

	return true;
}

void CUtility_cpu::Shutdown()
{
	if( this->m_access_of_cpu )
	{
		PdhCloseQuery(this->m_queryHandle);
	}
}

void CUtility_cpu::Run()
{
	PDH_FMT_COUNTERVALUE counterValue;

	if( this->m_access_of_cpu )
	{
		if( this->m_lastSampleTime + 1000 < ::GetTickCount() )
		{
			this->m_lastSampleTime = ::GetTickCount();
			::PdhCollectQueryData(this->m_queryHandle);
			::PdhGetFormattedCounterValue(this->m_counterHandle, PDH_FMT_LONG, NULL, &counterValue);
			this->m_cpu_utilization = counterValue.longValue;
		}
	}
}

int CUtility_cpu::GetUtilization()
{
	int utilization;

	if( this->m_access_of_cpu )
	{
		utilization = (int)this->m_cpu_utilization;
	}

	else
	{
		utilization = 0;
	}

	return utilization;
}

