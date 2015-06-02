/*
 * File name - GEFogShader.cpp
 * Author - CuiXue
 * Updated date - 2013.3.25
 * Desc - 
 *
 */

#include "GEFogShader.h"

CGEFogShader::CGEFogShader()
{
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pInputLayout = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pFogParamsBuffer = 0;
	this->m_pSamplerState = 0;
	this->m_pFogColorInPS = 0;
}

CGEFogShader::CGEFogShader(const CGEFogShader& source)
{
}

CGEFogShader& CGEFogShader::operator=(const CGEFogShader& source)
{
	return *this;
}

CGEFogShader::~CGEFogShader()
{
}

bool CGEFogShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shader(pDevice, hWnd, L"../Include/Shaders/Fog.vs",
													 L"../Include/Shaders/Fog.ps");
	if( !result )
		return false;

	return true;
}

void CGEFogShader::Shutdown()
{
	this->_shutdown();
}

bool CGEFogShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
						  D3DXMATRIX world,
						  D3DXMATRIX view,
						  D3DXMATRIX proj,
						  ID3D11ShaderResourceView* pTexture,
						  D3DXVECTOR4 fogColor,
						  float fogStart,
						  float fogEnd)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, 
									  pTexture, fogColor, fogStart, fogEnd);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGEFogShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* pErrorMessage;
	ID3D10Blob* pVertexShaderBuf;
	ID3D10Blob* pPixelShaderBuf;
	
	D3D11_BUFFER_DESC matrixBufDesc;
	D3D11_BUFFER_DESC fogParamsBufDesc;
	D3D11_BUFFER_DESC fogParamsInPS;

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	unsigned int numElements;

	pErrorMessage = 0;
	pVertexShaderBuf = 0;
	pPixelShaderBuf = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "FogVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "FogPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	result = pDevice->CreateVertexShader(pVertexShaderBuf->GetBufferPointer(),
		pVertexShaderBuf->GetBufferSize(), NULL, &this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShaderBuf->GetBufferPointer(),
		pPixelShaderBuf->GetBufferSize(), NULL, &this->m_pPixelShader);
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

	matrixBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufDesc.MiscFlags = 0;
	matrixBufDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	fogParamsBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogParamsBufDesc.ByteWidth = sizeof(_fog_constant_buffer);
	fogParamsBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogParamsBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogParamsBufDesc.MiscFlags = 0;
	fogParamsBufDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&fogParamsBufDesc, NULL, &this->m_pFogParamsBuffer);
	if( FAILED(result) )
		return false;

	fogParamsInPS.Usage = D3D11_USAGE_DYNAMIC;
	fogParamsInPS.ByteWidth = sizeof(_fog_color);
	fogParamsInPS.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogParamsInPS.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogParamsInPS.MiscFlags = 0;
	fogParamsInPS.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&fogParamsInPS, NULL, &this->m_pFogColorInPS);
	if( FAILED(result) )
		return false;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MipLODBias = 0.0f;
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

void CGEFogShader::_shutdown()
{
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_pFogParamsBuffer);
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGEFogShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError;
	unsigned long bufferSize;

	ofstream fout;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(int i = 0; i != bufferSize; i++)
	{
		fout << pCompileError[i];
	}

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", pShaderFilename, MB_OK);

	return;
}

bool CGEFogShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX world,
									  D3DXMATRIX view,
									  D3DXMATRIX proj,
									  ID3D11ShaderResourceView* pTexture,
									  D3DXVECTOR4 fog_color,
									  float fogStart,
									  float fogEnd)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	MatrixBufferType* pData_Matrix;
	_fog_color* pData_fogColor;
	_fog_constant_buffer* pData_Fog;
	
	unsigned bufferNumber;

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_Matrix = (MatrixBufferType*)mappedResources.pData;
	pData_Matrix->world = world;
	pData_Matrix->view = view;
	pData_Matrix->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	bufferNumber = 0;
	
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	result = pDeviceContext->Map(this->m_pFogParamsBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_Fog = (_fog_constant_buffer*)mappedResources.pData;

	pData_Fog->start = fogStart;
	pData_Fog->end = fogEnd;

	pDeviceContext->Unmap(this->m_pFogParamsBuffer, 0);

	bufferNumber = 1;

	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_pFogParamsBuffer);

	result = pDeviceContext->Map(this->m_pFogColorInPS, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_fogColor = (_fog_color*)mappedResources.pData;
	pData_fogColor->fog_color = fog_color;

	pDeviceContext->Unmap(this->m_pFogColorInPS, 0);

	bufferNumber = 0;

	pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &this->m_pFogColorInPS);

	return true;
}

void CGEFogShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

