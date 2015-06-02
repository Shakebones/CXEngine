/*
 * File name - GEFrustum.h
 * Author - CuiXue
 * Updated date - 2013.2.12
 * Desc - 
 *
 */
#ifndef _GE_FRUSTUM_H_
#define _GE_FRUSTUM_H_

#ifdef _WIN32
#include "Utility.h"
#include <D3DX10math.h>
#endif

/*
 * Class name - CGEFrustum
 * Author - CuiXue
 * Updated date - 2013.2.12
 * Desc - 
 *
 */
class CGEFrustum
{
public:
	// Interfaces
	CGEFrustum();
	CGEFrustum(const CGEFrustum&);
	CGEFrustum& operator=(const CGEFrustum&);
	~CGEFrustum();

public:
	bool ConstructFrustum(float, D3DXMATRIX, D3DXMATRIX);

	// Check point/box/sphere/etc.
	bool CheckPoint(float, float, float);
	bool CheckBox(float, float, float, float);
	bool CheckBox(float, float, float,
				  float, float, float);
	bool CheckSphere(float, float, float, float);

private:
	D3DXPLANE m_planes[6];
};

#endif