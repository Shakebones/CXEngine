/*
 * File name - GECameraMovementMgr.h
 * Author - CuiXue
 * Updated date - 2013.2.26
 * Desc -
 *
 * 处理摄像机移动效果 （平滑移动、渲染）
 */
#ifndef _GE_CAMERA_MOVEMENT_MGR_H_
#define _GE_CAMERA_MOVEMENT_MGR_H_

#ifdef _WIN32
#include "Utility.h"
#include <math.h>
#endif

/*
 * Class name - CGECameraMovementMgr
 * Author - CuiXue
 * Updated date - 2013.2.26
 * Desc -
 *
 */
class CGECameraMovementMgr
{
public:
	// Interfaces
	CGECameraMovementMgr();
	CGECameraMovementMgr(const CGECameraMovementMgr&);
	CGECameraMovementMgr& operator=(const CGECameraMovementMgr&);
	~CGECameraMovementMgr();

public:
	void SetStartPoint(float, float, float);
	void SetFrameTime(float);
	float GetRotationY();
	void GetRotationY(float&);
	float GetRotationX();
	void GetRotationX(float&);
	float GetMoveForwardBackPos();
	void GetMoveForwardBackPos(float&);
	void GetUpDownPos(float&);
	float GetUpDownPos();
	void GetLeftRightPos(float&);
	float GetLeftRightPos();
	
	// All movement by keyboard
	void LeftRotationY(bool);
	void RightRotationY(bool);
	void LookUp(bool);
	void LookDown(bool);
	void MoveForward(bool);
	void MoveBack(bool);
	void Up(bool);
	void Down(bool);
	void MoveLeft(bool);
	void MoveRight(bool);

private:
	float m_frameTime;
	float m_rotY;
	float m_rotX;
	float m_rotZ;
	float m_moveForwardBackPos;
	float m_moveLeftRightPos;
	float m_moveUpDownPos;

	float m_leftRotationSpd;
	float m_rightRotationSpd;
	float m_lookUpSpd;
	float m_lookDownSpd;
	float m_upSpd;
	float m_downSpd;
	float m_moveForwardSpd;
	float m_moveBackSpd;
	float m_moveLeftSpd;
	float m_moveRightSpd;
	float m_moveUpSpd;
	float m_moveDownSpd;
};

#endif