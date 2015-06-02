/*
 * File name - GEInput.h
 * Author - CuiXue
 * Updated date - 2013.1.11
 * Desc - 
 *
 */
#ifndef _GE_INPUT_H_
#define _GE_INPUT_H_

#ifdef _WIN32
#include "Utility.h"
#endif

class CGEInput
{
public:

	// Constructors & destructor
	CGEInput();
	CGEInput(const CGEInput&);
	CGEInput& operator=(const CGEInput&);
	~CGEInput();

	// Working Interfaces
	bool Initialize();
	void Shutdown();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int); 
	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];

	float m_xmouse;
	float m_ymouse;
	float m_wheelmouse;
};

#endif