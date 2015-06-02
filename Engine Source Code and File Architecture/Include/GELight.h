/*
 * File name - GELight.h
 * Author - CuiXue
 * Updated date - 2012.12.26
 * Desc -
 *
 */
#ifndef _GE_LIGHT_H_
#define _GE_LIGHT_H_

#ifdef _WIN32
#include "Utility.h"
#include <D3DX10math.h>
#endif

/*
 * Class name - CGELight
 * Author - CuiXue
 * Updated date - 2012.12.26
 * Desc -
 *
 */
class CGELight
{
public:
	CGELight();
	CGELight(const CGELight&);
	CGELight& operator=(const CGELight&);
	~CGELight();

public:
	// Interfaces
	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetLightDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);

	// Point light & spot light.
	void SetLightPosition(float, float, float);

	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetLightPosition();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetLightDirection();
	D3DXVECTOR4 GetSpecularColor();
	float		GetSpecularPower();

private:

	D3DXVECTOR3 m_lightDir;
	D3DXVECTOR4 m_lightPosition;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_specularColor;
	float		m_specularPower;
};

#endif