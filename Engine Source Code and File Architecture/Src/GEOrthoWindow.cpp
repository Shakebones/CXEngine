/*
 * File name - GEOrthoWindow.cpp
 * Author - CuiXue
 * Updated date - 2013.7.19
 * Desc - 
 *
 */
#include "GEOrthoWindow.h"

CGEOrthoWindow::CGEOrthoWindow()
{
	this->m_pVertexBuffer = 0;
	this->m_pIndexBuffer = 0;
	this->m_vertexCount = 0;
	this->m_indexCount = 0;
}

CGEOrthoWindow::CGEOrthoWindow(const CGEOrthoWindow& source)
{
}

CGEOrthoWindow& CGEOrthoWindow::operator=(const CGEOrthoWindow& source)
{
	return *this;
}

CGEOrthoWindow::~CGEOrthoWindow()
{
}

bool CGEOrthoWindow::Initialize(ID3D11Device* pDevice, int width, int height)
{
	bool result;

	result = this->_initialize_buffers(pDevice, width, height);
	if( !result )
		return false;

	return true;
}

void CGEOrthoWindow::Shutdown()
{
	this->_shutdown();
}

void CGEOrthoWindow::Render(ID3D11DeviceContext* pDeviceContext)
{
	this->_render_buffers(pDeviceContext);
	return;
}

int CGEOrthoWindow::GetIndexCount()
{
	return this->m_indexCount;
}

bool CGEOrthoWindow::_initialize_buffers(ID3D11Device* pDevice, int width, int height)
{
	float left, right, top, bottom;
	CGEOrthoWindow::VertexType* pVertices;
	unsigned long* pIndices;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	left = (float)(width / 2) * (-1);
	right = left + (float)width;
	top = (float)(height / 2);
	bottom = top - (float)height;

	this->m_vertexCount = 6;
	this->m_indexCount = this->m_vertexCount;

	pVertices = new CGEOrthoWindow::VertexType[this->m_vertexCount];
	if( !pVertices )
		return false;

	pIndices = new unsigned long[this->m_indexCount];
	if( !pIndices )
		return false;

	pVertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	pVertices[0].texCoord = D3DXVECTOR2(0.0f, 0.0f);

	pVertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	pVertices[1].texCoord = D3DXVECTOR2(1.0f, 1.0f);

	pVertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	pVertices[2].texCoord = D3DXVECTOR2(0.0f, 1.0f);

	pVertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	pVertices[3].texCoord = D3DXVECTOR2(0.0f, 0.0f);

	pVertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	pVertices[4].texCoord = D3DXVECTOR2(1.0f, 0.0f);
	
	pVertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	pVertices[5].texCoord = D3DXVECTOR2(1.0f, 1.0f);

	for(int i = 0; i != this->m_indexCount; i++)
	{
		pIndices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(CGEOrthoWindow::VertexType) * this->m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

	::DeleteArray<CGEOrthoWindow::VertexType*>(pVertices);
	::DeleteArray<unsigned long*>(pIndices);

	return true;
}

void CGEOrthoWindow::_shutdown()
{
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pVertexBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pIndexBuffer);
}

void CGEOrthoWindow::_render_buffers(ID3D11DeviceContext* pDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(CGEOrthoWindow::VertexType);
	offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &this->m_pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(this->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return;
}