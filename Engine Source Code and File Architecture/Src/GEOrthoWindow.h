/*
 * File name - GEOrthoWindow
 * Author - CuiXue
 * Updated date - 2013.7.19
 * Desc - 
 *
 */
#ifndef _GE_ORTHO_WINDOW_H_
#define _GE_ORTHO_WINDOW_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>

#include "Utility.h"
#endif

/*
 * File name - CGEOrthoWindow
 * Author - CuiXue
 * Updated date - 2013.7.19
 * Desc - 
 *
 */
class CGEOrthoWindow
{
private:
	// Private structure
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texCoord;
	};

public:
	// Constructors
	CGEOrthoWindow();
	CGEOrthoWindow(const CGEOrthoWindow&);
	CGEOrthoWindow& operator=(const CGEOrthoWindow&);
	~CGEOrthoWindow();

public:
	// Interfaces
	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool _initialize_buffers(ID3D11Device*, int, int);
	void _shutdown();
	void _render_buffers(ID3D11DeviceContext*);

private:
	// Members
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_vertexCount;
	int m_indexCount;
};

#endif