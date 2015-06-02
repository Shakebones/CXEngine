/*
 * File name - GEText.h
 * Author - CuiXue
 * Updated date - 2013.1.3
 * Desc -
 *
 */
#ifndef _GE_TEXT_H_
#define _GE_TEXT_H_

#ifdef _WIN32
#include "GEFont.h"
#include "GEFontShader.h"
#endif



/*
 * Class name - CGEText
 * Author - CuiXue
 * Updated date - 2013.1.3
 * Desc - 
 *
 */
class CGEText
{
public:
	// Interfaces
	CGEText();
	CGEText(const CGEText&);
	CGEText& operator=(const CGEText&);
	~CGEText();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);
	bool Text(ID3D11Device*, ID3D11DeviceContext*, int, char*, int, int, int, D3DXVECTOR4);

private:
	// Kernels
	bool _initialize_sentence(SentenceType**, int, ID3D11Device*);
	bool _update_sentence(SentenceType*, char*, int, int, int, float, float, float, ID3D11DeviceContext*);
	void _release_sentence(SentenceType**);
	bool _render_sentence(ID3D11DeviceContext*, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
	CGEFont*		m_pFont;
	CGEFontShader*	m_pFontShader;
	int				m_screenWidth;
	int				m_screenHeight;
	int				m_num_of_sentences;
	D3DXMATRIX		m_baseViewMatrix;

	// Modifying...
	MyVector<SentenceType*> m_myvec_sentences;
	//SentenceType*	m_sentence1;
	//SentenceType*	m_sentence2;
};

#endif