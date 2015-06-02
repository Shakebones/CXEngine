/*
 * File name - GEFireShader.cpp
 * Author - CuiXue
 * Updated date - 2013.7.6
 * Desc - 
 *
 */
#include "GEFireShader.h"

CGEFireShader::CGEFireShader()
{
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pInputLayout = 0;
	this->m_pSamplerState = 0;
	this->m_pClampSamplerState = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pNoiseBuffer = 0;
	this->m_pDistortionBuffer = 0;
}

CGEFireShader::CGEFireShader(const CGEFireShader& source)
{
}

CGEFireShader& CGEFireShader::operator=(const CGEFireShader& source)
{
	return *this;
}

CGEFireShader::~CGEFireShader()
{
}

bool CGEFireShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	
	result = this->_initialize_shaders(pDevice, hWnd, L"../Include/Shaders/Fire.vs",
													  L"../Include/Shaders/Fire.ps");
	if( !result )
		return false;

	return true;
}

void CGEFireShader::Shutdown()
{
	this->_shutdown();
}

bool CGEFireShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, D3DXMATRIX world, D3DXMATRIX view,
						   D3DXMATRIX proj, ID3D11ShaderResourceView** ppTextureArray, float frameTime, D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, 
						   D3DXVECTOR2 distortion1, D3DXVECTOR2 distortion2, D3DXVECTOR2 distortion3,
						   float distortionScale, float distortionBias)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, ppTextureArray, frameTime,
		scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
	if( !result )
		return false;

	this->_render_shaders(pDeviceContext, indexCount);

	return true;
}

bool CGEFireShader::_initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* pVertexShaderBuf = 0;
	ID3D10Blob* pPixelShaderBuf = 0;
	ID3D10Blob* pErrorMessage = 0;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC samplerDesc_clamp;
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC noiseBufferDesc;
	D3D11_BUFFER_DESC distortionBufferDesc;

	result = D3DX11CompileFromFile(vsFilename, NULL, 0, "FireVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
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

	result = D3DX11CompileFromFile(psFilename, NULL, 0, "FirePS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pPixelShaderBuf, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, psFilename);
		}

		else
		{
			MessageBox(hWnd, psFilename, L"Missing Shade File", MB_OK);
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

	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&noiseBufferDesc, NULL, &this->m_pNoiseBuffer);
	if( FAILED(result) )
		return false;

	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&distortionBufferDesc, NULL, &this->m_pDistortionBuffer);
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

	samplerDesc_clamp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc_clamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc_clamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc_clamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc_clamp.MipLODBias = 0.0f;
    samplerDesc_clamp.MaxAnisotropy = 1;
    samplerDesc_clamp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc_clamp.BorderColor[0] = 0;
	samplerDesc_clamp.BorderColor[1] = 0;
	samplerDesc_clamp.BorderColor[2] = 0;
	samplerDesc_clamp.BorderColor[3] = 0;
    samplerDesc_clamp.MinLOD = 0;
    samplerDesc_clamp.MaxLOD = D3D11_FLOAT32_MAX;

	result = pDevice->CreateSamplerState(&samplerDesc, &this->m_pSamplerState);
	if( FAILED(result) )
		return false;


	return true;
}

void CGEFireShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pDistortionBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pNoiseBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pClampSamplerState);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
}

void CGEFireShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError = 0;
	unsigned long bufferSize = 0;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fstream fout;

	fout.open("shader-error.txt");
	for(int i = 0; i != bufferSize; i++)
		fout << pCompileError[i];

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", L"Error", MB_OK);
}

bool CGEFireShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
									   ID3D11ShaderResourceView** ppTextureArray, float frameTime, D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, 
									   D3DXVECTOR2 distortion1, D3DXVECTOR2 distortion2, D3DXVECTOR2 distortion3, float distortionScale, float distortionBias)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* pData_matrix;
	NoiseBufferType* pData_noise;
	DistortionBufferType* pData_distortion;
	unsigned int bufferNumber;

	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&proj, &proj);

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) )
		return false;

	pData_matrix = (MatrixBufferType*)mappedResource.pData;

	pData_matrix->world = world;
	pData_matrix->view = view;
	pData_matrix->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);
	
	bufferNumber = 0;

	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_pMatrixBuffer);

	// Noise buffer
	result = pDeviceContext->Map(this->m_pNoiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) )
		return false;

	pData_noise = (NoiseBufferType*)mappedResource.pData;

	pData_noise->frameTime = frameTime;
	pData_noise->scrollSpeeds = scrollSpeeds;
	pData_noise->scales = scales;
	pData_noise->padding = 0.0f;

	pDeviceContext->Unmap(this->m_pNoiseBuffer, 0);
	
	bufferNumber = 1;

	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_pNoiseBuffer);

	pDeviceContext->PSSetShaderResources(0, 1, &ppTextureArray[0]);
	pDeviceContext->PSSetShaderResources(1, 1, &ppTextureArray[1]);
	pDeviceContext->PSSetShaderResources(2, 1, &ppTextureArray[2]);

	// Distortion
	result = pDeviceContext->Map(this->m_pDistortionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( FAILED(result) )
		return false;

	pData_distortion = (DistortionBufferType*)mappedResource.pData;

	pData_distortion->distortion1 = distortion1;
	pData_distortion->distortion2 = distortion2;
	pData_distortion->distortion3 = distortion3;
	pData_distortion->distortionScale = distortionScale;
	pData_distortion->distortionBias = distortionBias;

	pDeviceContext->Unmap(this->m_pDistortionBuffer, 0);

	bufferNumber = 0;

	pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &this->m_pDistortionBuffer);

	return true;
}

void CGEFireShader::_render_shaders(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->PSSetSamplers(1, 1, &this->m_pClampSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}