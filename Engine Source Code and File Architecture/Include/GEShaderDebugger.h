/*
 * File name - GEShaderDebugger.h
 * Author - CuiXue
 * Updated date - 2013.3.18
 * Desc - 
 *
 */
#ifndef _GE_SHADER_DEBUGGER_H_
#define _GE_SHADER_DEBUGGER_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

/*
 * Class name - CGEShaderDebugger
 * Author - CuiXue
 * Updated date - 2013.3.18
 * Desc - 
 *
 */
class CGEShaderDebugger
{
public:
	CGEShaderDebugger();
	CGEShaderDebugger(const CGEShaderDebugger&);
	CGEShaderDebugger& operator=(const CGEShaderDebugger&);
	~CGEShaderDebugger();

public:
	bool Initialize(ID3D11Device*, int, int, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();

private:
	bool _initialize_buffers(ID3D11Device*);
	void _shutdown();
	bool _updated_buffers(ID3D11DeviceContext*, int, int);
	bool _render_buffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_pVertexBuf;
	ID3D11Buffer* m_pIndexBuf;

	int m_vertexCount;
	int m_indexCount;
	int m_screenWidth;
	int m_screenHeight;
	int m_bitmapWidth;
	int m_bitmapHeight;
	int m_previousPosX;
	int m_previousPosY;
};

#endif