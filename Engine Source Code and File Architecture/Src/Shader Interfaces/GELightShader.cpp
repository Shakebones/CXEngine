/*
 * File name - GELightShader.cpp
 * Author - CuiXue
 * Updated date - 2012.12.26
 * Desc -
 *
 */
#include "GELightShader.h"

CGELightShader::CGELightShader()
{
	this->m_pInputLayout = 0;
	this->m_pVertexShader = 0;
	this->m_pHullShader = 0;
	this->m_pDomainShader = 0;
	this->m_pPixelShader = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pTessellationBuffer = 0;
	this->m_pSampleState = 0;
	this->m_pLightBuffer = 0;
	this->m_pCameraBuffer = 0;
}

CGELightShader::CGELightShader(const CGELightShader& source)
{
}

CGELightShader& CGELightShader::operator=(const CGELightShader& source)
{
	return *this;
}

CGELightShader::~CGELightShader()
{
	this->_shutdown();
}

bool CGELightShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	result = this->_initialize_shader(pDevice, hWnd, L"..\\Include\\Shaders\\Light.vs", 
		L"..\\Include\\Shaders\\Light.ps");
	if( !result )
		return false;

	return true;
}

bool CGELightShader::Initialize_DX11Instance(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	result = this->_initialize_shader_dx11_instance(pDevice, hWnd, L"..\\Include\\Shaders\\Light_instance.vs", 
		L"..\\Include\\Shaders\\Light_instance.ps");
	if( !result )
		return false;

	return true;
}

bool CGELightShader::Initialize_Tessellation(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	result = this->_initialize_shader_tessellation(pDevice, hWnd, 
		L"..\\Include\\Shaders\\Light_tessellation.hlsl");

	if( !result )
		return false;

	return true;
}

void CGELightShader::Shutdown()
{
	this->_shutdown();
}

bool CGELightShader::Render(ID3D11DeviceContext* pDeviceContext, 
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
							float specularPower)
{
	bool result;
	result = this->_set_shader_params(pDeviceContext, world, view, proj,
		pTexture, lightDirection, ambientColor, diffuseColor, cameraPos, specularColor, specularPower);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);

	return true;
}

bool CGELightShader::Render(ID3D11DeviceContext* pDeviceContext,
							int vertexCount,
							int instanceCount,
							D3DXMATRIX world,
							D3DXMATRIX view, 
							D3DXMATRIX proj,
							ID3D11ShaderResourceView* pTexture,
							D3DXVECTOR3 lightDirection,
							D3DXVECTOR4 ambientColor,
							D3DXVECTOR4 diffuseColor,
							D3DXVECTOR4 specularColor,
							D3DXVECTOR3 cameraPos,
							float specularPower)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, pTexture, lightDirection, 
		ambientColor, diffuseColor, cameraPos, specularColor, specularPower);

	if( !result )
		return false;

	this->_render_shader(pDeviceContext, vertexCount, instanceCount);

	return true;
}

bool CGELightShader::RenderTessellation(ID3D11DeviceContext* pDeviceContext,
							int indexCount,
							ID3D11ShaderResourceView** ppTexture,
							GE_UpdateConstantBuffer* pUpdateCB,
							GE_Material_ConstantBufferType* pMaterialCB)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, ppTexture, pUpdateCB, pMaterialCB);
	if( !result )
		return false;

	this->_render_shader_tessellation(pDeviceContext, indexCount);

	return true;
}

