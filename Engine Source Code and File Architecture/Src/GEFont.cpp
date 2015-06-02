/*
 * File name - GEFont.cpp
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc
 *
 */
#include "GEFont.h"

CGEFont::CGEFont()
{
	this->m_pFont = 0;
	this->m_pTexture = 0;
}

CGEFont::CGEFont(const CGEFont& source)
{
}

CGEFont& CGEFont::operator=(const CGEFont& source)
{
	return *this;
}

CGEFont::~CGEFont()
{
}

bool CGEFont::Initialize(ID3D11Device* pDevice, char* fontFilename, WCHAR* textureFilename)
{
	bool result;

	result = this->_load_font(fontFilename);
	if( !result )
		return false;

	result = this->_load_texture(pDevice, textureFilename);
	if( !result )
		return false;

	return true;
}

void CGEFont::Shutdown()
{
	this->_release_texture();
	this->_release_font_data();
}

bool CGEFont::_load_font(char* filename)
{
	ifstream fin;
	char temp;

	this->m_pFont = new GEFontType[95];
	if( !this->m_pFont )
		return false;

	fin.open(filename);
	if( fin.fail() )
		return false;

	for(int i = 0; i < 95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
			fin.get(temp);

		fin.get(temp);
		while(temp != ' ')
			fin.get(temp);

		fin >> this->m_pFont[i].left;
		fin >> this->m_pFont[i].right;
		fin >> this->m_pFont[i].size;
	}

	fin.close();

	return true;
}

void CGEFont::_release_font_data()
{
	Delete<GEFontType*>(this->m_pFont);
}

bool CGEFont::_load_texture(ID3D11Device* pDevice, WCHAR* filename)
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

void CGEFont::_release_texture()
{
	::Shutdown<CGETexture*>(this->m_pTexture);
}

ID3D11ShaderResourceView* CGEFont::GetTexture()
{
	return this->m_pTexture->GetTexture();
}

void CGEFont::BuildVertexArray(void* pVertices, char* sentence, float drawX, float drawY)
{
	GEVertex3D_Tex* tmpVertices;
	int numLetters;
	int index;
	int letter;

	tmpVertices = (GEVertex3D_Tex*)pVertices;

	index = 0;
	numLetters = (int)strlen(sentence);

	for(int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		if( letter == 0 )
			drawX += 3.0f;

		else
		{
			tmpVertices[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			tmpVertices[index].texture = D3DXVECTOR2(this->m_pFont[letter].left, 0.0f);
			index++;

			tmpVertices[index].position = D3DXVECTOR3((drawX + this->m_pFont[letter].size), (drawY - 16), 0.0f);
			tmpVertices[index].texture = D3DXVECTOR2(this->m_pFont[letter].right, 1.0f);
			index++;

			tmpVertices[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);
			tmpVertices[index].texture = D3DXVECTOR2(this->m_pFont[letter].left, 1.0f);
			index++;

			tmpVertices[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			tmpVertices[index].texture = D3DXVECTOR2(this->m_pFont[letter].left, 0.0f);
			index++;

			tmpVertices[index].position = D3DXVECTOR3(drawX + this->m_pFont[letter].size, drawY, 0.0f);
			tmpVertices[index].texture = D3DXVECTOR2(this->m_pFont[letter].right, 0.0f);
			index++;

			tmpVertices[index].position = D3DXVECTOR3((drawX + this->m_pFont[letter].size), (drawY - 16), 0.0f);
			tmpVertices[index].texture = D3DXVECTOR2(this->m_pFont[letter].right, 1.0f);
			index++;

			drawX = drawX + this->m_pFont[letter].size + 1.0f;
		}
	}
}