/*
 * File name - GEText.cpp
 * Author - CuiXue
 * Updated date - 2013.1.4
 * Desc -
 *
 */
#include "GEText.h"

CGEText::CGEText()
{
	this->m_pFont = 0;
	this->m_pFontShader = 0;
}

CGEText::CGEText(const CGEText& source)
{
}

CGEText& CGEText::operator=(const CGEText& source)
{
	return *this;
}

CGEText::~CGEText()
{
}

bool CGEText::Initialize(ID3D11Device* pDevice, 
						 ID3D11DeviceContext* pDeviceContext,
						 HWND hWnd,
						 int screenWidth,
						 int screenHeight,
						 D3DXMATRIX baseView)
{
	bool result;

	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;
	this->m_baseViewMatrix = baseView;

	this->m_pFont = new CGEFont;
	if( !this->m_pFont )
		return false;

	result = this->m_pFont->Initialize(pDevice, "..\\Res\\Font\\fontdata.txt", L"..\\Res\\Tex\\font.dds");
	if( !result )
	{
		::MessageBox(hWnd, L"Could not initialize font object", L"Error", MB_OK);
		return false;
	}

	this->m_pFontShader = new CGEFontShader;
	if( !this->m_pFontShader )
		return false;

	result = this->m_pFontShader->Initialize(pDevice, hWnd);
	if( !result )
	{
		MessageBox(hWnd, L"Could not initialize font shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void CGEText::Shutdown()
{
	int len = this->m_myvec_sentences.GetMyVectorLength();
	for(int i = 0; i < len; i++)
	{
		this->_release_sentence(&this->m_myvec_sentences[i]);
	}

	::Shutdown<CGEFontShader*>(this->m_pFontShader);
	::Shutdown<CGEFont*>(this->m_pFont);
}

bool CGEText::Render(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX world, D3DXMATRIX ortho)
{
	bool result;

	for(int i = 0; i < this->m_myvec_sentences.GetMyVectorLength(); i++)
	{
		result = this->_render_sentence(pDeviceContext, this->m_myvec_sentences[i], world, ortho);
		if( !result )
			return false;
	}

	return true;
}

bool CGEText::_initialize_sentence(SentenceType** ppSentence, int maxLength, ID3D11Device* pDevice)
{
	GEVertex3D_Tex* pVertices;
	unsigned int* pIndices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	HRESULT result;

	*ppSentence = new SentenceType;
	if( !*ppSentence )
		return false;

	(*ppSentence)->pVB = 0;
	(*ppSentence)->pIB = 0;
	(*ppSentence)->maxLength = maxLength;
	(*ppSentence)->vertexCount = 6 * maxLength;
	(*ppSentence)->indexCount = (*ppSentence)->vertexCount;

	pVertices = new GEVertex3D_Tex[(*ppSentence)->vertexCount];
	if( !pVertices )
		return false;

	pIndices = new unsigned int[(*ppSentence)->indexCount];
	if( !pIndices )
		return false;

	// Initialize vertex/index buffer
	::memset(pVertices, 0, (sizeof(GEVertex3D_Tex) * (*ppSentence)->vertexCount));

	for(int i = 0; i < (*ppSentence)->indexCount; i++)
		pIndices[i] = i;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(GEVertex3D_Tex) * (*ppSentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &(*ppSentence)->pVB);
	if( FAILED(result) )
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * (*ppSentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &(*ppSentence)->pIB);
	if( FAILED(result) )
		return false;

	::DeleteArray<GEVertex3D_Tex*>(pVertices);
	::DeleteArray<unsigned int*>(pIndices);

	return true;
}

bool CGEText::_update_sentence(SentenceType* pSentence, 
							   char* pText, 
							   int posx, 
							   int posy,
							   int guid,
							   float r, 
							   float g, 
							   float b, 
							   ID3D11DeviceContext* pDeviceContext)
{
	HRESULT result;
	GEVertex3D_Tex* pVertices;
	GEVertex3D_Tex* tmp_pVertices;
	float draw_posx;
	float draw_posy;
	int numLetters;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	pSentence->r = r;
	pSentence->g = g;
	pSentence->b = b;
	pSentence->guid = guid;

	numLetters =  (int)strlen(pText);
	if( numLetters > pSentence->maxLength )
		return false;

	pVertices = new GEVertex3D_Tex[pSentence->vertexCount];
	if( !pVertices )
		return false;

	// placeholder
	memset(pVertices, 0, sizeof(GEVertex3D_Tex) * pSentence->vertexCount);

	draw_posx = (float)(((this->m_screenWidth / 2) * (-1)) + posx);
	draw_posy = (float)((this->m_screenHeight / 2) - posy);

	this->m_pFont->BuildVertexArray((void*)pVertices, pText, draw_posx, draw_posy);

	result = pDeviceContext->Map(pSentence->pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) )
		return false;

	tmp_pVertices = (GEVertex3D_Tex*)mappedResource.pData;

	memcpy(tmp_pVertices, (void*)pVertices, (sizeof(GEVertex3D_Tex) * pSentence->vertexCount));

	pDeviceContext->Unmap(pSentence->pVB, 0);

	::DeleteArray<GEVertex3D_Tex*>(pVertices);

	return true;
}

void CGEText::_release_sentence(SentenceType** ppSentence)
{
	if( *ppSentence )
	{
		::ReleaseWithoutDel<ID3D11Buffer*>((*ppSentence)->pVB);
		::ReleaseWithoutDel<ID3D11Buffer*>((*ppSentence)->pIB);

		delete *ppSentence;
		*ppSentence = NULL;
	}

	return;
}

bool CGEText::_render_sentence(ID3D11DeviceContext* pDeviceContext, SentenceType* pSentence, 
							   D3DXMATRIX world,
							   D3DXMATRIX ortho)
{
	bool result;
	unsigned int stride;
	unsigned int offset;
	D3DXVECTOR4 pixelColor;
	
	stride = sizeof(GEVertex3D_Tex);
	offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &pSentence->pVB, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(pSentence->pIB, DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pixelColor = D3DXVECTOR4(pSentence->r, pSentence->g, pSentence->b, 1.0f);

	result = this->m_pFontShader->Render(pDeviceContext, pSentence->indexCount,
		world, this->m_baseViewMatrix, ortho, this->m_pFont->GetTexture(), pixelColor);
	if( !result )
		return false;

	return true;
}

bool CGEText::Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, int GUID, char* pText, int posx, int posy, int max_length, D3DXVECTOR4 color)
{
	bool result;
	color.w = 1.0f;
	SentenceType* pNewSentence = NULL;

	int i = 0;
	while( i < this->m_myvec_sentences.GetMyVectorLength() )
	{
		if( this->m_myvec_sentences[i]->guid == GUID )
		{
			result = this->_update_sentence(this->m_myvec_sentences[i], pText, posx, posy, GUID, color.x, color.y, color.z, pDeviceContext);
			if( !result )
				return false;
			
			return true;
		}
		i++;
	}

	if( i == this->m_myvec_sentences.GetMyVectorLength() )
	{
		pNewSentence = new SentenceType;
		if( !pNewSentence )
			return false;

		result = this->_initialize_sentence(&pNewSentence, max_length, pDevice);
		if( !result )
			return false;

		result = this->_update_sentence(pNewSentence, pText, posx, posy, GUID, color.x, color.y, color.z, pDeviceContext);
		if( !result )
			return false;

		// Add new sentenc to vector container of CGEText.
		this->m_myvec_sentences.Pushback(pNewSentence);

		this->m_num_of_sentences = this->m_myvec_sentences.GetMyVectorLength();

		pNewSentence = NULL;
	}

	return true;
}