bool CGELightShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd, WCHAR* vs, WCHAR* ps)
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

	pErrorMessage = 0;
	pVertexShader = 0;
	pPixelShader = 0;

	result = D3DX11CompileFromFile(vs, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	result = D3DX11CompileFromFile(ps, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	result = pDevice->CreateSamplerState(&samplerDesc, &this->m_pSampleState);
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

	return true;
}

bool CGELightShader::_initialize_shader_dx11_instance(ID3D11Device* pDevice, HWND hWnd, WCHAR* vs, WCHAR* ps)
{
	HRESULT result;
	ID3D10Blob*					pErrorMessage;
	ID3D10Blob*					pVertexShader;
	ID3D10Blob*					pPixelShader;

	D3D11_INPUT_ELEMENT_DESC	polygonLayout[4];
	unsigned int				numElements;
	D3D11_SAMPLER_DESC			samplerDesc;
	D3D11_BUFFER_DESC			matrixBufferDesc;
	D3D11_BUFFER_DESC			cameraBufferDesc;
	D3D11_BUFFER_DESC			lightBufferDesc;

	pErrorMessage = 0;
	pVertexShader = 0;
	pPixelShader = 0;

	result = D3DX11CompileFromFile(vs, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	result = D3DX11CompileFromFile(ps, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	polygonLayout[3].SemanticName = "TEXCOORD";
	polygonLayout[3].SemanticIndex = 1;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 1;
	polygonLayout[3].AlignedByteOffset = 0;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[3].InstanceDataStepRate = 1;

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

	result = pDevice->CreateSamplerState(&samplerDesc, &this->m_pSampleState);
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

	return true;
}

bool CGELightShader::_initialize_shader_tessellation(ID3D11Device* pDevice, HWND hWnd, WCHAR* hlsl)
{
	HRESULT result;
	ID3D10Blob*					pErrorMessage;
	ID3D10Blob*					pVertexShader;
	ID3D10Blob*					pHullShader;
	ID3D10Blob*					pDomainShader;
	ID3D10Blob*					pPixelShader;

	D3D11_INPUT_ELEMENT_DESC	polygonLayout[5];
	unsigned int				numElements;
	D3D11_SAMPLER_DESC			samplerDesc;
	D3D11_BUFFER_DESC			updateConstantBuffer;
	D3D11_BUFFER_DESC			materialConstantBuffer;

	pErrorMessage = 0;
	pVertexShader = 0;
	pPixelShader = 0;
	pHullShader = 0;
	pDomainShader = 0;

	result = D3DX11CompileFromFile(hlsl, NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pVertexShader, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, hlsl);
		}

		else
		{
			::MessageBox(hWnd, hlsl, L"Missing shader file", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(hlsl, NULL, NULL, "HS", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pHullShader, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, hlsl);
		}

		else
		{
			MessageBox(hWnd, hlsl, L"Missing shader file", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(hlsl, NULL, NULL, "DS", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pDomainShader, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, hlsl);
		}

		else
		{
			MessageBox(hWnd, hlsl, L"Missing shader file", MB_OK);
		}
		
		return false;
	}

	result = D3DX11CompileFromFile(hlsl, NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &pPixelShader, &pErrorMessage, NULL);
	if( FAILED(result) )
	{
		if( pErrorMessage )
		{
			this->_output_error_message(pErrorMessage, hWnd, hlsl);
		}

		else
		{
			::MessageBox(hWnd, hlsl, L"Missing shader file.", MB_OK);
		}

		return false;
	}

	result = pDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL,
		&this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreateHullShader(pHullShader->GetBufferPointer(), pHullShader->GetBufferSize(), NULL,
		&this->m_pHullShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreateDomainShader(pDomainShader->GetBufferPointer(), pDomainShader->GetBufferSize(), NULL,
		&this->m_pDomainShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL,
		&this->m_pPixelShader);
	if( FAILED(result) )
		return false;

	// Input layout. (polygonLayout)
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

	polygonLayout[3].SemanticName = "BINORMAL";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "TANGENT";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShader->GetBufferPointer(),
		pVertexShader->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShader);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShader);
	::ReleaseWithoutDel<ID3D10Blob*>(pHullShader);
	::ReleaseWithoutDel<ID3D10Blob*>(pDomainShader);

	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = pDevice->CreateSamplerState(&samplerDesc, &this->m_pSampleState);
	if( FAILED(result) )
		return false;

	updateConstantBuffer.Usage = D3D11_USAGE_DYNAMIC;
	updateConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	updateConstantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	updateConstantBuffer.MiscFlags = 0;
	updateConstantBuffer.StructureByteStride = 0;
	updateConstantBuffer.ByteWidth = sizeof(GE_UpdateConstantBuffer);

	result = pDevice->CreateBuffer(&updateConstantBuffer, NULL, &this->m_pUpdateConstantBuffer);
	if (FAILED(result))
		return false;

	materialConstantBuffer.Usage = D3D11_USAGE_DYNAMIC;
	materialConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialConstantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialConstantBuffer.MiscFlags = 0;
	materialConstantBuffer.StructureByteStride = 0;
	materialConstantBuffer.ByteWidth = sizeof(GE_Material_ConstantBufferType);

	result = pDevice->CreateBuffer(&updateConstantBuffer, NULL, &this->m_pMaterialConstantBuffer);
	if (FAILED(result))
		return false;

	return true;
}

void CGELightShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pUpdateConstantBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMaterialConstantBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pTessellationBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pLightBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pCameraBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSampleState);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
	::ReleaseWithoutDel<ID3D11HullShader*>(this->m_pHullShader);
	::ReleaseWithoutDel<ID3D11DomainShader*>(this->m_pDomainShader);
}

void CGELightShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* shaderFilename)
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

bool CGELightShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, 
										D3DXMATRIX world,
										D3DXMATRIX view, 
										D3DXMATRIX proj, 
										ID3D11ShaderResourceView* pTexture,
										D3DXVECTOR3 lightDirection, 
										D3DXVECTOR4 ambientColor,
										D3DXVECTOR4 diffuseColor,
										D3DXVECTOR3 cameraPos,
										D3DXVECTOR4 specularColor,
										float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	unsigned int bufferNum;
	MatrixBufferType* pData_matrix;
	LightBufferType*  pData_light;
	CameraBufferType* pData_camera;

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

	return true;
}

