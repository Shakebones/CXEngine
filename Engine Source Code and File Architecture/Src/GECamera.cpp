/*
 * File name - GECamera.cpp
 * Author - CuiXue
 * Updated date 2012.12.12
 * Desc -
 *
 */
#include "GECamera.h"

CGECamera::CGECamera()
{
	this->m_posX = this->m_posY = this->m_posZ = 0.0f;
	this->m_rotX = this->m_rotY = this->m_rotZ = 0.0f;
}

CGECamera::CGECamera(const CGECamera& source)
{
}

CGECamera& CGECamera::operator=(const CGECamera& source)
{
	return *this;
}

CGECamera::~CGECamera()
{
}

D3DXVECTOR3 CGECamera::GetPosition()
{
	return D3DXVECTOR3(this->m_posX, this->m_posY, this->m_posZ);
}

D3DXVECTOR3 CGECamera::GetDirection()
{
	return this->m_camera_direction;
}

D3DXVECTOR3 CGECamera::GetRotation()
{
	return D3DXVECTOR3(this->m_rotX, this->m_rotY, this->m_rotZ);
}

void CGECamera::SetPosition(float x, float y, float z)
{
	this->m_posX = x;
	this->m_posY = y;
	this->m_posZ = z;
}

void CGECamera::SetRotation(float x, float y, float z)
{
	this->m_rotX = x;
	this->m_rotY = y;
	this->m_rotZ = z;
}

void CGECamera::Run()
{
	D3DXMATRIX	rotationMatrix;
	D3DXVECTOR3 position;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;

	float pitch, yaw, roll;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	position.x = this->m_posX;
	position.y = this->m_posY;
	position.z = this->m_posZ;

	target.x = 0.0f;
	target.y = 0.0f;
	target.z = 1.0f;

	pitch = this->m_rotX * 0.0174532925f;
	yaw   = this->m_rotY * 0.0174532925f;
	roll  = this->m_rotZ * 0.0174532925f;

	::D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	::D3DXVec3TransformCoord(&target, &target, &rotationMatrix);
	::D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	target += position;

	::D3DXMatrixLookAtLH(&this->m_viewMatrix, &position, &target, &up);

	this->m_camera_direction = D3DXVECTOR3(this->m_viewMatrix._13, this->m_viewMatrix._23, this->m_viewMatrix._33);

	return;
}

void CGECamera::CalculateReflectionMatrix(float height)
{
	D3DXMATRIX	rotationMatrix;
	D3DXVECTOR3 position;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;

	float pitch, yaw, roll;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	position.x =  this->m_posX;
	position.y = -this->m_posY + (height * 2.0f);
	position.z =  this->m_posZ;

	target.x = 0.0f;
	target.y = 0.0f;
	target.z = 1.0f;

	pitch = -this->m_rotX * 0.0174532925f;
	yaw   =  this->m_rotY * 0.0174532925f;
	roll  =  this->m_rotZ * 0.0174532925f;

	::D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	::D3DXVec3TransformCoord(&target, &target, &rotationMatrix);
	::D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	target += position;

	::D3DXMatrixLookAtLH(&this->m_reflectionMatrix, &position, &target, &up);

	return;
}

D3DXMATRIX CGECamera::GetReflectionViewMatrix()
{
	return this->m_reflectionMatrix;
}

void CGECamera::GetViewMatrix(D3DXMATRIX& _out)
{
	_out = this->m_viewMatrix;
}