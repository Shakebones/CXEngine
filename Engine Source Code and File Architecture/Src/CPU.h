/*
 * File name - CPU.h
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
#ifndef _CPU_H_
#define _CPU_H_

#ifdef _WIN32
#include <Pdh.h>

#pragma comment(lib, "pdh.lib")
#endif

/*
 * Class name - CUtility_cpu
 * Author - CuiXue
 * Updated date - 2013.2.1
 * Desc - 
 *
 */
class CUtility_cpu
{
public:
	CUtility_cpu();
	CUtility_cpu(const CUtility_cpu&);
	CUtility_cpu& operator=(const CUtility_cpu&);
	~CUtility_cpu();

public:
	bool Initialize();
	void Run();
	void Shutdown();
	int GetUtilization();

private:
	bool m_access_of_cpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpu_utilization;
};
#endif