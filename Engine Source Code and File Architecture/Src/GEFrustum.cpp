/*
 * File name - GEFrustum.cpp
 * Author - CuiXue
 * Updated date - 2013.2.23
 * Desc - 
 *
 */
#include "GEFrustum.h"

CGEFrustum::CGEFrustum()
{
}

CGEFrustum::CGEFrustum(const CGEFrustum& srouce)
{
}

CGEFrustum& CGEFrustum::operator=(const CGEFrustum& srouce)
{
	return *this;
}

CGEFrustum::~CGEFrustum()
{
}

// Desc -
// 根据 projection matrix （投影变换矩阵）、view matrix（视图变换矩阵）
// 决定当前视野的视域体，并根据物体的外截体作裁剪
bool CGEFrustum::ConstructFrustum(float scDepth, D3DXMATRIX projectionMat, D3DXMATRIX viewMat)
{
	float minDepth;
	float r;

	D3DXMATRIX matrix;

	minDepth	= -projectionMat._43 / projectionMat._33;
	r			= scDepth / (scDepth - minDepth);
	projectionMat._33 = r;
	projectionMat._43 = -r * minDepth;

	::D3DXMatrixMultiply(&matrix, &viewMat, &projectionMat);
	
	// Near
	this->m_planes[0].a = matrix._14 + matrix._13;
	this->m_planes[0].b = matrix._24 + matrix._23;
	this->m_planes[0].c = matrix._34 + matrix._33;
	this->m_planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&this->m_planes[0], &this->m_planes[0]);

	// Far
	this->m_planes[1].a = matrix._14 - matrix._13;
	this->m_planes[1].b = matrix._24 - matrix._23;
	this->m_planes[1].c = matrix._34 - matrix._33;
	this->m_planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&this->m_planes[1], &this->m_planes[1]);

	// Left
	this->m_planes[2].a = matrix._14 + matrix._11;
	this->m_planes[2].b = matrix._24 + matrix._21;
	this->m_planes[2].c = matrix._34 + matrix._31;
	this->m_planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&this->m_planes[2], &this->m_planes[2]);

	// Right
	this->m_planes[3].a = matrix._14 - matrix._11;
	this->m_planes[3].b = matrix._24 - matrix._21;
	this->m_planes[3].c = matrix._34 - matrix._31;
	this->m_planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&this->m_planes[3], &this->m_planes[3]);

	// Top;
	this->m_planes[4].a = matrix._14 - matrix._12;
	this->m_planes[4].b = matrix._24 - matrix._22;
	this->m_planes[4].c = matrix._34 - matrix._32;
	this->m_planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&this->m_planes[4], &this->m_planes[4]);

	// Bottom
	this->m_planes[5].a = matrix._14 + matrix._12;
	this->m_planes[5].b = matrix._24 + matrix._22;
	this->m_planes[5].c = matrix._34 + matrix._32;
	this->m_planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&this->m_planes[5], &this->m_planes[5]);

	return true;
}

bool CGEFrustum::CheckSphere(float x, float y, float z, float radius)
{
	for(int i = 0; i != 6; i++)
	{
		if( D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3(x, y, z)) < -radius)
		{
			return false;
		}
	}

	return true;
}

bool CGEFrustum::CheckPoint(float x, float y, float z)
{
	for(int i = 0; i != 6; i++)
	{
		if( D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3(x, y, z)) < 0.0f )
		{
			return false;
		}
	}
	return true;
}

bool CGEFrustum::CheckBox(float x, float y, float z, float radius)
{
	for(int i = 0; i != 6; i++)
	{
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x - radius), (y - radius), (z - radius))) >= 0.0f)
		{
			continue;
		}
		
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x + radius), (y - radius), (z - radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x - radius), (y + radius), (z - radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x + radius), (y + radius), (z - radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x - radius), (y - radius), (z + radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x + radius), (y - radius), (z + radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x - radius), (y + radius), (z + radius))) >= 0.0f)
		{
			continue;
		}
		
		if(D3DXPlaneDotCoord(&m_planes[i], &D3DXVECTOR3((x + radius), (y + radius), (z + radius))) >= 0.0f)
		{
			continue;
		}
	}

	return true;
}

bool CGEFrustum::CheckBox(float xCenter, float yCenter, float zCenter,
							   float xSize, float ySize, float zSize)
{
	for(int i = 0; i != 6; i++)
	{
		if( D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) > 0.0f )
		{
			continue;
		}

		if( D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&this->m_planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}
	}

	return true;
}