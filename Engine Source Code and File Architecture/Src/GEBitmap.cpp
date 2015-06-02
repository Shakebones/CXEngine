/*
 * File name - GEBitmap.cpp
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc - 
 *
 */
#include "GEBitmap.h"

CGEBitmap::CGEBitmap()
{
	this->m_pVertexBuffer = 0;
	this->m_pIndexBuffer= 0;
	this->m_pTexture = 0;
}

CGEBitmap::CGEBitmap(const CGEBitmap& source)
{
}

CGEBitmap& CGEBitmap::operator=(const CGEBitmap& source)
{
	return *this;
}

CGEBitmap::~CGEBitmap()
{
}

bool CGEBitmap::Initialize(ID3D11Device* pDevice, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;
	this->m_bitmapWidth = bitmapWidth;
	this->m_bitmapHeight = bitmapHeight;
	this->m_prePosx = this->m_prePosy = -1;

	result = this->_initialize_buffers(pDevice);
	if( !result )
		return false;

	result = this->_load_texture(pDevice, textureFilename);
	if( !result )
		return false;

	return true;
}

void CGEBitmap::Shutdown()
{
	this->_release_texture();
	this->_shutdown();
}

bool CGEBitmap::Render(ID3D11DeviceContext* pDeviceContext, int posx, int posy)
{
	bool result;

	result = this->_update_buffers(pDeviceContext, posx, posy);
	if( !result )
		return false;

	this->_render_buffers(pDeviceContext);

	return true;
}

int CGEBitmap::GetIndexCount()
{
	return this->m_indexCount;
}

ID3D11ShaderResourceView* CGEBitmap::GetTexture()
{
	return this->m_pTexture->GetTexture();
}

bool CGEBitmap::_initialize_buffers(ID3D11Device* pDevice)
{
	GEVertex3D_Tex* pVertices;
	unsigned long* pIndices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;

	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	this->m_vertexCount = 6;
	this->m_indexCount = this->m_vertexCount;

	pVertices = new GEVertex3D_Tex[this->m_vertexCount];
	if( !pVertices )
		return false;

	pIndices = new unsigned long[this->m_indexCount];
	if( !pIndices )
		return false;

	::memset(pVertices, 0, (sizeof(GEVertex3D_Tex) * this->m_vertexCount));

	for(int i = 0; i < this->m_indexCount; i++)
		pIndices[i] = i;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Tex) * this->m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_pVertexBuffer);
	if( FAILED(result) )
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &this->m_pIndexBuffer);
	if( FAILED(result) )
		return false;

	::DeleteArray<GEVertex3D_Tex*>(pVertices);
	::DeleteArray<unsigned long*>(pIndices);

	return true;
}

void CGEBitmap::_shutdown()
{
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pIndexBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pVertexBuffer);
}

bool CGEBitmap::_update_buffers(ID3D11DeviceContext* pDeviceContext, int posx, int posy)
{
	float left;
	float right;
	float top;
	float bottom;

	GEVertex3D_Tex *pVertices;
	GEVertex3D_Tex *pVertices2;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result;

	if( (posx == this->m_prePosx) && (posy == this->m_prePosy) )
		return true;

	this->m_prePosx = posx;
	this->m_prePosy = posy;

	left	= (float)((this->m_screenWidth / 2) * (-1)) + (float)posx;
	right	= left + (float)this->m_bitmapWidth;
	top		= (float)(this->m_screenHeight / 2) - (float)posy;
	bottom	= top - (float)this->m_bitmapHeight;

	pVertices = new GEVertex3D_Tex[this->m_vertexCount];
	if( !pVertices )
		return false;

	pVertices[0] = GEVertex3D_Tex(D3DXVECTOR3(left, top, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[1] = GEVertex3D_Tex(D3DXVECTOR3(right, bottom, 0.0f), D3DXVECTOR2(1.0f, 1.0f));
	pVertices[2] = GEVertex3D_Tex(D3DXVECTOR3(left, bottom, 0.0f), D3DXVECTOR2(0.0f, 1.0f));
	pVertices[3] = GEVertex3D_Tex(D3DXVECTOR3(left, top, 0.0f), D3DXVECTOR2(0.0f, 0.0f));
	pVertices[4] = GEVertex3D_Tex(D3DXVECTOR3(right, top, 0.0f), D3DXVECTOR2(1.0f, 0.0f));
	pVertices[5] = GEVertex3D_Tex(D3DXVECTOR3(right, bottom, 0.0f), D3DXVECTOR2(1.0f, 1.0f));

	result = pDeviceContext->Map(this->m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) )
		return false;

	pVertices2 = (GEVertex3D_Tex*)mappedResource.pData;
	::memcpy(pVertices2, (void*)pVertices, (sizeof(GEVertex3D_Tex) * this->m_vertexCount));
	pDeviceContext->Unmap(this->m_pVertexBuffer, 0);

	::DeleteArray<GEVertex3D_Tex*>(pVertices);

	return true;
}

void CGEBitmap::_render_buffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(GEVertex3D_Tex);
	offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(this->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool CGEBitmap::_load_texture(ID3D11Device* pDevice, WCHAR* filename)
{
	bool result;
	
	this->m_pTexture = new CGETexture;
	if( !this->m_pTexture )
		return false;

	result = this->m_pTexture->Initialize(pDevice, filename);
	if( !result )
		return false;

	return true;
}

void CGEBitmap::_release_texture()
{
	::Shutdown<CGETexture*>(this->m_pTexture);
}