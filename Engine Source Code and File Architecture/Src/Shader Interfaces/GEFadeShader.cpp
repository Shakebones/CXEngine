/*
 * File name - CGEFadeShader.cpp
 * Author - CuiXue
 * Updated date - 2013.4.15
 * Desc - 
 * 
 */
#include "GEFadeShader.h"

CGEFadeShader::CGEFadeShader()
{
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pInputLayout = 0;
	this->m_pSamplerState = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pFadeFactorBuffer = 0;
}

CGEFadeShader::CGEFadeShader(const CGEFadeShader& source)
{
}

CGEFadeShader& CGEFadeShader::operator=(const CGEFadeShader& source)
{
	return *this;
}

CGEFadeShader::~CGEFadeShader()
{
}

bool CGEFadeShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shaders(pDevice, hWnd, L"../Include/Shaders/Fade.vs",
													  L"../Include/Shaders/Fade.ps");
	if( !result )
		return false;

	return true;
}

void CGEFadeShader::Shutdown()
{
	this->_shutdown();
}

bool CGEFadeShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
						   D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
						   ID3D11ShaderResourceView* pTexture,
						   float fadeFactor)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, pTexture, fadeFactor);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGEFadeShader::_initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	unsigned int numElements;
	ID3D10Blob* pVertexShaderBuf;
	ID3D10Blob* pPixelShaderBuf;
	ID3D10Blob* pErrorMessage;
	
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC fadeFactorBufferDesc;

	pVertexShaderBuf = 0;
	pPixelShaderBuf = 0;
	pErrorMessage = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "FadeVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShaderBuf, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, vsFilename);
		}

		else
		{
			MessageBox(hWnd, vsFilename, L"Missing Shade File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "FadePS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pPixelShaderBuf, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, psFilename);
		}

		else
		{
			MessageBox(hWnd, vsFilename, L"Missing Shade File", MB_OK);
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
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShaderBuf->GetBufferPointer(),
		pVertexShaderBuf->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	fadeFactorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fadeFactorBufferDesc.ByteWidth = sizeof(FadeBufferType);
	fadeFactorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fadeFactorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fadeFactorBufferDesc.MiscFlags = 0;
	fadeFactorBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&fadeFactorBufferDesc, NULL, &this->m_pFadeFactorBuffer);
	if( FAILED(result) )
		return false;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxAnisotropy = 1;
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

void CGEFadeShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pFadeFactorBuffer);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
}

void CGEFadeShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError;
	unsigned long bufferSize;

	fstream fout;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(int i = 0; i != bufferSize; ++i)
	{
		fout << pCompileError[i];
	}

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", L"Error", MB_OK);

	return;
}

bool CGEFadeShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext,
									   D3DXMATRIX world,
									   D3DXMATRIX view,
									   D3DXMATRIX proj,
									   ID3D11ShaderResourceView* pTexture,
									   float _fadeFactor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;

	MatrixBufferType* pData_matrix;
	FadeBufferType* pData_fade;
	unsigned int bufferNum;

	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&proj, &proj);

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_matrix = (MatrixBufferType*)mappedResources.pData;

	pData_matrix->world = world;
	pData_matrix->view = view;
	pData_matrix->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	bufferNum = 0;

	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &this->m_pMatrixBuffer);

	result = pDeviceContext->Map(this->m_pFadeFactorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_fade = (FadeBufferType*)mappedResources.pData;

	pData_fade->fadeFactor = _fadeFactor;

	pDeviceContext->Unmap(this->m_pFadeFactorBuffer, 0);

	bufferNum = 0;
	pDeviceContext->PSSetConstantBuffers(bufferNum, 1, &this->m_pFadeFactorBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	return true;
}

void CGEFadeShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}