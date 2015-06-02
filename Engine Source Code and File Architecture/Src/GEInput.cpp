/*
 * File name - GEInput.cpp
 * Author - CuiXue
 * Updated date - 2012.11.27
 * Desc -
 *
 * Definition
 */
#include "GEInput.h"

CGEInput::CGEInput()
{
}

CGEInput::CGEInput(const CGEInput& source)
{
}

CGEInput& CGEInput::operator=(const CGEInput& source)
{
	return *this;
}

CGEInput::~CGEInput()
{
}

bool CGEInput::Initialize()
{
	for(int i = 0; i < 256; i++)
		this->m_keys[i] = false;

	return true;
}

void CGEInput::Shutdown()
{
}

void CGEInput::KeyDown(unsigned int key)
{
	this->m_keys[key] = true;
}

void CGEInput::KeyUp(unsigned int key)
{
	this->m_keys[key] = false;
}

bool CGEInput::IsKeyDown(unsigned int key)
{
	return this->m_keys[key];
}