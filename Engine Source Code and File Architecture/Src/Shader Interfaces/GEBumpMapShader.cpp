/*
 * File name - GEBumpMapShader.cpp
 * Author - CuiXue
 * Updated date - 2013.3.11
 * Desc - 
 *
 */
#include "GEBumpMapShader.h"

CGEBumpMapShader::CGEBumpMapShader()
{
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pInputLayout = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pLightBuffer = 0;
	this->m_pCameraBuffer = 0;
	this->m_pSamplerState = 0;
}

CGEBumpMapShader::CGEBumpMapShader(const CGEBumpMapShader& source)
{
}

CGEBumpMapShader& CGEBumpMapShader::operator=(const CGEBumpMapShader& source)
{
	return *this;
}

CGEBumpMapShader::~CGEBumpMapShader()
{
}

bool CGEBumpMapShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	result = this->_initialize_shader(pDevice, hWnd,
		L"../Include/Shaders/BumpMap.vs", L"../Include/Shaders/BumpMap.ps");
	if( !result )
		return false;

	return true;
}

void CGEBumpMapShader::Shutdown()
{
	this->_shutdown();
}

bool CGEBumpMapShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
							  D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
							  ID3D11ShaderResourceView** pTextureArray, D3DXVECTOR3 lightDir,
							  D3DXVECTOR3 cameraPos,
							  D3DXVECTOR4 ambientColor,
							  D3DXVECTOR4 diffuseColor,
							  D3DXVECTOR4 specularColor,
							  float specularPower)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, pTextureArray, lightDir, 
		cameraPos, ambientColor, diffuseColor, specularColor, specularPower);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGEBumpMapShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* pVertexShaderBuf;
	ID3D10Blob* pPixelShaderBuf;
	ID3D10Blob* pErrorMessage;
	
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElem;

	pVertexShaderBuf = 0;
	pPixelShaderBuf = 0;
	pErrorMessage = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "BumpMapVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "BumpMapPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		NULL, 0, &pPixelShaderBuf, &pErrorMessage, NULL);
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	numElem = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = pDevice->CreateInputLayout(polygonLayout, numElem, pVertexShaderBuf->GetBufferPointer(),
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

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&cameraBufferDesc, NULL, &this->m_pCameraBuffer);
	if( FAILED(result) )
		return false;

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&lightBufferDesc, NULL, &this->m_pLightBuffer);
	if( FAILED(result) )
		return false;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
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

void CGEBumpMapShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pCameraBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pLightBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGEBumpMapShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
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

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", L"Error", MB_OK);
}

bool CGEBumpMapShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext,
										  D3DXMATRIX world,
										  D3DXMATRIX view,
										  D3DXMATRIX proj,
										  ID3D11ShaderResourceView** pTextureArray,
										  D3DXVECTOR3 lightDir,
										  D3DXVECTOR3 cameraPos,
										  D3DXVECTOR4 ambientColor,
										  D3DXVECTOR4 diffuseColor,
										  D3DXVECTOR4 specularColor,
										  float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	MatrixBufferType* pDataMatrix;
	LightBufferType* pDataLight;
	CameraBufferType* pDataCamera;
	unsigned int bufferNumber;

	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&proj, &proj);

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pDataMatrix = (MatrixBufferType*)mappedResources.pData;

	pDataMatrix->world = world;
	pDataMatrix->view = view;
	pDataMatrix->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	bufferNumber = 0;

	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 2, pTextureArray);

	result = pDeviceContext->Map(this->m_pCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pDataCamera = (CameraBufferType*)mappedResources.pData;

	pDataCamera->cameraPosition = cameraPos;
	pDataCamera->padding = 0.0f;

	pDeviceContext->Unmap(this->m_pCameraBuffer, 0);

	bufferNumber = 1;

	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_pCameraBuffer);

	result = pDeviceContext->Map(this->m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pDataLight = (LightBufferType*)mappedResources.pData;
	pDataLight->ambientColor = ambientColor;
	pDataLight->diffuseColor = diffuseColor;
	pDataLight->direction = lightDir;
	pDataLight->specularColor = specularColor;
	pDataLight->specularPower = specularPower;

	pDeviceContext->Unmap(this->m_pLightBuffer, 0);

	bufferNumber = 0;

	pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &this->m_pLightBuffer);

	return true;
}

void CGEBumpMapShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
