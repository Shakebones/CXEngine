/*
 * File name - GETextureShader.cpp
 * Author - CuiXue
 * Updated date - 2012.12.21
 * Desc -
 *
 */
#include "GETextureShader.h"

CGETextureShader::CGETextureShader()
{
	this->m_pInputLayout = 0;
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pSamplerState = 0;
}

CGETextureShader::CGETextureShader(const CGETextureShader& source)
{
}

CGETextureShader& CGETextureShader::operator=(const CGETextureShader& source)
{
	return *this;
}

CGETextureShader::~CGETextureShader()
{
}

bool CGETextureShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	result = this->_initialize_shader(pDevice, hWnd, L"..\\Include\\Shaders\\Texture.vs", 
		L"..\\Include\\Shaders\\Texture.ps");
	if( !result )
		return false;

	return true;
}

void CGETextureShader::Shutdown()
{
	this->_shutdown();
}

bool CGETextureShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
							  D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
							  ID3D11ShaderResourceView* pTexture)
{
	bool result;
	result = this->_set_shader_params(pDeviceContext, world, view, projection, pTexture);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);
	return true;
}

bool CGETextureShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd,
										  WCHAR* vs, WCHAR* ps)
{
	HRESULT result;
	ID3D10Blob*		pErrorMessage;
	ID3D10Blob*		pVertexShaderBuffer;
	ID3D10Blob*		pPixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	pErrorMessage = 0;
	pVertexShaderBuffer = 0;
	pPixelShaderBuffer = 0;

	// Compile vertex shader & pixel shader

	result = ::D3DX11CompileFromFile(vs, NULL, NULL,
		"TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		NULL, &pVertexShaderBuffer, &pErrorMessage, NULL);

	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, vs);
		}

		else
		{
			::MessageBox(hWnd, vs, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = ::D3DX11CompileFromFile(ps, NULL, NULL,
		"TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		NULL, &pPixelShaderBuffer, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, ps);
		}

		else
		{
			::MessageBox(hWnd, ps, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create vertex shader & pixel shader
	result = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(), 
		pVertexShaderBuffer->GetBufferSize(), NULL, &this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
		pPixelShaderBuffer->GetBufferSize(), NULL, &this->m_pPixelShader);
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

	// Create input layout, which describe vertex structure.
	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShaderBuffer);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShaderBuffer);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	// 可控参数标记 - 采样器状态
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
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

void CGETextureShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGETextureShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError = NULL;
	unsigned long bufferSize = 0;
	unsigned long i;

	ofstream fout;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(i = 0; i < bufferSize; i++)
	{
		fout << pCompileError[i];
	}

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.",
		pShaderFilename, MB_OK);

	return;
}

bool CGETextureShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext,
										  D3DXMATRIX world,							// World matrix
										  D3DXMATRIX view,							// View matrix
										  D3DXMATRIX projection,					// Projection matrix
										  ID3D11ShaderResourceView* pTexture)		// Texture source
{
	HRESULT						result;
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	MatrixBufferType*			pData;
	unsigned int				bufferNum;

	// Transpose matrix.
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD,
		0, &mappedResource);
	if( FAILED(result) )
		return false;

	pData = (MatrixBufferType*)mappedResource.pData;
	pData->world = world;
	pData->view = view;
	pData->proj = projection;
	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	bufferNum = 0;

	// Set constant value.
	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &this->m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);
	return true;
}

void CGETextureShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