bool CGELightShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, 
										ID3D11ShaderResourceView** pTexture,
										GE_UpdateConstantBuffer* pUpdateCB,
										GE_Material_ConstantBufferType* pMaterialCB)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	GE_UpdateConstantBuffer* pData_update;
	GE_Material_ConstantBufferType* pData_material;
	ID3D11Buffer* pBuffer[2];
	ID3D11ShaderResourceView* pSRV[2];

	::D3DXMatrixTranspose(&pUpdateCB->mWorld, &pUpdateCB->mWorld);
	::D3DXMatrixTranspose(&pUpdateCB->mViewProjection, &pUpdateCB->mViewProjection);
	::D3DXMatrixTranspose(&pUpdateCB->mWorldViewProjection, &pUpdateCB->mWorldViewProjection);
	::D3DXMatrixTranspose(&pUpdateCB->mView, &pUpdateCB->mView);
	::D3DXMatrixTranspose(&pUpdateCB->mProjection, &pUpdateCB->mProjection);
	::D3DXMatrixTranspose(&pUpdateCB->mInvView, &pUpdateCB->mInvView);

	// Material
	result = pDeviceContext->Map(this->m_pUpdateConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&mappedResources);
	if( FAILED(result) )
		return false;

	pData_update = (GE_UpdateConstantBuffer*)mappedResources.pData;
	pData_update->mWorld = pUpdateCB->mWorld;
	pData_update->mProjection = pUpdateCB->mProjection;
	pData_update->mView = pUpdateCB->mView;
	pData_update->mViewProjection = pUpdateCB->mViewProjection;
	pData_update->mWorldViewProjection = pUpdateCB->mWorldViewProjection;
	pData_update->vDetailTessellationHeightScale = pData_update->vDetailTessellationHeightScale;
	pData_update->vGridSize = pUpdateCB->vGridSize;
	pData_update->vMeshColor = pUpdateCB->vMeshColor;
	pData_update->vScreenResolution = pUpdateCB->vScreenResolution;
	pData_update->vTessellationFactor = pUpdateCB->vTessellationFactor;
	pData_update->mInvView = pUpdateCB->mInvView;
	pDeviceContext->Unmap(this->m_pUpdateConstantBuffer, 0);

	result = pDeviceContext->Map(this->m_pMaterialConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_material = (GE_Material_ConstantBufferType*)mappedResources.pData;
	pData_material->g_materialAmbientColor	= pMaterialCB->g_materialAmbientColor;
	pData_material->g_materialDiffuseColor 	= pMaterialCB->g_materialDiffuseColor;
	pData_material->g_materialSpecularColor	= pMaterialCB->g_materialSpecularColor;
	pData_material->g_fSpecularExponent    	= pMaterialCB->g_fSpecularExponent;
	pData_material->g_LightPosition        	= pMaterialCB->g_LightPosition;
	pData_material->g_LightDiffuse        	= pMaterialCB->g_LightDiffuse;
	pData_material->g_LightAmbient         	= pMaterialCB->g_LightAmbient;
	pData_material->g_vEye            		= pMaterialCB->g_vEye;
	pData_material->g_fBaseTextureRepeat	= pMaterialCB->g_fBaseTextureRepeat;
	pData_material->g_fPOMHeightMapScale  	= pMaterialCB->g_fPOMHeightMapScale;
	pData_material->g_fShadowSoftening   	= pMaterialCB->g_fShadowSoftening;
	pData_material->g_nMinSamples    		= pMaterialCB->g_nMinSamples;
	pData_material->g_nMaxSamples    		= pMaterialCB->g_nMaxSamples;
	pDeviceContext->Unmap(this->m_pMaterialConstantBuffer, 0);

	pBuffer[0] = this->m_pUpdateConstantBuffer;
	pBuffer[1] = this->m_pMaterialConstantBuffer;
	pDeviceContext->VSSetConstantBuffers( 0, 2, pBuffer );
    pDeviceContext->HSSetConstantBuffers( 0, 2, pBuffer );
    pDeviceContext->DSSetConstantBuffers( 0, 2, pBuffer );
    pDeviceContext->PSSetConstantBuffers( 0, 2, pBuffer );

	pSRV[0] = pTexture[0];
	pSRV[1] = pTexture[1];
	pDeviceContext->VSSetShaderResources(0, 2, pSRV);
	pDeviceContext->DSSetShaderResources(0, 2, pSRV);
	pDeviceContext->PSSetShaderResources(0, 2, pSRV);

	return true;
}

void CGELightShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSampleState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

void CGELightShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int vertexCount, int instanceCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSampleState);
	pDeviceContext->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void CGELightShader::_render_shader_tessellation(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetSamplers(0, 1, &this->m_pSampleState);
	pDeviceContext->DSSetSamplers(0, 1, &this->m_pSampleState);
	pDeviceContext->HSSetSamplers(0, 1, &this->m_pSampleState);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSampleState);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->HSSetShader(this->m_pHullShader, NULL, 0);
	pDeviceContext->DSSetShader(this->m_pDomainShader, NULL, 0);
	pDeviceContext->GSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

