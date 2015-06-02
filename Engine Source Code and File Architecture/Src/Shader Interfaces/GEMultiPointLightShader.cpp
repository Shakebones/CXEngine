/*
 * File name - GEPointLightShader.cpp
 * Author - CuiXue
 * Updated date - 2013.5.2
 * Desc - 
 *
 */
#include "GEMultiPointLightShader.h"

CGEMultiPointLightShader::CGEMultiPointLightShader()
{
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pSamplerState = 0;
	this->m_pLightPosition = 0;
	this->m_pLightColor = 0;
	this->m_pInputLayout = 0;
}

CGEMultiPointLightShader::CGEMultiPointLightShader(const CGEMultiPointLightShader& source)
{
}

CGEMultiPointLightShader& CGEMultiPointLightShader::operator=(const CGEMultiPointLightShader& source)
{
	return *this;
}

CGEMultiPointLightShader::~CGEMultiPointLightShader()
{
}

/*
 * Desc - 
 * 如果增加新的渲染阶段的 shader，在此函数中增需要读取新的shader
 */
bool CGEMultiPointLightShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;

	// 如果有新的渲染阶段的shader，在kernel中增加编译
	result = this->_initialize_shaders(pDevice, hWnd, L"../Include/Shaders/MultiPointLight.vs",
													  L"../Include/Shaders/MultiPointLight.ps");
	if( !result )
		return false;

	return true;
}

void CGEMultiPointLightShader::Shutdown()
{
	this->_shutdown();
}

bool CGEMultiPointLightShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, 
								 D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
								 ID3D11ShaderResourceView* pTexture, int numLights,
								 D3DXVECTOR4 lightColor[],
								 D3DXVECTOR4 lightPosition[])
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj, pTexture, numLights, lightColor, lightPosition);
	if( !result )
		return false;

	this->_render_shaders(pDeviceContext, indexCount);

	return true;
}

/*
 * Desc -
 * 增加或删除 ID3D11Buffer 时，在此函数中修改 增加或删除 相应
 * 结构及变量，位置随意；如果有新的渲染阶段shader，在此函数中增
 * 加编译相应的shader文件
 */
bool CGEMultiPointLightShader::_initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightColorBufferDesc;
	D3D11_BUFFER_DESC lightPositionBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	ID3D10Blob* pVertexShaderBuf;
	ID3D10Blob* pPixelShaderBuf;
	ID3D10Blob* pErrorMessage;

	// 修改此处
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	unsigned int numElem;

	pVertexShaderBuf = 0;
	pPixelShaderBuf = 0;
	pErrorMessage = 0;
	
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "MultiPointLightVS", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pVertexShaderBuf, &pErrorMessage,
		NULL);
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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "MultiPointLightPS", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pPixelShaderBuf, &pErrorMessage,
		NULL);
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

	/*
	 * Desc - 修改此处
	 * 增加顶点的 Layout 属性
	 */

	/////////////////////////////////////////////////////////////
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
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
	/////////////////////////////////////////////////////////////

	/*
	 * 修改结束
	 */

	numElem = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = pDevice->CreateInputLayout(polygonLayout, numElem, pVertexShaderBuf->GetBufferPointer(),
		pVertexShaderBuf->GetBufferSize(), &this->m_pInputLayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShaderBuf);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShaderBuf);

	/*
	 * 添加新的buffer描述结构
	 */
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightColorBufferDesc.ByteWidth = sizeof(LightColorBufferType);
	lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightColorBufferDesc.MiscFlags = 0;
	lightColorBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&lightColorBufferDesc, NULL, &this->m_pLightColor);
	if( FAILED(result) )
		return false;

	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&lightPositionBufferDesc, NULL, &this->m_pLightPosition);
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

/*
 * Desc -
 * 增加或删除 ID3D11Buffer 时，在此函数中修改 增加或删除 相应
 * 结构及变量，位置随意
 */
void CGEMultiPointLightShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pSamplerState);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pLightPosition);
	::ReleaseWithoutDel<ID3D11Buffer*>(this->m_pLightColor);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputLayout);
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
}

void CGEMultiPointLightShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError;
	unsigned int bufferSize;
	ofstream fout;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();

	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader_error.txt");

	for(int i = 0; i != bufferSize; i++)
	{
		fout << pCompileError[i];
	}

	fout.close();

	::ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);

	MessageBox(hWnd, L"Compiling shader error. Check shader-error.txt for information.", pShaderFilename, MB_OK);

	return;
}

/*
 * Desc -
 * 增加相应的 shader 常量缓存时，在此函数中增加相应的映射关系
 */
bool CGEMultiPointLightShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, 
											 D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj,
											 ID3D11ShaderResourceView* pTexture, int numLights, D3DXVECTOR4 lightColor[], D3DXVECTOR4 lightPosition[])
{
	HRESULT result;

	unsigned int bufferNum;

	D3D11_MAPPED_SUBRESOURCE mappedResources;

	MatrixBufferType* pData_matrix;
	LightPositionBufferType* pData_light_position;
	LightColorBufferType* pData_light_color;

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

	result = pDeviceContext->Map(this->m_pLightPosition, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_light_position = (LightPositionBufferType*)mappedResources.pData;

	for(int i = 0; i != NUM_LIGHTS; i++)
	{
		pData_light_position->lightPosition[i] = lightPosition[i];
	}

	pDeviceContext->Unmap(this->m_pLightPosition, 0);

	bufferNum = 1;

	pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &this->m_pLightPosition);

	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	result = pDeviceContext->Map(this->m_pLightColor, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if( FAILED(result) )
		return false;

	pData_light_color = (LightColorBufferType*)mappedResources.pData;

	for(int i = 0; i != NUM_LIGHTS; i++)
	{
		pData_light_color->lightColor[i] = lightColor[i];
	}

	bufferNum = 0;

	pDeviceContext->PSSetConstantBuffers(bufferNum, 1, &this->m_pLightColor);

	return true;
}

/*
 * Desc -
 * 如果增加自定义的渲染阶段，在此函数中修改
 * 例如：在 D3D11 中增加曲面细分阶段，需要增加 Hull Shader & Domain Shader 过程
 */
void CGEMultiPointLightShader::_render_shaders(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pSamplerState);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}