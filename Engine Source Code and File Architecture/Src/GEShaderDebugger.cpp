/*
 * File name - GEShaderDebugger.cpp
 * Author - CuiXue
 * Updated date - 2013.3.18
 * Desc - 
 *
 */
#include "GEShaderDebugger.h"

CGEShaderDebugger::CGEShaderDebugger()
{
	this->m_pVertexBuf = 0;
	this->m_pIndexBuf = 0;
	this->m_vertexCount = 0;
	this->m_indexCount = 0;
	this->m_screenWidth = 0;
	this->m_screenHeight = 0;
	this->m_bitmapWidth = 0;
	this->m_bitmapHeight = 0;
	this->m_previousPosX = 0;
	this->m_previousPosY = 0;
}

CGEShaderDebugger::CGEShaderDebugger(const CGEShaderDebugger& source)
{

}

CGEShaderDebugger& CGEShaderDebugger::operator=(const CGEShaderDebugger& source)
{
	return *this;
}

CGEShaderDebugger::~CGEShaderDebugger()
{
}

bool CGEShaderDebugger::Initialize(ID3D11Device* pDevice, int screenWidth, int screenHeight,
								   int bitmapWidth, int bitmapHeight)
{
	bool result;

	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;
	this->m_bitmapWidth = bitmapWidth;
	this->m_bitmapHeight = bitmapHeight;

	this->m_previousPosX = -1;
	this->m_previousPosY = -1;

	result = this->_initialize_buffers(pDevice);
	if( !result )
		return false;

	return true;
}

int CGEShaderDebugger::GetIndexCount()
{
	return this->m_indexCount;
}

void CGEShaderDebugger::Shutdown()
{
	this->_shutdown();
}

bool CGEShaderDebugger::Render(ID3D11DeviceContext* pDeviceContext, int posX, int posY)
{
	bool result;

	result = this->_updated_buffers(pDeviceContext, posX, posY);
	if( !result )
		return false;

	this->_render_buffers(pDeviceContext);

	return true;
}

bool CGEShaderDebugger::_initialize_buffers(ID3D11Device* pDevice)
{
	GEVertex3D_Tex* pVertices;
	unsigned long* pIndices;
	D3D11_BUFFER_DESC vbDesc;
	D3D11_BUFFER_DESC ibDesc;
	D3D11_SUBRESOURCE_DATA vbData;
	D3D11_SUBRESOURCE_DATA ibData;
	HRESULT result;

	this->m_vertexCount = 6;
	this->m_indexCount = this->m_vertexCount;

	pVertices = new GEVertex3D_Tex[this->m_vertexCount];
	if( !pVertices )
		return false;

	pIndices = new unsigned long[this->m_indexCount];
	if( !pIndices )
		return false;

	memset(pVertices, 0, (sizeof(GEVertex3D_Tex) * this->m_vertexCount));

	for(int i = 0; i != this->m_indexCount; i++)
		pIndices[i] = i;

	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(GEVertex3D_Tex) * this->m_vertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	vbData.pSysMem = pVertices;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&vbDesc, &vbData, &this->m_pVertexBuf);
	if( FAILED(result) )
		return false;

	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * this->m_indexCount;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	ibData.pSysMem = pIndices;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&ibDesc, &ibData, &this->m_pIndexBuf);
	if( FAILED(result) )
		return false;

	::DeleteArray<GEVertex3D_Tex*>(pVertices);
	::DeleteArray<unsigned long*>(pIndices);

	return true;
}

void CGEShaderDebugger::_shutdown()
{
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pVertexBuf);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pIndexBuf);
}

bool CGEShaderDebugger::_updated_buffers(ID3D11DeviceContext* pDeviceContext, int positionX, int positionY)
{
	HRESULT result;

	float left, right, top, bottom;
	GEVertex3D_Tex* pVertices;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	GEVertex3D_Tex* verticesPtr;

	if( (positionX == this->m_previousPosX) && (positionY == this->m_previousPosY) )
		return true;

	this->m_previousPosX = positionX;
	this->m_previousPosY = positionY;

	left = (float)((this->m_screenWidth / 2) * (-1)) + (float)positionX;
	right = left + (float)this->m_bitmapWidth;
	top = (float)(this->m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)this->m_bitmapHeight;

	pVertices = new GEVertex3D_Tex[this->m_vertexCount];
	if( !pVertices )
		return false;

	pVertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	pVertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	pVertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	pVertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	pVertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	pVertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	pVertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	pVertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	pVertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	pVertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	pVertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	pVertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	result = pDeviceContext->Map(this->m_pVertexBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	verticesPtr = (GEVertex3D_Tex*)mappedResources.pData;
	
	memcpy(verticesPtr, (void*)pVertices, (sizeof(GEVertex3D_Tex) * this->m_vertexCount));

	pDeviceContext->Unmap(this->m_pVertexBuf, 0);

	::DeleteArray<GEVertex3D_Tex*>(pVertices);

	return true;
}

bool CGEShaderDebugger::_render_buffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(GEVertex3D_Tex);
	offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertexBuf, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(this->m_pIndexBuf, DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}