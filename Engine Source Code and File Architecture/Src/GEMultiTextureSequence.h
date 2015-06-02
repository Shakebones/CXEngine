/*
 * File name - GETextureSequence.h
 * Author - CuiXue
 * Updated date - 2013.3.2
 * Desc - 
 *
 * 修改中 ——
 * 是否要为纹理序列增加一个数据结构，并添加纹理ID？
 */
#ifndef _GE_TEXTURE_SEQUENCE_H_
#define _GE_TEXTURE_SEQUENCE_H_

#ifdef _WIN32

#include <D3D11.h>
#include <D3DX11.h>

#endif

#include "Utility.h"

typedef struct _multitexture_res_node
{
	int ID;
	MyVector<WCHAR*> vecTexturesFilename;
	MyVector<WCHAR*> vecTexturesFiletype;
	MyVector<ID3D11ShaderResourceView*> vecTextures;
}GEMultiTexture, *pGEMultiTexture;

/*
 * Class name - CGEMultiTextureSequence
 * Author - CuiXue
 * Updated date - 2013.3.2
 * Desc - 
 *
 */
class CGEMultiTextureSequence
{
public:
	CGEMultiTextureSequence();
	CGEMultiTextureSequence(const CGEMultiTextureSequence&);
	CGEMultiTextureSequence& operator=(const CGEMultiTextureSequence&);
	~CGEMultiTextureSequence();

public:
	// Functions
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
	bool AddTexture(int, WCHAR*);
	bool AddShaderResroucesToSequence(int, ID3D11ShaderResourceView*, WCHAR*);
	bool CopyFrom(const CGEMultiTextureSequence&);
	bool RemoveTexture(int, WCHAR*);
	bool RemoveTextureSequence(int);
	void Shutdown();

public:
	// Access
	ID3D11ShaderResourceView** GetTextureSequence(int);
	int GetMulTitextureSequenceCount();
	int GetTexturesCount_EachSequence(int);

private:
	bool Extract_texture_filename_filetype(char*, char**, char**);
	bool Extract_texture_filename_filetypeW(WCHAR*, WCHAR**, WCHAR**);

private:
	int		m_num_MultiTextureSequenceCount;
	MyVector<GEMultiTexture> m_texVec;
	ID3D11Device* m_pd3d11Device;
	ID3D11DeviceContext* m_pDeviceContext;
};

#endif