/*
 * File name - GEGlassShader.cpp
 * Author - CuiXue
 * Updated date - 2013.5.26
 * Desc - 
 *
 */
#include "GEGlassShader.h"

CGEGlassShader::CGEGlassShader()
{
	this->m_pGlassBuffer = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pInputLayout = 0;
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pSamplerState = 0;
}

CGEGlassShader::CGEGlassShader(const CGEGlassShader& source)
{
}

CGEGlassShader& CGEGlassShader::operator=(const CGEGlassShader& source)
{
	return *this;
}

CGEGlassShader::~CGEGlassShader()
{
}

bool CGEGlassShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shaders(pDevice, hWnd, L"../Include/Shaders/Glass.vs",
													  L"../Include/Shaders/Glass.ps");
	if( !result )
		return false;

	return true;
}

void CGEGlassShader::Shutdown()
{
	this->_shutdown();
}

bool CGEGlassShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,						
							ID3D11ShaderResourceView* colorTex, ID3D11ShaderResourceView* normalTex, ID3D11ShaderResourceView* refractionTex,
							float refractionScale)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, colorTex, normalTex, refractionTex, refractionScale);
	if( !result )
		return false;

	this->_render_shaders(pDeviceContext, indexCount);

	return true;
}

bool CGEGlassShader::_initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* pVertexShaderBuf;
	ID3D10Blob* pPixelShaderBuf;
	ID3D10Blob* pErrorMessage;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC glassBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	unsigned int numElements;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "GlassShaderVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&pVertexShaderBuf, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, vsFilename);
		}

		else
		{
			MessageBox(hWnd, vsFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "GlassShaderPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pPixelShaderBuf, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, psFilename);
		}

		else
		{
			MessageBox(hWnd, psFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	result = pDevice->CreateVertexShader(pVertexShaderBuf->GetBufferPointer(), pVertexShaderBuf->GetBufferSize(), NULL, &this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShaderBuf->GetBufferPointer(), pPixelShaderBuf->GetBufferSize(), NULL, &this->m_pPixelShader);
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

	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShaderBuf->GetBufferPointer(), pVertexShaderBuf->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShaderBuf);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShaderBuf);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	glassBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	glassBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	glassBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	glassBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	glassBufferDesc.MiscFlags = 0;
	glassBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&glassBufferDesc, NULL, &this->m_pGlassBuffer);
	if( FAILED(result) )
		return false;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MipLODBias = 0.0f;
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

void CGEGlassShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pGlassBuffer);
}

void CGEGlassShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError;
	unsigned long bufferSize;

	ofstream fout;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(int i = 0; i != bufferSize; i++)
		fout << pCompileError[i];

	fout.close();

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", pShaderFilename, MB_OK);
}

bool CGEGlassShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
										ID3D11ShaderResourceView* colorTex, ID3D11ShaderResourceView* normalTex, ID3D11ShaderResourceView* refractionTex,
										float glassFactor)
{
	HRESULT result;
	
	unsigned int numBuffers;

	MatrixBufferType *pData_matrix;
	GlassFactorBufferType* pData_glass;
	D3D11_MAPPED_SUBRESOURCE mappedResources;

	::D3DXMatrixTranspose(&world, &world);
	::D3DXMatrixTranspose(&view, &view);
	::D3DXMatrixTranspose(&proj, &proj);

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_matrix = (MatrixBufferType*)mappedResources.pData;

	pData_matrix->world = world;
	pData_matrix->view = view;
	pData_matrix->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	numBuffers = 0;

	pDeviceContext->VSSetConstantBuffers(numBuffers, 1, &this->m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &colorTex);
	pDeviceContext->PSSetShaderResources(1, 1, &normalTex);
	pDeviceContext->PSSetShaderResources(2, 1, &refractionTex);

	result = pDeviceContext->Map(this->m_pGlassBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_glass = (GlassFactorBufferType*)mappedResources.pData;
	pData_glass->refractionScale = glassFactor;
	pData_glass->padding = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	pDeviceContext->Unmap(this->m_pGlassBuffer, 0);

	numBuffers = 0;

	pDeviceContext->PSSetConstantBuffers(numBuffers, 1, &this->m_pGlassBuffer);

	return true;
}

void CGEGlassShader::_render_shaders(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}
