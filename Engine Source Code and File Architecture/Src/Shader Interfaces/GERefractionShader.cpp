/*
 * File name - GELightShader.cpp
 * Author - CuiXue
 * Updated date - 2013.4.22
 * Desc -
 *
 */
#include "GERefractionShader.h"

CGERefractionShader::CGERefractionShader()
{
	this->m_pInputLayout = 0;
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pSamplerState = 0;
	this->m_pLightBuffer = 0;
	this->m_pCameraBuffer = 0;
	this->m_pClipPlaneBuffer = 0;
}

CGERefractionShader::CGERefractionShader(const CGERefractionShader& source)
{
}

CGERefractionShader& CGERefractionShader::operator=(const CGERefractionShader& source)
{
	return *this;
}

CGERefractionShader::~CGERefractionShader()
{
	this->_shutdown();
}

bool CGERefractionShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	result = this->_initialize_shader(pDevice, hWnd, L"../Include/Shaders/Refraction.vs", 
		L"../Include/Shaders/Refraction.ps");
	if( !result )
		return false;

	return true;
}

void CGERefractionShader::Shutdown()
{
	this->_shutdown();
}

bool CGERefractionShader::Render(ID3D11DeviceContext* pDeviceContext, 
							int indexCount,
							D3DXMATRIX world,
							D3DXMATRIX view,
							D3DXMATRIX proj,
							ID3D11ShaderResourceView* pTexture,
							D3DXVECTOR3 lightDirection,
							D3DXVECTOR4 ambientColor,
							D3DXVECTOR4 diffuseColor,
							D3DXVECTOR4 specularColor,
							D3DXVECTOR3 cameraPos,
							float specularPower,
							D3DXVECTOR4 clipPlane)
{
	bool result;
	result = this->_set_shader_params(pDeviceContext, world, view, proj,
		pTexture, lightDirection, ambientColor, diffuseColor, cameraPos, specularColor, specularPower, clipPlane);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGERefractionShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd, WCHAR* vs, WCHAR* ps)
{
	HRESULT result;
	ID3D10Blob*					pErrorMessage;
	ID3D10Blob*					pVertexShader;
	ID3D10Blob*					pPixelShader;

	D3D11_INPUT_ELEMENT_DESC	polygonLayout[3];
	unsigned int				numElements;
	D3D11_SAMPLER_DESC			samplerDesc;
	D3D11_BUFFER_DESC			matrixBufferDesc;
	D3D11_BUFFER_DESC			cameraBufferDesc;
	D3D11_BUFFER_DESC			lightBufferDesc;
	D3D11_BUFFER_DESC			ClipPlaneBufferDesc;

	pErrorMessage = 0;
	pVertexShader = 0;
	pPixelShader = 0;

	result = D3DX11CompileFromFile(vs, NULL, NULL, "RefractionVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShader, &pErrorMessage, NULL);
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

	result = D3DX11CompileFromFile(ps, NULL, NULL, "RefractionPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pPixelShader, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, ps);
		}

		else
		{
			::MessageBox(hWnd, ps, L"Missing shader file.", MB_OK);
		}

		return false;
	}

	result = pDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL,
		&this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL,
		&this->m_pPixelShader);
	if( FAILED(result) )
		return false;

	// Input layout. (polygonLayout)
	// 1. Position.
	// 2. Texture coordinate.
	// 3. Normals.
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

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShader->GetBufferPointer(),
		pVertexShader->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShader);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShader);

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MipLODBias = 0.0f;
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
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);

	result = pDevice->CreateBuffer(&lightBufferDesc, NULL, &this->m_pLightBuffer);
	if( FAILED(result) )
		return false;

	ClipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ClipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ClipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ClipPlaneBufferDesc.MiscFlags = 0;
	ClipPlaneBufferDesc.StructureByteStride = 0;
	ClipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);

	result = pDevice->CreateBuffer(&ClipPlaneBufferDesc, NULL, &this->m_pClipPlaneBuffer);
	if( FAILED(result) )
		return false;

	return true;
}

void CGERefractionShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pClipPlaneBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pLightBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pCameraBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGERefractionShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* shaderFilename)
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

bool CGERefractionShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, 
											 D3DXMATRIX world,
											 D3DXMATRIX view, 
											 D3DXMATRIX proj, 
											 ID3D11ShaderResourceView* pTexture,
											 D3DXVECTOR3 lightDirection, 
											 D3DXVECTOR4 ambientColor,
											 D3DXVECTOR4 diffuseColor,
											 D3DXVECTOR3 cameraPos,
											 D3DXVECTOR4 specularColor,
											 float specularPower,
											 D3DXVECTOR4 clipPlane)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	unsigned int bufferNum;
	MatrixBufferType* pData_matrix;
	LightBufferType*  pData_light;
	CameraBufferType* pData_camera;
	ClipPlaneBufferType* pData_clipPlane;

	::D3DXMatrixTranspose(&world, &world);
	::D3DXMatrixTranspose(&view, &view);
	::D3DXMatrixTranspose(&proj, &proj);

	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD,
		0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_matrix = (MatrixBufferType*)mappedResources.pData;
	pData_matrix->world = world;
	pData_matrix->view = view;
	pData_matrix->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);

	bufferNum = 0;
	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &this->m_pMatrixBuffer);
	result = pDeviceContext->Map(this->m_pCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_camera = (CameraBufferType*)mappedResources.pData;
	pData_camera->cameraPosition = cameraPos;
	pData_camera->padding = 0.0f;
	pDeviceContext->Unmap(this->m_pCameraBuffer, 0);
	bufferNum = 1;
	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &this->m_pCameraBuffer);


	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	result = pDeviceContext->Map(this->m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&mappedResources);
	if( FAILED(result) )
		return false;

	pData_light = (LightBufferType*)mappedResources.pData;
	pData_light->ambientColor = ambientColor;
	pData_light->diffuseColor = diffuseColor;
	pData_light->direction = lightDirection;
	pData_light->specularColor = specularColor;
	pData_light->specularPower = specularPower;
	pDeviceContext->Unmap(this->m_pLightBuffer, 0);
	
	bufferNum = 0;
	pDeviceContext->PSSetConstantBuffers(bufferNum, 1, &this->m_pLightBuffer);

	result = pDeviceContext->Map(this->m_pClipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_clipPlane = (ClipPlaneBufferType*)mappedResources.pData;
	pData_clipPlane->clipPlane = clipPlane;

	pDeviceContext->Unmap(this->m_pClipPlaneBuffer, 0);

	bufferNum = 2;

	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &this->m_pClipPlaneBuffer);

	return true;
}

void CGERefractionShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

