/*
 * File name - GEBitmap.h
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc -
 *
 */
#ifndef _GE_BITMAP_H_
#define _GE_BITMAP_H_

#ifdef _WIN32
#include <D3D11.h>
#include <D3DX10math.h>
#include "Utility.h"
#include "GETexture.h"
#endif


/*
 * Class name - CGEBitmap
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc - 
 *
 */
class CGEBitmap
{
public:
	// Interfaces
	CGEBitmap();
	CGEBitmap(const CGEBitmap&);
	CGEBitmap& operator=(const CGEBitmap&);
	~CGEBitmap();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	// Kernels
	bool _initialize_buffers(ID3D11Device*);
	void _shutdown();
	bool _update_buffers(ID3D11DeviceContext*, int, int);
	void _render_buffers(ID3D11DeviceContext*);

	bool _load_texture(ID3D11Device*, WCHAR*);
	void _release_texture();

private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	CGETexture*	m_pTexture;
	int m_vertexCount;
	int m_indexCount;
	int m_screenWidth;
	int m_screenHeight;
	int m_bitmapWidth;
	int m_bitmapHeight;
	int m_prePosx;
	int m_prePosy;
};
#endif