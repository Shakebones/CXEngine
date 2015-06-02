/*
 * File name - GEColorShader.cpp
 * Author - CuiXue
 * Updated date - 2012.12.12
 * Desc - 
 *
 */
#include "GEColorShader.h"

////////////////////////////////////////////////////////////
//////////////////////// Interfaces ////////////////////////

CGEColorShader::CGEColorShader()
{
	this->m_pLayout = 0;
	this->m_pMatrixBuffer = 0;
	this->m_pPixelShader = 0;
	this->m_pVertexShader = 0;
}

CGEColorShader::CGEColorShader(const CGEColorShader& source)
{
}

CGEColorShader& CGEColorShader::operator=(const CGEColorShader& source)
{
	return *this;
}

CGEColorShader::~CGEColorShader()
{
}

bool CGEColorShader::Initialize(ID3D11Device* pDevice, HWND hWnd)
{
	bool result;
	result = this->_initialize_shader(pDevice, hWnd, L"..\\Include\\Shaders\\Color.vs",
		L"..\\Include\\Shaders\\Color.ps");
	if( !result )
		return false;

	return true;
}

void CGEColorShader::Shutdown()
{
	this->_shutdown();
}

bool CGEColorShader::Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
							D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
{
	bool result;
	result = this->_set_shader_params(pDeviceContext, world, view, proj);
	if( !result )
		return false;

	this->_render_shader(pDeviceContext, indexCount);
	return true;
}

/////////////////////////////////////////////////////////
//////////////////////// Kernels ////////////////////////
bool CGEColorShader::_initialize_shader(ID3D11Device* pDevice, HWND hWnd, WCHAR* vs, WCHAR* ps)
{
	HRESULT result;
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	ID3D10Blob* pixelShaderBuffer = 0;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	D3D11_BUFFER_DESC matrixBufferDesc;
	unsigned int num_of_elements;

	// Compile vertex shader
	result = ::D3DX11CompileFromFile(vs, NULL, NULL, "ColorVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, 
		NULL);

	// Show error code
	if( FAILED(result) )
	{
		if( errorMessage )
			this->_output_error_message(errorMessage, hWnd, vs);
		else
			::MessageBox(hWnd, vs, L"Missing shader file", MB_OK);
		return false;
	}

	// Compile pixel shader
	result = ::D3DX11CompileFromFile(ps, NULL, NULL, "ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

	if( FAILED(result) )
	{
		if( errorMessage )
			this->_output_error_message(errorMessage, hWnd, ps);
		else
			::MessageBox(hWnd, ps, L"Missing shader file", MB_OK);
		return false;
	}

	// Create vertex shader & pixel shader
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		NULL, &this->m_pVertexShader);
	if( FAILED(result) )
		return false;

	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		NULL, &this->m_pPixelShader);
	if( FAILED(result) )
		return false;

	/*
	 * The final thing that needs to be setup to utilize the shader is the constant buffer. 
	 * As you saw in the vertex shader we currently have just one constant buffer so we only 
	 * need to setup one here so we can interface with the shader. The buffer usage needs to
	 * be set to dynamic since we will be updating it each frame. The bind flags indicate that
	 * this buffer will be a constant buffer. The cpu access flags need to match up with the 
	 * usage so it is set to D3D11_CPU_ACCESS_WRITE. Once we fill out the description we can 
	 * then create the constant buffer interface and then use that to access the internal 
	 * variables in the shader using the function SetShaderParameters.
	 */
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	num_of_elements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = pDevice->CreateInputLayout(polygonLayout, num_of_elements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &this->m_pLayout);
	if( FAILED(result) )
		return false;

	ReleaseWithoutDel<ID3D10Blob*>(vertexShaderBuffer);
	ReleaseWithoutDel<ID3D10Blob*>(pixelShaderBuffer);

	// Set matrix constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->m_pMatrixBuffer);
	if( FAILED(result) )
		return false;

	return true;
}

void CGEColorShader::_shutdown()
{
	ReleaseWithoutDel<ID3D11Buffer*>(this->m_pMatrixBuffer);
	ReleaseWithoutDel<ID3D11InputLayout*>(this->m_pLayout);
	ReleaseWithoutDel<ID3D11PixelShader*>(this->m_pPixelShader);
	ReleaseWithoutDel<ID3D11VertexShader*>(this->m_pVertexShader);
}

void CGEColorShader::_output_error_message(ID3D10Blob* pErrorMessage, HWND hWnd, WCHAR* shaderFilename)
{
	char* compileErrors = NULL;
	unsigned long bufferSize;
	ofstream fout;

	compileErrors = (char*)pErrorMessage->GetBufferPointer();
	bufferSize = pErrorMessage->GetBufferSize();

	fout.open("../GESystem/Error/compiling-shader-error.txt");

	for(unsigned long i = 0; i < bufferSize; i++)
		fout << compileErrors[i];

	fout.close();

	ReleaseWithoutDel<ID3D10Blob*>(pErrorMessage);
	::MessageBox(hWnd, L"Compiling shader error. Check \\Error\\compiling-shader-error.txt for information.",
		shaderFilename, MB_OK);
}

bool CGEColorShader::_set_shader_params(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX world, D3DXMATRIX view,
										D3DXMATRIX proj)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrix_data_ptr;
	unsigned int buffer_number;
	
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&proj, &proj);

	// Lock the constant buffer for written.
	result = pDeviceContext->Map(this->m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD,
		0, &mappedResource);
	if( FAILED(result) )
		return false;

	matrix_data_ptr = (MatrixBufferType*)mappedResource.pData;
	matrix_data_ptr->world = world;
	matrix_data_ptr->view = view;
	matrix_data_ptr->proj = proj;

	pDeviceContext->Unmap(this->m_pMatrixBuffer, 0);
	buffer_number = 0;

	pDeviceContext->VSSetConstantBuffers(buffer_number, 1, &this->m_pMatrixBuffer);
	return true;
}

void CGEColorShader::_render_shader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
	pDeviceContext->IASetInputLayout(this->m_pLayout);
	pDeviceContext->VSSetShader(this->m_pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(this->m_pPixelShader, NULL, 0);
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}