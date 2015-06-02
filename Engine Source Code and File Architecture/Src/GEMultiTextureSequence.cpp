/*
 * File name - GETextureSequence.cpp
 * Author - CuiXue
 * Updated date - 2013.3.3
 * Desc - 
 *
 */
#include "GEMultiTextureSequence.h"

CGEMultiTextureSequence::CGEMultiTextureSequence()
{
	this->m_num_MultiTextureSequenceCount = 0;
	this->m_pd3d11Device = 0;
}

CGEMultiTextureSequence::CGEMultiTextureSequence(const CGEMultiTextureSequence& source)
{
}

CGEMultiTextureSequence& CGEMultiTextureSequence::operator=(const CGEMultiTextureSequence& source)
{
	return *this;
}

CGEMultiTextureSequence::~CGEMultiTextureSequence()
{
}

bool CGEMultiTextureSequence::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	this->m_pd3d11Device = NULL;
	this->m_pd3d11Device = pDevice;
	this->m_pDeviceContext = pDeviceContext;

	return true;
}

ID3D11ShaderResourceView** CGEMultiTextureSequence::GetTextureSequence(int resourceID)
{
	if( resourceID < 0 )
		return NULL;

	for(int i = 0; i != this->m_texVec.GetMyVectorLength(); i++)
	{
		if( resourceID == this->m_texVec[i].ID )
		{
			return this->m_texVec[i].vecTextures.GetFirstPointerAddr();
		}
	}

	return NULL;
}

int CGEMultiTextureSequence::GetMulTitextureSequenceCount()
{
	return this->m_num_MultiTextureSequenceCount;
}

int CGEMultiTextureSequence::GetTexturesCount_EachSequence(int resourceID)
{
	if( resourceID < 0 )
		return -1;

	for(int i = 0; i != this->m_texVec.GetMyVectorLength(); i++)
	{
		if( resourceID == this->m_texVec[i].ID )
		{
			return this->m_texVec[i].vecTextures.GetMyVectorLength();
		}
	}

	return -1;
}

bool CGEMultiTextureSequence::AddTexture(int resourceID, WCHAR* pPath)
{
	HRESULT result;
	WCHAR* pFilename;
	WCHAR* pFiletype;

	ID3D11ShaderResourceView* tmpTexture = NULL;

	if( NULL == pPath || resourceID < 0 )
		return false;

	// Extract filename & filetype
	this->Extract_texture_filename_filetypeW(pPath, &pFilename, &pFiletype);

	for(int i = 0; i != this->m_texVec.GetMyVectorLength(); i++)
	{
		if( resourceID == this->m_texVec[i].ID )
		{
			result = ::D3DX11CreateShaderResourceViewFromFile(this->m_pd3d11Device, pPath, NULL, NULL,
				&tmpTexture, NULL);
			if( FAILED(result) )
				return false;

			this->m_texVec[i].vecTextures.Pushback(tmpTexture);
			tmpTexture = NULL;

			this->m_texVec[i].vecTexturesFilename.Pushback(pFilename);
			pFilename = NULL;

			this->m_texVec[i].vecTexturesFiletype.Pushback(pFiletype);
			pFiletype = NULL;

			return true;
		}
	}

	GEMultiTexture* newNode = new GEMultiTexture;
	if( !newNode )
		return false;

	newNode->ID = resourceID;

	result = ::D3DX11CreateShaderResourceViewFromFile(this->m_pd3d11Device, pPath, NULL, NULL,
				&tmpTexture, NULL);
	
	if( FAILED(result) )
		return false;

	newNode->vecTextures.Pushback(tmpTexture);
	newNode->vecTexturesFilename.Pushback(pFilename);
	newNode->vecTexturesFiletype.Pushback(pFiletype);

	tmpTexture = NULL;
	pFilename = NULL;
	pFiletype = NULL;

	this->m_texVec.Pushback(*newNode);
	newNode = NULL;

	this->m_num_MultiTextureSequenceCount++;

	return true;
}

/*
 * Desc - 
 * CopyResource() 执行失败，强制将ShaderResourceView转换为ID3D11Resource后，
 * 无法复制纹理资源
 * 
 * 状态 - 待解决
 */
bool CGEMultiTextureSequence::AddShaderResroucesToSequence(int resourceID, ID3D11ShaderResourceView* pShaderResourceView, WCHAR* pPath)
{
	WCHAR* pFilename;
	WCHAR* pFiletype;

	if( pShaderResourceView == NULL || resourceID < 0 )
		return false;

	// Extract filename & filetype
	if( NULL != pPath )
		this->Extract_texture_filename_filetypeW(pPath, &pFilename, &pFiletype);

	else
	{
		pFilename = L"XXX";
		pFiletype = L"XXX";
	}

	for(int i = 0; i != this->m_texVec.GetMyVectorLength(); i++)
	{
		if( resourceID == this->m_texVec[i].ID )
		{
			this->m_texVec[i].vecTextures.Pushback(pShaderResourceView);

			this->m_texVec[i].vecTexturesFilename.Pushback(pFilename);
			pFilename = NULL;

			this->m_texVec[i].vecTexturesFiletype.Pushback(pFiletype);
			pFiletype = NULL;

			return true;
		}
	}

	GEMultiTexture* newNode = new GEMultiTexture;
	if( !newNode )
		return false;

	newNode->ID = resourceID;

	newNode->vecTextures.Pushback(pShaderResourceView);
	newNode->vecTexturesFilename.Pushback(pFilename);
	newNode->vecTexturesFiletype.Pushback(pFiletype);

	pFilename = NULL;
	pFiletype = NULL;

	this->m_texVec.Pushback(*newNode);
	newNode = NULL;

	this->m_num_MultiTextureSequenceCount++;

	return true;
}

