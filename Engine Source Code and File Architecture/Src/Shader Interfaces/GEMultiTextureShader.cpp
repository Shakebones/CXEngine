/*
 * File name - GEMultiTexture.cpp
 * Author - CuiXue
 * Updated date - 2013.3.1
 * Desc - 
 *
 */
#include "GEMultiTextureShader.h"

CGEMultiTextureShader::CGEMultiTextureShader()
{
	this->m_pInputLayout = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pSamplerState = 0;
	this->m_pPSConstantBuffer = 0;
}

CGEMultiTextureShader::CGEMultiTextureShader(const CGEMultiTextureShader& source)
{
}

CGEMultiTextureShader& CGEMultiTextureShader::operator=(const CGEMultiTextureShader& source)
{
	return *this;
}

CGEMultiTextureShader::~CGEMultiTextureShader()
{
}

bool CGEMultiTextureShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	result = this->_initialize_shader(pDevice, hWnd, L"../Include/Shaders/MultiTexture_2tex.vs",
		L"../Include/Shaders/MultiTexture_2tex.ps");
	if( !result )
		return false;

	return true;
}

void CGEMultiTextureShader::Shutdown()
{
	this->_shutdown();
}

bool CGEMultiTextureShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, int numOfTextures, D3DXMATRIX world,
							 D3DXMATRIX view, D3DXMATRIX proj, ID3D11ShaderResourceView** ppTextureArray)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, numOfTextures, world, view, proj, ppTextureArray);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGEMultiTextureShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* pErrorMessage;
	ID3D10Blob* pVertexShaderBuf;
	ID3D10Blob* pPixelShaderBuf;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElem;
	D3D11_BUFFER_DESC matrixBuffer;
	D3D11_SAMPLER_DESC samplerDesc;

	pErrorMessage = 0;
	pVertexShaderBuf = 0;
	pPixelShaderBuf = 0;

	result = ::D3DX11CompileFromFile(vsFilename, NULL, NULL, "MultiTextureVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuf, &pErrorMessage, NULL);
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

	result = ::D3DX11CompileFromFile(psFilename, NULL, NULL, "MultiTexturePS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pPixelShaderBuf, &pErrorMessage, NULL);
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

	result = pDevice->CreateVertexShader(pVertexShaderBuf->GetBufferPointer(), pVertexShaderBuf->GetBufferSize(), NULL,
		&this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShaderBuf->GetBufferPointer(), pPixelShaderBuf->GetBufferSize(), NULL,
		&this->m_pPixelShader);
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

	numElem = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = pDevice->CreateInputLayout(polygonLayout, numElem, pVertexShaderBuf->GetBufferPointer(),
		pVertexShaderBuf->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShaderBuf);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShaderBuf);

	matrixBuffer.Usage = D3D11_USAGE_DYNAMIC;
	matrixBuffer.ByteWidth = sizeof(MatrixBufferType);
	matrixBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBuffer.MiscFlags = 0;
	matrixBuffer.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBuffer, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

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

	return true;
}

void CGEMultiTextureShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pPSConstantBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGEMultiTextureShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileErrors;
	unsigned long bufferSize;
	ofstream fout;

	pCompileErrors = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(int i = 0; i != bufferSize; i++)
	{
		fout << pCompileErrors[i];
	}

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", L"Error", MB_OK);

	return;
}

bool CGEMultiTextureShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, 
										 int num_of_textures,
										 D3DXMATRIX world,
										 D3DXMATRIX view,
										 D3DXMATRIX proj,
										 ID3D11ShaderResourceView** ppTextureArray)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	MatrixBufferType* pData;
	unsigned int bufferNumber;

	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&proj, &proj);

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData = (MatrixBufferType*)mappedResources.pData;

	pData->world = world;
	pData->view = view;
	pData->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	bufferNumber = 0;

	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, num_of_textures, ppTextureArray);

	// Pixel Shader constant buffer.

	return true;
}

void CGEMultiTextureShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}