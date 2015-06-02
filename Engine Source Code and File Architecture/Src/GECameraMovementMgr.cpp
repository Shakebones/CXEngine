/*
 * File name - GECameraMovementMgr.cpp
 * Author - CuiXue
 * Updated date - 2013.2.26
 * Desc -
 *
 */
#include "GECameraMovementMgr.h"

CGECameraMovementMgr::CGECameraMovementMgr()
{
	this->m_frameTime = 0.0f;
	this->m_leftRotationSpd = 0.0f;
	this->m_rightRotationSpd = 0.0f;
	this->m_lookDownSpd = 0.0f;
	this->m_lookUpSpd = 0.0f;
	this->m_rotY = 0.0f;
	this->m_rotX = 0.0f;
	this->m_rotZ = 0.0f;
	this->m_moveBackSpd = 0.0f;
	this->m_moveForwardSpd = 0.0f;
	this->m_upSpd = 0.0f;
	this->m_downSpd = 0.0f;
	this->m_moveForwardBackPos = 0.0f;
	this->m_moveLeftRightPos = 0.0f;
	this->m_moveUpDownPos = 0.0f;
	this->m_moveLeftSpd = 0.0f;
	this->m_moveRightSpd = 0.0f;
	this->m_moveUpSpd = 0.0f;
	this->m_moveDownSpd = 0.0f;
}

CGECameraMovementMgr::CGECameraMovementMgr(const CGECameraMovementMgr& source)
{

}

CGECameraMovementMgr& CGECameraMovementMgr::operator=(const CGECameraMovementMgr& source)
{
	return *this;
}

CGECameraMovementMgr::~CGECameraMovementMgr()
{

}

void CGECameraMovementMgr::SetFrameTime(float time)
{
	this->m_frameTime = time;
	return;
}

float CGECameraMovementMgr::GetRotationY()
{
	return this->m_rotY;
}

float CGECameraMovementMgr::GetRotationX()
{
	return this->m_rotX;
}

void CGECameraMovementMgr::GetRotationX(float& _out)
{
	_out = this->m_rotX;
}

void CGECameraMovementMgr::GetRotationY(float& _out)
{
	_out = this->m_rotY;
	return;
}

void CGECameraMovementMgr::GetMoveForwardBackPos(float& _out)
{
	_out = this->m_moveForwardBackPos;
}

float CGECameraMovementMgr::GetMoveForwardBackPos()
{
	return this->m_moveForwardBackPos;
}

void CGECameraMovementMgr::LeftRotationY(bool isKeyDown)
{
	if( isKeyDown )
	{
		this->m_leftRotationSpd += this->m_frameTime * 0.01f;
		if( this->m_leftRotationSpd > (this->m_frameTime * 0.15f) )
			this->m_leftRotationSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_leftRotationSpd -= this->m_frameTime * 0.005f;
		if( this->m_leftRotationSpd < 0.0f )
			this->m_leftRotationSpd = 0.0f;
	}

	this->m_rotY -= this->m_leftRotationSpd;
	if( this->m_rotY < 0.0f )
		this->m_rotY += 360.0f;
}

void CGECameraMovementMgr::RightRotationY(bool isKeyDown)
{
	if( isKeyDown )
	{
		this->m_rightRotationSpd += this->m_frameTime * 0.01f;
		if( this->m_rightRotationSpd > (this->m_frameTime * 0.15f) )
			this->m_rightRotationSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_rightRotationSpd -= this->m_frameTime * 0.005f;
		if( this->m_rightRotationSpd < 0.0f )
			this->m_rightRotationSpd = 0.0f;
	}

	this->m_rotY += this->m_rightRotationSpd;
	if( this->m_rotY > 360.0f )
		this->m_rotY -= 360.0f;
}

void CGECameraMovementMgr::LookUp(bool isKeyDown)
{
	if( isKeyDown )
	{
		this->m_lookUpSpd += this->m_frameTime * 0.01f;
		if( this->m_lookUpSpd > (this->m_frameTime * 0.15f) )
			this->m_lookUpSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_lookUpSpd -= this->m_frameTime * 0.005f;
		if( this->m_lookUpSpd < 0.0f )
			this->m_lookUpSpd = 0.0f;
	}

	this->m_rotX -= this->m_lookUpSpd;
	if( this->m_rotX < 0.0f )
		this->m_rotX += 360.0f;
}

void CGECameraMovementMgr::LookDown(bool isKeyDown)
{
	if( isKeyDown )
	{
		this->m_lookDownSpd += this->m_frameTime * 0.01f;
		if( this->m_lookDownSpd > (this->m_frameTime * 0.15f) )
			this->m_lookDownSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_lookDownSpd -= this->m_frameTime * 0.005f;
		if( this->m_lookDownSpd < 0.0f )
			this->m_lookDownSpd = 0.0f;
	}

	this->m_rotX += this->m_lookDownSpd;
	if( this->m_rotX > 360.0f )
		this->m_rotX -= 360.0f;
}

