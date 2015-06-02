/*
 * File name - GETextureTranslate.cpp
 * Author - CuiXue
 * Updated date - 2013.4.7
 * Desc - 
 *
 */
#include "GETextureTranslate.h"

CGETextureTranslate::CGETextureTranslate()
{
	this->m_pInputLayout = 0;
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pInputLayout = 0;
	this->m_pTranslationBuffer = 0;
}

CGETextureTranslate::CGETextureTranslate(const CGETextureTranslate& source)
{
}

CGETextureTranslate& CGETextureTranslate::operator=(const CGETextureTranslate& source)
{
	return *this;
}

CGETextureTranslate::~CGETextureTranslate()
{
}

bool CGETextureTranslate::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shaders(pDevice, hWnd, L"..\\Include\\Shaders\\TextureTranslation.vs",
													  L"..\\Include\\Shaders\\TextureTranslation.ps");
	if( !result )
		return false;

	return true;
}

void CGETextureTranslate::Shutdown()
{
	this->_shutdown();
}

bool CGETextureTranslate::Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
								 D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
								 ID3D11ShaderResourceView* pTexture,
								 float translation)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, pTexture, translation);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGETextureTranslate::_initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	unsigned int numElements;
	ID3D10Blob* pVertexShaderBuf;
	ID3D10Blob* pPixelShaderBuf;
	ID3D10Blob* pErrorMessage;

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC texTranslationBufferDesc;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	pVertexShaderBuf = 0;
	pPixelShaderBuf = 0;
	pErrorMessage = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "TexTranslationVS", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pVertexShaderBuf, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, vsFilename);
		}

		else
		{
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "TexTranslationPS", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pPixelShaderBuf, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, psFilename);
		}

		else
		{
			MessageBox(hWnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = pDevice->CreateVertexShader(pVertexShaderBuf->GetBufferPointer(), pVertexShaderBuf->GetBufferSize(),
		NULL, &this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShaderBuf->GetBufferPointer(), pPixelShaderBuf->GetBufferSize(),
		NULL, &this->m_pPixelShader);
	if( FAILED(result) )
		return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
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

	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShaderBuf->GetBufferPointer(),
		pVertexShaderBuf->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShaderBuf);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShaderBuf);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	texTranslationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	texTranslationBufferDesc.ByteWidth = sizeof(TranslationBufferType);
	texTranslationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	texTranslationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texTranslationBufferDesc.MiscFlags = 0;
	texTranslationBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&texTranslationBufferDesc, NULL, &this->m_pTranslationBuffer);
	if(FAILED(result))
	{
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = pDevice->CreateSamplerState(&samplerDesc, &this->m_pSamplerState);
	if( FAILED(result) )
		return false;

	return true;
}

void CGETextureTranslate::_shutdown()
{
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGETextureTranslate::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError;
	unsigned long bufferSize;
	fstream fout;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(int i = 0; i != bufferSize; i++)
	{
		fout << pCompileError[i];
	}

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", L"Error", MB_OK);
}

bool CGETextureTranslate::_set_shader_params(ID3D11DeviceContext* pDeviceContext, 
											 D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
											 ID3D11ShaderResourceView* pTexture,
											 float translation)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	MatrixBufferType* pData_MatrixBuf;
	TranslationBufferType* pData_Translation;

	unsigned int numBuffer;
	
	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( !result )
		return false;

	pData_MatrixBuf = (MatrixBufferType*)mappedResources.pData;

	pData_MatrixBuf->world = world;
	pData_MatrixBuf->view = view;
	pData_MatrixBuf->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	numBuffer = 0;

	pDeviceContext->VSSetConstantBuffers(numBuffer, 0, &this->m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	result = pDeviceContext->Map(this->m_pTranslationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( !result )
		return false;

	pData_Translation = (TranslationBufferType*)mappedResources.pData;

	pData_Translation->texTranslation = translation;

	pDeviceContext->Unmap(this->m_pTranslationBuffer, 0);

	numBuffer = 0;

	pDeviceContext->PSSetConstantBuffers(numBuffer, 1, &this->m_pTranslationBuffer);

	return true;
}

void CGETextureTranslate::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