bool CGEMultiTextureSequence::CopyFrom(const CGEMultiTextureSequence& source)
{
	this->m_texVec.Reset();
	this->m_texVec.Copy(source.m_texVec);

	this->m_num_MultiTextureSequenceCount = source.m_num_MultiTextureSequenceCount;

	return true;
}

bool CGEMultiTextureSequence::RemoveTexture(int resourceID, WCHAR* pFilename)
{
	if( resourceID < 0 || pFilename == NULL )
		return false;

	for(int i = 0; i != this->m_texVec.GetMyVectorLength(); i++)
	{
		if( resourceID == this->m_texVec[i].ID )
		{
			for(int j = 0; j != this->m_texVec[i].vecTexturesFilename.GetMyVectorLength(); i++)
			{
				if( 0 == lstrcmpW(pFilename, this->m_texVec[i].vecTexturesFilename[j]) )
				{
					this->m_texVec[i].vecTextures.Remove(j);
					this->m_texVec[i].vecTexturesFilename.Remove(j);
					this->m_texVec[i].vecTexturesFiletype.Remove(j);

					return true;
				}
			}

			return false;
		}
	}

	return false;
}

bool CGEMultiTextureSequence::RemoveTextureSequence(int resourceID)
{
	if( resourceID < 0 )
		return false;

	for(int i = 0; i != this->m_texVec.GetMyVectorLength(); i++)
	{
		if( resourceID == this->m_texVec[i].ID )
		{
			this->m_texVec[i].vecTextures.Reset();
			this->m_texVec[i].vecTexturesFilename.Reset();
			this->m_texVec[i].vecTexturesFiletype.Reset();
			this->m_texVec.Remove(i);
			this->m_num_MultiTextureSequenceCount--;
			return true;
		}
	}

	return false;
}

void CGEMultiTextureSequence::Shutdown()
{
	this->m_texVec.Reset();
	this->m_num_MultiTextureSequenceCount = 0;
}

/*
 * Function name - Extract_filename_filetype
 * Params - char*(pPath), char**(_name), char**(_type)
 * Author - CuiXue
 * Desc - 
 *
 */
bool CGEMultiTextureSequence::Extract_texture_filename_filetype(char* pPath, char** _name, char** _type)
{
	if( NULL == pPath )
		return false;

	*_name = NULL;
	*_type = NULL;

	char* ptr = pPath;
	int len = strlen(ptr);
	int i = 0;

	while( *ptr != '\0' && i != len )
	{
		if( *ptr == '.' && ((i + 3) != len) )
		{
			if( *(ptr + 1) == 'd' && *(ptr + 2) == 'd' && *(ptr + 3) == 's' )
			{
				int curLen = strlen(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					char *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}

				return true;
			}

			if( *(ptr + 1) == 'j' && *(ptr + 2) == 'p' && *(ptr + 3) == 'g' )
			{
				int curLen = strlen(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					char *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}
				return true;
			}

			if( *(ptr + 1) == 'b' && *(ptr + 2) == 'm' && *(ptr + 3) == 'p' )
			{
				int curLen = strlen(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					char *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}
				return true;
			}
		}

		i++;
		ptr++;
	}

	return true;
}

/*
 * Function name - Extract_filename_filetype
 * Params - WCHAR*(pPath), WCHAR**(_name), WCHAR**(_type)
 * Author - CuiXue
 * Desc - 
 *
 */
bool CGEMultiTextureSequence::Extract_texture_filename_filetypeW(WCHAR* pPath, WCHAR** _name, WCHAR** _type)
{
	if( NULL == pPath )
		return false;

	*_name = NULL;
	*_type = NULL;

	WCHAR* ptr = pPath;
	int len = lstrlenW(ptr);
	int i = 0;

	while( *ptr != '\0' && i != len )
	{
		if( *ptr == '.' && ((i + 3) != len) )
		{
			if( *(ptr + 1) == 'd' && *(ptr + 2) == 'd' && *(ptr + 3) == 's' )
			{
				int curLen = lstrlenW(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					WCHAR *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}

				return true;
			}

			if( *(ptr + 1) == 'j' && *(ptr + 2) == 'p' && *(ptr + 3) == 'g' )
			{
				int curLen = lstrlenW(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					WCHAR *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}
				return true;
			}

			if( *(ptr + 1) == 'b' && *(ptr + 2) == 'm' && *(ptr + 3) == 'p' )
			{
				int curLen = lstrlenW(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					WCHAR *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}
				return true;
			}
		}

		i++;
		ptr++;
	}

	return true;
}





