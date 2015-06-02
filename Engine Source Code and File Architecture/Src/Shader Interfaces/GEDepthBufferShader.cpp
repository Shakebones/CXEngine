/*--------------------------------------------------------------------------
 * File name - GEDepthBufferShader.cpp
 * Author - CuiXue
 * Updated date - 2013.7.16
 * Desc - 
 *
 *--------------------------------------------------------------------------*/
#include "GEDepthBufferShader.h"

CGEDepthBufferShader::CGEDepthBufferShader()
{
	this->m_pVertexShader = 0;
	this->m_pPixelShader = 0;
	this->m_pInputlayout = 0;
	this->m_pBasicSampler = 0;
	this->m_pMatrixBuffer = 0;
}

CGEDepthBufferShader::CGEDepthBufferShader(const CGEDepthBufferShader& source)
{

}

CGEDepthBufferShader& CGEDepthBufferShader::operator=(const CGEDepthBufferShader& source)
{
	return *this;
}

CGEDepthBufferShader::~CGEDepthBufferShader()
{
}

bool CGEDepthBufferShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	
	result = this->_initialize_shaders(pDevice, hWnd, L"../Include/Shaders/ZBuffer.vs",
													  L"../Include/Shaders/ZBuffer.ps");
	if( !result )
		return false;

	return true;
}

void CGEDepthBufferShader::Shutdown()
{
	this->_shutdown();
}

bool CGEDepthBufferShader::SetParams(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	bool result;

	result = this->_set_shader_params(pDeviceContext, world, view, proj);
	if( !result )
		return false;

	return true;
}

bool CGEDepthBufferShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	bool result;
	
	result = this->_render(pDeviceContext, indexCount);
	if( !result )
		return false;

	return true;
}

/*--------------------------------------------------------------------------
 * Desc - 
 *
 * Kernels of interfaces
 *
 *--------------------------------------------------------------------------*/
bool CGEDepthBufferShader::_initialize_shaders(ID3D11Device* pDevice, HWND hWnd, WCHAR* vsFilename, WCHAR* psFilename /* May need more other shaders like hull shader*/)
{
	HRESULT result = E_FAIL;
	ID3D10Blob* pVertexShaderBuf = 0;
	ID3D10Blob* pPixelShaderBuf = 0;
	ID3D10Blob* pErrorMessage = 0;

	int numElements;
	const int numPolygonLayout = 1;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[numPolygonLayout];
	D3D11_SAMPLER_DESC basicSamplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ZBufferVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pVertexShaderBuf, &pErrorMessage, NULL);
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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ZBufferPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pPixelShaderBuf, &pErrorMessage, NULL);
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

	result = pDevice->CreateVertexShader(pVertexShaderBuf->GetBufferPointer(), pVertexShaderBuf->GetBufferSize(), NULL, &this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pPixelShaderBuf->GetBufferPointer(), pPixelShaderBuf->GetBufferSize(), NULL, &this->m_pPixelShader);
	if( FAILED(result) )
		return false;

	// Polygon layout
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = pDevice->CreateInputLayout(polygonLayout, numElements, pVertexShaderBuf->GetBufferPointer(), pVertexShaderBuf->GetBufferSize(), &this->m_pInputlayout);
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<ID3D10Blob*>(pVertexShaderBuf);
	::ReleaseWithoutDel<ID3D10Blob*>(pPixelShaderBuf);

	// Buffers description
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	// Sampler description
	basicSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	basicSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	basicSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	basicSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	basicSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	basicSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	basicSamplerDesc.MinLOD = 0.0f;
	basicSamplerDesc.MaxAnisotropy = 1;
	basicSamplerDesc.MipLODBias = 0.0f;
	basicSamplerDesc.BorderColor[0] = 0.0f;
	basicSamplerDesc.BorderColor[1] = 0.0f;
	basicSamplerDesc.BorderColor[2] = 0.0f;
	basicSamplerDesc.BorderColor[3] = 0.0f;

	result = pDevice->CreateSamplerState(&basicSamplerDesc, &this->m_pBasicSampler);
	if( FAILED(result) )
		return false;

	return true;
}

void CGEDepthBufferShader::_shutdown()
{
	::ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
	::ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	::ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pInputlayout);
	::ReleaseWithoutDel<ID3D11SamplerState*>(this->m_pBasicSampler);
}

void CGEDepthBufferShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* pShaderFilename)
{
	char* pCompileError = 0;
	unsigned long bufferSize = 0;
	fstream fout;

	pCompileError = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");
	for(int i = 0; i != bufferSize; i++)
	{
		fout << pCompileError[i];
	}
	fout.close();

	MessageBox(hWnd, L"Compile shader error. Check shader-error.txt for information.", pShaderFilename, MB_OK); 
}

bool CGEDepthBufferShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	MatrixBufferType* pData_matrix;
	unsigned int numBuffer;

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

	numBuffer = 0;

	pDeviceContext->VSSetConstantBuffers(numBuffer, 1, &this->m_pMatrixBuffer);

	return true;
}

bool CGEDepthBufferShader::_render(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pInputlayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &this->m_pBasicSampler);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return true;
}