/*
 * File name - GECamera.h
 * Author - CuiXue
 * Updated date - 2012.12.12
 * Desc - 
 *
 */
#ifndef _GE_CAMERA_H_
#define _GE_CAMERA_H_

#ifdef _WIN32
#include <D3DX10math.h>
#endif

/*
 * Class name - CGECamera
 * Author - CuiXue
 * Updated date - 2012.12.12
 * Desc - 
 *
 */
class CGECamera
{
public:
	CGECamera();
	CGECamera(const CGECamera&);
	CGECamera& operator=(const CGECamera&);
	~CGECamera();

public:
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();
	D3DXVECTOR3 GetDirection();

	void Run();
	void CalculateReflectionMatrix(float);
	void GetViewMatrix(D3DXMATRIX&);
	D3DXMATRIX GetReflectionViewMatrix();

private:
	float m_posX, m_posY, m_posZ;
	float m_rotX, m_rotY, m_rotZ;

	D3DXVECTOR3 m_camera_direction;

	D3DXMATRIX m_viewMatrix;
	
	D3DXMATRIX m_reflectionMatrix;
};

#endif