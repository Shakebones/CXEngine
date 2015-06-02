/*
 * File name - GEFontShader.cpp
 * Author - CuiXue
 * Updated date - 2013.1.2
 * Desc -
 *
 */
#include "GEFontShader.h"

CGEFontShader::CGEFontShader()
{
	m_pVertexShader		= 0;
	m_pPixelShader		= 0;
	m_pLayout			= 0;
	m_pConstantBuffer	= 0;
	m_pSamplerState		= 0;
	m_pFontPixelBuffer	= 0;
}

CGEFontShader::CGEFontShader(const CGEFontShader& source)
{
}

CGEFontShader& CGEFontShader::operator=(const CGEFontShader& source)
{
	return *this;
}

CGEFontShader::~CGEFontShader()
{
}

bool CGEFontShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shader(pDevice, hWnd, L"..\\Include\\Shaders\\Font.vs", L"..\\Include\\Shaders\\Font.ps");
	if( !result )
		return false;

	return true;
}

void CGEFontShader::Shutdown()
{
	this->_shutdown();
}

bool CGEFontShader::Render(ID3D11DeviceContext* pDeviceContext, 
						   int indexCount, 
						   D3DXMATRIX world, 
						   D3DXMATRIX view, 
						   D3DXMATRIX proj, 
						   ID3D11ShaderResourceView* pTexture, 
						   D3DXVECTOR4 pixelColor)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, pTexture, pixelColor);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGEFontShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd, WCHAR* vs, WCHAR* ps)
{
	HRESULT result;
	ID3D10Blob* pErrorMessage;
	ID3D10Blob* pVertexShader;
	ID3D10Blob* pPixelShader;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;

	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	pErrorMessage = 0;
	pVertexShader = 0;
	pPixelShader = 0;

	result = ::D3DX11CompileFromFile(vs, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pVertexShader, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, vs);
		}

		else
		{
			::MessageBox(hWnd, vs, L"Missing shader file", MB_OK);
		}

		return false;
	}

	result = ::D3DX11CompileFromFile(ps, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pPixelShader, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, ps);
		}

		else
		{
			::MessageBox(hWnd, ps, L"Missing shader file", MB_OK);
		}

		return false;
	}

	result = pDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &this->m_pPixelShader);
	if( FAILED(result) )
		return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShader->GetBufferPointer(),
		pVertexShader->GetBufferSize(), &this->m_pLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShader);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShader);

	// Set the description of the dynamic constant buffer.
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&constantBufferDesc, NULL, &this->m_pConstantBuffer);
	if( FAILED(result) )
		return false;

	// Set description of the sampler state structure.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = pDevice->CreateSamplerState(&samplerDesc, &this->m_pSamplerState);
	if( FAILED(result) )
		return false;

	// Set the description of the dynamic pixel constant buffer.
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&pixelBufferDesc, NULL, &this->m_pFontPixelBuffer);
	if( FAILED(result) )
		return false;

	return true;
}

void CGEFontShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pFontPixelBuffer);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pConstantBuffer);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pLayout);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGEFontShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR*  shaderFilename)
{
	char* compileErrors = NULL;
	unsigned long bufferSize;
	unsigned long i;
	ofstream fout;

	compileErrors = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();
	fout.open("shader-error.txt");

	for(i = 0; i < bufferSize; i++)
		fout << compileErrors[i];

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);
	::MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.",
		shaderFilename, MB_OK);
	
	return;
}

bool CGEFontShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, 
									   D3DXMATRIX world,
									   D3DXMATRIX view,
									   D3DXMATRIX proj,
									   ID3D11ShaderResourceView* pTexture,
									   D3DXVECTOR4 pixelColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* pData_matrix;
	unsigned int bufferNum;
	PixelBufferType* pData_font_pixel;



	result = pDeviceContext->Map(this->m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) )
		return false;

	pData_matrix = (MatrixBufferType*)mappedResource.pData;

	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&proj, &proj);

	pData_matrix->world = world;
	pData_matrix->view = view;
	pData_matrix->proj = proj;
	pDeviceContext->Unmap(this->m_pConstantBuffer, 0);

	bufferNum = 0;

	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &this->m_pConstantBuffer);

	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	result = pDeviceContext->Map(this->m_pFontPixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) )
		return false;

	pData_font_pixel = (PixelBufferType*)mappedResource.pData;
	pData_font_pixel->pixelColor = pixelColor;	
	pDeviceContext->Unmap(this->m_pFontPixelBuffer, 0);

	bufferNum = 0;
	pDeviceContext->PSSetConstantBuffers(bufferNum, 1, &this->m_pFontPixelBuffer);

	return true;
}

void CGEFontShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}