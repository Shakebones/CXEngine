/*
 * File name - GELight.cpp
 * Author - CuiXue
 * Updated date - 2012.12.26
 * Desc -
 *
 */
#include "GELight.h"

CGELight::CGELight()
{
	this->m_diffuseColor = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	this->m_lightDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CGELight::CGELight(const CGELight& source)
{
}

CGELight& CGELight::operator=(const CGELight& source)
{
	return *this;
}

CGELight::~CGELight()
{
}

void CGELight::SetAmbientColor(float r, float g, float b, float alpha)
{
	this->m_ambientColor = D3DXVECTOR4(r, g, b, alpha);
}

void CGELight::SetDiffuseColor(float r, float g, float b, float alpha)
{
	this->m_diffuseColor = D3DXVECTOR4(r, g, b, alpha);
	return;
}

void CGELight::SetLightDirection(float x, float y, float z)
{
	this->m_lightDir = D3DXVECTOR3(x, y, z);
	return;
}

void CGELight::SetLightPosition(float x, float y, float z)
{
	this->m_lightPosition = D3DXVECTOR4(x, y, z, 1.0f);
	return;
}

D3DXVECTOR4 CGELight::GetAmbientColor()
{
	return this->m_ambientColor;
}

D3DXVECTOR4 CGELight::GetDiffuseColor()
{
	return this->m_diffuseColor;
}

D3DXVECTOR3 CGELight::GetLightDirection()
{
	return this->m_lightDir;
}

void CGELight::SetSpecularColor(float r, float g, float b, float alpha)
{
	this->m_specularColor = D3DXVECTOR4(r, g, b, alpha);
}

void CGELight::SetSpecularPower(float specularPower)
{
	this->m_specularPower = specularPower;
}

D3DXVECTOR4 CGELight::GetSpecularColor()
{
	return this->m_specularColor;
}

float CGELight::GetSpecularPower()
{
	return this->m_specularPower;
}

D3DXVECTOR4 CGELight::GetLightPosition()
{
	return this->m_lightPosition;
}