void CGECameraMovementMgr::MoveForward(bool isKeyDown)
{
	float radians = 0.0f;
	float radiansX = 0.0f;

	if( isKeyDown )
	{
		this->m_moveForwardSpd += this->m_frameTime * 0.0001f;
		if( this->m_moveForwardSpd > (this->m_frameTime * 0.1f) )
			this->m_moveForwardSpd = this->m_frameTime * 0.1f;
	}

	else
	{
		this->m_moveForwardSpd -= this->m_frameTime * 0.00005f;
		if( this->m_moveForwardSpd < 0.0f )
			this->m_moveForwardSpd = 0.0f;
	}

	radians = this->m_rotY * 0.0174532925f;
	radiansX = this->m_rotX * 0.0174532925f;

	this->m_moveLeftRightPos += sin(radians) * this->m_moveForwardSpd;
	this->m_moveForwardBackPos += cos(radians) * this->m_moveForwardSpd;
}

void CGECameraMovementMgr::MoveBack(bool isKeyDown)
{
	float radiansY = 0.0f;
	float radiansX = 0.0f;

	if( isKeyDown )
	{
		this->m_moveBackSpd += this->m_frameTime * 0.0001f;
		if( this->m_moveBackSpd > (this->m_frameTime * 0.1f) )
			this->m_moveBackSpd = this->m_frameTime * 0.1f;
	}

	else
	{
		this->m_moveBackSpd -= this->m_frameTime * 0.00005f;
		if( this->m_moveBackSpd < 0.0f )
			this->m_moveBackSpd = 0.0f;
	}

	radiansY = this->m_rotY * 0.0174532925f;
	radiansX = this->m_rotX * 0.0174532925f;

	this->m_moveLeftRightPos -= sin(radiansY) * this->m_moveBackSpd;
	this->m_moveForwardBackPos -= cos(radiansY) * this->m_moveBackSpd;
}

void CGECameraMovementMgr::SetStartPoint(float x, float y, float z)
{
	this->m_moveLeftRightPos = x;
	this->m_moveUpDownPos = y;
	this->m_moveForwardBackPos = z;
}

void CGECameraMovementMgr::MoveLeft(bool isKeyDown)
{
	float radians = 0.0f;

	if( isKeyDown )
	{
		this->m_moveLeftSpd += this->m_frameTime * 0.0001f;
		if( this->m_moveLeftSpd > (this->m_frameTime * 0.15f) )
			this->m_moveLeftSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_moveLeftSpd -= this->m_frameTime * 0.00005f;
		if( this->m_moveLeftSpd < 0.0f )
			this->m_moveLeftSpd = 0.0f;
	}

	radians = this->m_rotY * 0.0174532925f;

	this->m_moveLeftRightPos -= cos(radians) * this->m_moveLeftSpd;
	this->m_moveForwardBackPos += sin(radians) * this->m_moveLeftSpd;
}

void CGECameraMovementMgr::MoveRight(bool isKeyDown)
{
	float radians = 0.0f;

	if( isKeyDown )
	{
		this->m_moveRightSpd += this->m_frameTime * 0.0001f;
		if( this->m_moveRightSpd > (this->m_frameTime * 0.15f) )
			this->m_moveRightSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_moveRightSpd -= this->m_frameTime * 0.00005f;
		if( this->m_moveRightSpd < 0.0f )
			this->m_moveRightSpd = 0.0f;
	}

	radians = this->m_rotY * 0.0174532925f;

	this->m_moveLeftRightPos += cos(radians) * this->m_moveRightSpd;
	this->m_moveForwardBackPos -= sin(radians) * this->m_moveRightSpd;
}

void CGECameraMovementMgr::Up(bool isKeyDown)
{
	if( isKeyDown )
	{
		this->m_moveUpSpd += this->m_frameTime * 0.0001f;
		if( this->m_moveUpSpd > (this->m_frameTime * 0.15f) )
			this->m_moveUpSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_moveUpSpd -= this->m_frameTime * 0.00005f;
		if( this->m_moveUpSpd < 0.0f )
			this->m_moveUpSpd = 0.0f;
	}

	this->m_moveUpDownPos += this->m_moveUpSpd;
}

void CGECameraMovementMgr::Down(bool isKeyDown)
{
	if( isKeyDown )
	{
		this->m_moveDownSpd += this->m_frameTime * 0.0001f;
		if( this->m_moveDownSpd > (this->m_frameTime * 0.15f) )
			this->m_moveDownSpd = this->m_frameTime * 0.15f;
	}

	else
	{
		this->m_moveDownSpd -= this->m_frameTime * 0.00005f;
		if( this->m_moveDownSpd < 0.0f )
			this->m_moveDownSpd = 0.0f;
	}

	this->m_moveUpDownPos -= this->m_moveDownSpd;
}

void CGECameraMovementMgr::GetLeftRightPos(float& _out)
{
	_out = this->m_moveLeftRightPos;
}

float CGECameraMovementMgr::GetLeftRightPos()
{
	return this->m_moveLeftRightPos;
}

void CGECameraMovementMgr::GetUpDownPos(float& _out)
{
	_out = this->m_moveUpDownPos;
}

float CGECameraMovementMgr::GetUpDownPos()
{
	return this->m_moveUpDownPos;
}